#include "Scanner.h"

#include "Report.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <array>
#include <stdexcept>

/* TODO:
    Error case: Unexpected character
    Error case: Unexpected second character
*/
using namespace std::string_view_literals;
using namespace Rdb;

[[nodiscard]] consteval std::string_view RemoveQuotes(std::string_view str) {
    if (str.front() != '"') { throw std::invalid_argument(R"(Doesn't start with '"')"); }
    if (str.back() != '"') { throw std::invalid_argument(R"(Doesn't end with '"')"); }
    str.remove_prefix(1);
    str.remove_suffix(1);
    return str;
}

TEST(ScannerTests, Scan_Comment) {
    constexpr auto source = "//This is a comment";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 1);

    ASSERT_EQ(tokens[0].GetType(), TokenType::END_OF_FILE);
}

TEST(ScannerTests, Scan_CommentWithNewLine) {
    constexpr auto source = "//This is a comment\n";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 1);

    ASSERT_EQ(tokens[0].GetType(), TokenType::END_OF_FILE);
}

TEST(ScannerTests, Scan_MultiLine) {
    constexpr auto source = R"(
/* This is a comment 
        123 * / they need to be next to eachother!
        /* repeat doesn't do anything
*/
)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 1);

    ASSERT_EQ(tokens[0].GetType(), TokenType::END_OF_FILE);
}

TEST(ScannerTests, Scan_Grouping) {
    constexpr auto source = "(( )){} // grouping stuff";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 7);

    constexpr auto expectedTokenTypes = std::array<TokenType, 7>{ TokenType::LEFT_PAREN, TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::RIGHT_PAREN, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE, TokenType::END_OF_FILE };
    for (int i = 0; const auto& expectedToken : expectedTokenTypes) {
        ASSERT_EQ(tokens[i].GetType(), expectedToken);
        if (i != tokens.size() - 1) { ASSERT_EQ(tokens[i].GetLexeme().size(), 1); }
        ++i;
    }
}

TEST(ScannerTests, Scan_Operators) {
    constexpr auto source = "!*+-/<> >= <= == //operators";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 11);

    constexpr auto expectedTokenTypes = std::array<TokenType, 11>{ TokenType::BANG, TokenType::STAR, TokenType::PLUS, TokenType::MINUS, TokenType::SLASH, TokenType::LESS, TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS_EQUAL, TokenType::EQUAL_EQUAL, TokenType::END_OF_FILE };
    constexpr auto doubleCharTokenTypes = std::array<TokenType, 4>{ TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL, TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL };
    for (int i = 0; const auto& expectedToken : expectedTokenTypes) {
        EXPECT_EQ(tokens[i].GetType(), expectedToken);

        const auto expectedLexemeSize = std::ranges::find(doubleCharTokenTypes, expectedToken) == doubleCharTokenTypes.end() ? 1 : 2;
        if (i != tokens.size() - 1) { ASSERT_EQ(tokens[i].GetLexeme().size(), expectedLexemeSize); }
        ++i;
    }
}

TEST(ScannerTests, Scan_NumericLiterals_Octal) {
    static constexpr auto source = "0123"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralDouble(), 83.0); // Octal value
}

TEST(ScannerTests, Scan_NumericLiterals_InvalidOctal_CausesError) {
    static constexpr auto source = "0181"sv; // 8 is not allowed in octal
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Invalid Octal number)"));
}

TEST(ScannerTests, Scan_NumericLiterals_Hex) {
    static constexpr auto source = "0x10"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralInt(), 16); // Hex value
}

TEST(ScannerTests, Scan_NumericLiterals_InvalidHex_CausesError) {
    static constexpr auto source = "0x1G1"sv; // G is not allowed in hex
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Invalid Hex number)"));
}

TEST(ScannerTests, Scan_NumericLiterals_Binary) {
    static constexpr auto source = "0b100"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralInt(), 4); // Hex value
}

TEST(ScannerTests, Scan_NumericLiterals_InvalidBinary_CausesError) {
    static constexpr auto source = "0b121"sv; // 2 is not allowed in binary
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Invalid Binary number)"));
}

TEST(ScannerTests, Scan_NumericLiterals_InvalidDecimal_CausesError) {
    static constexpr auto source = "1A1"sv; // A is not allowed in hex
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Invalid Decimal number)"));
}

TEST(ScannerTests, Scan_NumericLiterals_NumberSeperators_SeperatorsAreRemoved) {
    static constexpr auto source = "1'2'3"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralDouble(), 123.0);
}

TEST(ScannerTests, Scan_NumericLiterals_BankNotation_HappyPath) {
    static constexpr auto source = "1:3"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::BANK_NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralBankNumber(), (std::pair{ 1, 3 }));
}

TEST(ScannerTests, Scan_NumericLiterals_BankNotation_Hex_HappyPath) {
    static constexpr auto source = "0x5:0xA"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::BANK_NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralBankNumber(), (std::pair{ 5, 10 }));
}

TEST(ScannerTests, Scan_NumericLiterals_BankNotation_Binary_HappyPath) {
    static constexpr auto source = "0b101:0b1010"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::BANK_NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralBankNumber(), (std::pair{ 5, 10 }));
}

TEST(ScannerTests, Scan_NumericLiterals_BankNotationDoubleColon_NotASingleNumber) {
    static constexpr auto source = "1::3"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[1].GetType(), TokenType::COLON);
    EXPECT_EQ(tokens[2].GetType(), TokenType::COLON);
}

TEST(ScannerTests, Scan_NumericLiterals_BankNotationPrefix_NotASingleNumber) {
    static constexpr auto source = ":3"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].GetType(), TokenType::COLON);
}

TEST(ScannerTests, Scan_NumericLiterals_BankNotationSuffix_NotASingleNumber) {
    static constexpr auto source = "3:"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[1].GetType(), TokenType::COLON);
}

TEST(ScannerTests, Scan_NumericLiterals_NumberSeperators_DoubleSeperatorsError) {
    static constexpr auto source = "12''3"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Adjacent digit separators)"));
}

TEST(ScannerTests, Scan_NumericLiterals_NumberSeperators_BadUseOfSeperator_Error) {
    static constexpr auto source = "123'"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Unexpected character)"));
}

TEST(ScannerTests, Scan_NumericLiterals_LeadingPeriod) {
    static constexpr auto source = ".123"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 3);

    auto expectedLexeme = source;
    expectedLexeme.remove_prefix(1);
    EXPECT_EQ(tokens[1].GetType(), TokenType::NUMBER);
    EXPECT_EQ(tokens[1].GetLexeme(), expectedLexeme);
    EXPECT_EQ(tokens[1].GetLiteralDouble(), 123.0);
}

TEST(ScannerTests, Scan_NumericLiterals_TrailingPeriod) {
    static constexpr auto source = "123."sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 3);

    auto expectedLexeme = source;
    expectedLexeme.remove_suffix(1);
    EXPECT_EQ(tokens[0].GetType(), TokenType::NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), expectedLexeme);
    EXPECT_EQ(tokens[0].GetLiteralDouble(), 123.0);
}

TEST(ScannerTests, Scan_NumericLiterals_HappyPath) {

    static constexpr auto source = "0123.16"sv;
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::NUMBER);
    EXPECT_EQ(tokens[0].GetLexeme(), source);
    EXPECT_EQ(tokens[0].GetLiteralDouble(), 123.16);
}

TEST(ScannerTests, Scan_Identifier_HappyPath) {
    static constexpr auto source = R"(dest123 == 5)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].GetType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].GetLexeme(), "dest123"sv);
}

TEST(ScannerTests, Scan_Identifier_HappyPath_WithLeadingUnderscore) {

    static constexpr auto source = R"(_dest123 == 5)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].GetType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].GetLexeme(), "_dest123"sv);
}

TEST(ScannerTests, Scan_Identifier_HappyPath_WithMultiLeadingUnderscore) {
    static constexpr auto source = R"(__dest123 == 5)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].GetType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].GetLexeme(), "__dest123"sv);
}

TEST(ScannerTests, Scan_Identifier_LeadingNumberDoesntGetIncluded) {
    static constexpr auto source = R"(5_dest123 == 5)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[1].GetType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].GetLexeme(), "_dest123"sv);
}

TEST(ScannerTests, LogicOperators_AndOr) {
    static constexpr auto source = R"(true && false || true)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 6);

    EXPECT_EQ(tokens[1].GetType(), TokenType::LOGIC_AND);
    EXPECT_EQ(tokens[3].GetType(), TokenType::LOGIC_OR);
}

TEST(ScannerTests, BitWiseOperators_AndOr) {
    static constexpr auto source = R"(3 & 5 | 1000 ^ 20)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    const auto tokens = scanner.ScanTokens();

    ASSERT_FALSE(errors->HasError());
    ASSERT_EQ(tokens.size(), 8);

    EXPECT_EQ(tokens[1].GetType(), TokenType::BITWISE_AND);
    EXPECT_EQ(tokens[3].GetType(), TokenType::BITWISE_OR);
    EXPECT_EQ(tokens[5].GetType(), TokenType::BITWISE_XOR);
}