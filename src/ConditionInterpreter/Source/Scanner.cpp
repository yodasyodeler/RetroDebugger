#include "Scanner.h"

#include "Report.h"

#include <algorithm>
#include <map>

using namespace std::string_view_literals;

namespace {
// TODO: this may be a good candidate for a constexpr map
static const std::map<std::string_view, TokenType> KeywordMap = {
    /*{ "and"sv, TokenType::AND },
    { "class"sv, TokenType::CLASS },
    { "else"sv, TokenType::ELSE },*/
    { "false"sv, TokenType::FALSE },
    /*{ "fun"sv, TokenType::FUN },
    { "for"sv, TokenType::FOR },
    { "if"sv, TokenType::IF },
    { "nil"sv, TokenType::NIL },
    { "or"sv, TokenType::OR },
    { "print"sv, TokenType::PRINT },
    { "return"sv, TokenType::RETURN },
    { "super"sv, TokenType::SUPER },
    { "this"sv, TokenType::THIS },*/
    { "true"sv, TokenType::TRUE },
    //{ "var"sv, TokenType::VAR },
    //{ "while"sv, TokenType::WHILE },
};

// std library methods are not constexpr, most likely not an issue though.
static constexpr bool IsLetter(char character) {
    return ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'));
}

static constexpr bool IsAlpha(char character) {
    return (IsLetter(character) || character == '_');
}

static constexpr bool IsNumeric(char character) {
    return character >= '0' && character <= '9';
}

static constexpr bool IsNumberValid(char character) {
    constexpr auto digitSeperator = '\'';
    return IsNumeric(character) || IsLetter(character) || character == digitSeperator;
}

static constexpr bool IsAlphaNumeric(char character) {
    return IsAlpha(character) || IsNumeric(character);
}

enum NumericBase {
    Binary = 2,
    Octal = 8,
    Decimal = 10,
    Hex = 16,
};

std::string to_string(NumericBase numericBase) {
    switch (numericBase) {
        case NumericBase::Binary:
            return "Binary";
        case NumericBase::Octal:
            return "Octal";
        case NumericBase::Decimal:
            return "Decimal";
        case NumericBase::Hex:
            return "Hex";
    }

    return fmt::format("Base-{}", static_cast<int>(numericBase));
}

}

namespace Rdb {

Scanner::Scanner(ErrorsPtr errors, std::string_view source) :
    m_source(source),
    m_errors(std::move(errors)) {
}

TokenList Scanner::ScanTokens() {
    Cursor cursor;

    while (!IsAtEnd(cursor)) {
        cursor.start = cursor.current;
        ScanToken(cursor);
    }
    m_tokenList.emplace_back(TokenType::END_OF_FILE, "", cursor.current);

    return m_tokenList;
}

void Scanner::ScanToken(Cursor& cursor) {
    const char character = Pop(cursor);

    const auto MatchChar = [&](const char character) {
        if (IsAtEnd(cursor) || character != m_source[cursor.current]) {
            return false;
        }
        cursor.current++; // Matched the character, advance the cursor
        return true;
    };

    const auto CreateCharToken = [&](TokenType tokenType) { return Token(tokenType, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), cursor.start); };
    switch (character) {
        // Unique one char Tokens
        case '(':
            m_tokenList.emplace_back(CreateCharToken(TokenType::LEFT_PAREN));
            break;
        case ')':
            m_tokenList.emplace_back(CreateCharToken(TokenType::RIGHT_PAREN));
            break;
        case '{':
            m_tokenList.emplace_back(CreateCharToken(TokenType::LEFT_BRACE));
            break;
        case '}':
            m_tokenList.emplace_back(CreateCharToken(TokenType::RIGHT_BRACE));
            break;
        case ':':
            m_tokenList.emplace_back(CreateCharToken(TokenType::COLON));
            break;
        case ',':
            m_tokenList.emplace_back(CreateCharToken(TokenType::COMMA));
            break;
        case '.':
            m_tokenList.emplace_back(CreateCharToken(TokenType::DOT));
            break;
        case '-':
            m_tokenList.emplace_back(CreateCharToken(TokenType::MINUS));
            break;
        case '+':
            m_tokenList.emplace_back(CreateCharToken(TokenType::PLUS));
            break;
        case '?':
            m_tokenList.emplace_back(CreateCharToken(TokenType::QUESTION));
            break;
        case '*':
            m_tokenList.emplace_back(CreateCharToken(TokenType::STAR));
            break;
        case '^':
            m_tokenList.emplace_back(CreateCharToken(TokenType::BITWISE_XOR));
            break;

        // One/Two char Tokens
        case '!':
            m_tokenList.emplace_back(CreateCharToken(MatchChar('=') ? TokenType::BANG_EQUAL : TokenType::BANG));
            break;
        case '=': // Only supports '=='
            if (MatchChar('=')) {
                m_tokenList.emplace_back(CreateCharToken(TokenType::EQUAL_EQUAL));
            }
            else {
                m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), "Unexpected character.");
            }
            break;
        case '<':
            m_tokenList.emplace_back(CreateCharToken(MatchChar('=') ? TokenType::LESS_EQUAL : TokenType::LESS));
            break;
        case '>':
            m_tokenList.emplace_back(CreateCharToken(MatchChar('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER));
            break;
        case '&':
            m_tokenList.emplace_back(CreateCharToken(MatchChar('&') ? TokenType::LOGIC_AND : TokenType::BITWISE_AND));
            break;
        case '|':
            m_tokenList.emplace_back(CreateCharToken(MatchChar('|') ? TokenType::LOGIC_OR : TokenType::BITWISE_OR));
            break;

        // One or many char Token
        case '/':
            if (MatchChar('/')) {
                auto c = '\0';
                while (!IsAtEnd(cursor) && c != '\n') { c = Pop(cursor); } // Loop till end of file or hit return
                if (c == '\n') { cursor.line++; } // Handle the '\n' case here as well
                break;
            }
            else if (MatchChar('*')) {
                while (!IsAtEnd(cursor) && (Peek(cursor) != '*' || PeekAhead(cursor) != '/')) {
                    const auto c = Pop(cursor);
                    if (c == '\n') { cursor.line++; } // Handle the '\n' case here as well
                }
                if (IsAtEnd(cursor)) { m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), "Couldn't find end of comment block"); }
                cursor.current += 2; // move cursor past '*/'
                break;
            }
            else {
                m_tokenList.emplace_back(CreateCharToken(TokenType::SLASH));
                break;
            }

        // Whitespace
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            cursor.line++;
            break;

        // numeric
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            ScanNumericLiteral(cursor);
            break;

        // Report an Error, unknown character
        default:
            if (IsAlpha(character)) {
                ScanIdentifier(cursor);
            }
            else {
                m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), "Unexpected character.");
            }
    }
}

void Scanner::ScanNumericLiteral(Cursor& cursor) {
    const auto scanNumber = [&]() { while (!IsAtEnd(cursor) && IsNumberValid(Peek(cursor))) { Pop(cursor); } };
    const auto stripNumberSeperator = [&]() {
        std::string numberStr = std::string(m_source.data() + cursor.start, cursor.current - cursor.start);
        const auto invalidRepeatedNumberSeperator = numberStr.find("''");
        if (invalidRepeatedNumberSeperator != std::string::npos) {
            m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), "Adjacent digit separators");
            return std::string{};
        }
        if (numberStr.back() == '\'') {
            m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), "Unexpected character.");
            return std::string{};
        }

        const auto numberSeperators = std::ranges::remove(numberStr, '\'');
        numberStr.erase(numberSeperators.begin(), numberSeperators.end());

        return numberStr;
    };
    auto AddNumberToken = [this, &cursor](auto number) {
        m_tokenList.emplace_back(TokenType::NUMBER,
            std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start),
            number,
            cursor.current);
    };


    // Check for base
    auto expectedBase = NumericBase::Decimal;
    if (m_source[cursor.start] == '0') {
        // Hex
        if (const auto value = Peek(cursor);
            value == 'x' || value == 'X') {
            Pop(cursor);
            expectedBase = NumericBase::Hex;
        }

        // Binary
        else if (value == 'b' || value == 'B') {
            Pop(cursor);
            expectedBase = NumericBase::Binary;
        }
        // Octal
        else {
            expectedBase = NumericBase::Octal;
        }
    }


    scanNumber();
    // TODO: floating point may not work as expected with different floating point standards.
    if (Peek(cursor) == '.' && IsNumeric(PeekAhead(cursor))) {
        Pop(cursor);
        const auto scanDecimal = [&]() { while (!IsAtEnd(cursor) && IsNumeric(Peek(cursor))) { Pop(cursor); } };
        scanDecimal(); // decimal

        try {
            auto number = std::stod(stripNumberSeperator(), nullptr);
            AddNumberToken(number);
        }
        catch (...) {
            m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), "Invalid floating-point number");
        }
    }
    else {
        try {
            if (auto numberStr = stripNumberSeperator();
                !numberStr.empty()) {

                size_t pos{};
                // stoi doesn't recognize '0b' prefix.
                if (expectedBase == NumericBase::Binary) {
                    pos += 2;
                    numberStr = std::string(numberStr.begin() + 2, numberStr.end());
                }
                AddNumberToken(std::stoi(numberStr, &pos, static_cast<int>(expectedBase)));

                // Error if we stopped sooner than expected
                if (pos != numberStr.size()) {
                    m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), fmt::format("Invalid {} number", to_string(expectedBase)));
                }
            }
        }
        catch (...) {
            m_errors->Report(cursor.line, std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start), fmt::format("Invalid {} number", to_string(expectedBase)));
        }
    }
}

void Scanner::ScanIdentifier(Cursor& cursor) {
    while (IsAlphaNumeric(Peek(cursor))) { Pop(cursor); }

    const auto GetTokenType = [](std::string_view lexeme) {
        const auto keyword = KeywordMap.find(lexeme);
        if (keyword == KeywordMap.end()) { return TokenType::IDENTIFIER; }
        return keyword->second;
    };

    const auto lexeme = std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start);

    if (const auto tokenType = GetTokenType(lexeme);
        tokenType == TokenType::TRUE) {
        m_tokenList.emplace_back(GetTokenType(lexeme), lexeme, true, cursor.current);
    }
    else if (tokenType == TokenType::FALSE) {
        m_tokenList.emplace_back(GetTokenType(lexeme), lexeme, false, cursor.current);
    }
    // else if (tokenType == TokenType::NIL) {
    //     m_tokenList.emplace_back(GetTokenType(lexeme), lexeme, NilType{}, cursor.current);
    // }
    else {
        m_tokenList.emplace_back(GetTokenType(lexeme), lexeme, cursor.current);
    }
}

bool Scanner::IsAtEnd(const Cursor& cursor) {
    return cursor.current >= m_source.size();
}

char Scanner::Peek(const Cursor& cursor) {
    if (IsAtEnd(cursor)) { return '\0'; }
    return m_source[cursor.current];
}

char Scanner::PeekAhead(Cursor cursor) {
    cursor.current++; // look ahead
    if (IsAtEnd(cursor)) { return '\0'; }
    return m_source[cursor.current];
}

char Scanner::Pop(Cursor& cursor) {
    return m_source[cursor.current++];
}
}