#include "Parser.h"
#include "Report.h"
#include "Scanner.h"
#include "StringVisitor.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
/* TODO:

*/

using namespace std::string_view_literals;
using namespace std::string_literals;


TEST(ParserExpressionTests, Equality_AllTypes_AllLiteralTypes_HappyPath) {
    std::string expectedStr = "(== (== (== 1234 ABC) (group (!= true false))) nil)";
    std::string testStr = R"(1234 == "ABC" == (true != false) == nil)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    const auto expr = parser.Parse();
    EXPECT_TRUE(parser.IsDone());

    StringVisitor visitor;
    const auto visitReturn = expr->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}

TEST(ParserExpressionTests, Comparison_AllTypes_HappyPath) {
    std::string expectedStr = "(< (> (<= (>= 1234 ABC) true) false) nil)";
    std::string testStr = R"(1234 >= "ABC" <= true > false < nil)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    const auto expr = parser.Parse();
    EXPECT_TRUE(parser.IsDone());

    StringVisitor visitor;
    const auto visitReturn = expr->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}

TEST(ParserExpressionTests, Term_Factor_AllTypes_HappyPath) {
    std::string expectedStr = "(- (+ 1234 5678) (/ (* (- 1) 2.500000) 0))";
    std::string testStr = R"(1234 + 5678 - -1.0 * 2.5 / 0.0)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    const auto expr = parser.Parse();
    EXPECT_TRUE(parser.IsDone());

    StringVisitor visitor;
    const auto visitReturn = expr->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}

TEST(ParserExpressionTests, AllChapter6Grammers_HappyPath) {
    std::string expectedStr = "(== ABC (> 123 (group (+ 5 (/ 3.100000 (- 8))))))";
    std::string testStr = R"(ABC ==  123 > (5 + 3.1 / -8))";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    const auto expr = parser.Parse();
    EXPECT_TRUE(parser.IsDone());

    StringVisitor visitor;
    const auto visitReturn = expr->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}

TEST(ParserExpressionTests, Chapter6Challenge_CommaList_HappyPath) {
    std::string expectedStr = "(, (, 123 (+ 5 3.100000)) false)";
    std::string testStr = R"(123, 5 + 3.1, false)";

    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    const auto expr = parser.Parse();
    EXPECT_TRUE(parser.IsDone());

    StringVisitor visitor;
    const auto visitReturn = expr->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}

TEST(ParserExpressionTests, Chapter6Challenge_Assignment_TernaryOperator_HappyPath) {
    std::string expectedStr = "(? (== 123 false) (: (* 3.100000 5) 1))";
    std::string testStr = R"(123 == false ? 3.1 * 5.0 : 1.0)";

    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    const auto expr = parser.Parse();
    EXPECT_TRUE(parser.IsDone());

    StringVisitor visitor;
    const auto visitReturn = expr->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}

TEST(ParserExpressionTests, Chapter6Challenge_TernaryOperator_withCommaOperator_HappyPath) {
    std::string expectedStr = "(? true (: (, (, 5 6) 8) 7))";
    std::string testStr = R"(true ? 5,6,8 : 7)";

    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    const auto expr = parser.Parse();
    EXPECT_TRUE(parser.IsDone());

    StringVisitor visitor;
    const auto visitReturn = expr->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}

// Syntax Error Handling

TEST(ParserExpressionTests, Chapter6Challenge_Assignment_Ternary_NoLeftOperand_ExpectThrow) {
    std::string testStr = R"( ? 8 : 5)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Chapter6Challenge_Assignment_Ternary_NoMidOperand_ExpectThrow) {
    std::string testStr = R"(true ?  : 5)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Chapter6Challenge_Assignment_Ternary_NoRightOperand_ExpectThrow) {
    std::string testStr = R"(true ? 8 : )";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Chapter6Challenge_Comma_NoLeftHandOperand_ExpectThrow) {
    std::string testStr = R"(, 8)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Chapter6Challenge_Comma_NoRightHandOperand_ExpectThrow) {
    std::string testStr = R"(8, )";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"( at end": Expect expression)"));
}

TEST(ParserExpressionTests, Equality_NoLeftHandOperand_ExpectThrow) {
    std::string testStr = R"(== 8)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Equality_NoRightHandOperand_ExpectThrow) {
    std::string testStr = R"(8 ==)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"( at end": Expect expression)"));
}

TEST(ParserExpressionTests, Comparison_NoLeftHandOperand_ExpectThrow) {
    std::string testStr = R"(<= 8)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Comparison_NoRightHandOperand_ExpectThrow) {
    std::string testStr = R"(8 <=)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"( at end": Expect expression)"));
}

TEST(ParserExpressionTests, Term_NoLeftHandOperand_ExpectThrow) {
    std::string testStr = R"(+ 8)"; // Need to use '+', '-' has a Unary use
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Term_NoRightHandOperand_ExpectThrow) {
    std::string testStr = R"(8 +)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Expect expression.)"));
}

TEST(ParserExpressionTests, Factor_NoLeftHandOperand_ExpectThrow) {
    std::string testStr = R"(/ 8)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr("Expect expression."));
}

TEST(ParserExpressionTests, Factor_NoRightHandOperand_ExpectThrow) {
    std::string testStr = R"(8 /)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    EXPECT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"( at end": Expect expression)"));
}

TEST(ParserExpressionTests, Unary_BangOnWrongSide_ExpectThrow) {
    std::string testStr = R"(8!)";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    ASSERT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"( at '!'": Expected end of expression.)"));
}

TEST(ParserExpressionTests, Expression_NoClosingParen_ExpectThrow) {
    std::string testStr = R"( ( 8 )";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"( at end": Expect ')' after expression)"));
}

TEST(ParserExpressionTests, Expression_NoOpeningParen_ExpectThrow) {
    std::string testStr = R"( 8 ) )";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    Parser parser(errors, scanner.ScanTokens());

    ASSERT_EQ(parser.Parse(), nullptr);
    ASSERT_TRUE(parser.IsDone());
    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(at ')'": Expected end of expression)"));
}

TEST(ParserExpressionTests, Expression_SemiColon_CausesError) {
    std::string testStr = R"( 8; )";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(";": Unexpected character.)"));
}

TEST(ParserExpressionTests, Expression_assignment_CausesError) {
    std::string testStr = R"( abc = 8 )";
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, testStr);
    auto tokens = scanner.ScanTokens();

    ASSERT_TRUE(errors->HasError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"("=": Unexpected character.)"));
}