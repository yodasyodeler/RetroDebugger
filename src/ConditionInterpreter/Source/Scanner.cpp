#include "Scanner.h"

#include "Report.h"

#include <map>

using namespace std::string_view_literals;

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
static constexpr bool IsAlpha(char character) {
    return ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || character == '_');
}

static constexpr bool IsDigit(char character) {
    return character >= '0' && character <= '9';
}

static constexpr bool IsAlphaNumeric(char character) {
    return IsAlpha(character) || IsDigit(character);
}


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
    const auto scanNum = [&]() { while (!IsAtEnd(cursor) && IsDigit(Peek(cursor))) { Pop(cursor); } };

    scanNum();
    // TODO: floating point may not work as expected with different floating point standards.
    if (Peek(cursor) == '.' && IsDigit(PeekAhead(cursor))) {
        Pop(cursor);
        scanNum(); // decimal
    }

    m_tokenList.emplace_back(TokenType::NUMBER,
        std::string_view(m_source.data() + cursor.start, cursor.current - cursor.start),
        std::stod(std::string(m_source.data() + cursor.start, cursor.current - cursor.start)),
        cursor.current);
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