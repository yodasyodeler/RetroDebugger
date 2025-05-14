#include "Interpreter.h"

#include "Parser.h"
#include "Scanner.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <vector>

/* TODO:

*/

std::pair<Expr::IExprPtr, ErrorsPtr> ScanAndParse(std::string_view source) {

    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, source);
    Parser parser(errors, scanner.ScanTokens());
    auto expr = parser.Parse();

    return { expr, errors };
}

TEST(InterpreterExpressionTests, Equality_DoubleEqual_HappyPath) {
    std::string expectedOutcome = "true";
    std::string testStr = R"(1 == 1)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Equality_NotEqual_HappyPath) {
    std::string expectedOutcome = "false";
    std::string testStr = R"(1 != 1)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Comparison_Greater_HappyPath) {
    std::string expectedOutcome = "true";
    std::string testStr = R"((6 > 2) == (6 >= 6))";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Comparison_Less_HappyPath) {
    std::string expectedOutcome = "false";
    std::string testStr = R"((6 < 2) == (6 <= 6))";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Comma_HappyPath) {
    std::string expectedOutcome = "100";
    std::string testStr = R"(1, 2, 19, 100)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Equality_Ternary_HappyPath) {
    std::string expectedOutcome = "-5";
    std::string testStr = R"(false,true,false ? 100 : -5)"; // Comma occurs before Ternary
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Unary_Minus_HappyPath) {
    std::string expectedOutcome = "-5";
    std::string testStr = R"(-(2 + 3))";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Unary_Not_HappyPath) {
    std::string expectedOutcome = "false";
    std::string testStr = R"(!(2 + 3))";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Term_Divide_HappyPath) {
    std::string expectedOutcome = "10";
    std::string testStr = R"(300 / 30)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Term_Mult_HappyPath) {
    std::string expectedOutcome = "300";
    std::string testStr = R"(10 * 30)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Term_Minus_HappyPath) {
    std::string expectedOutcome = "-3";
    std::string testStr = R"(2.3 - 5.3)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, Term_Plus_Number_HappyPath) {
    std::string expectedOutcome = "13";
    std::string testStr = R"(-2 + 5 + 10)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, DISABLED_Term_Plus_String_HappyPath) {
    std::string expectedOutcome = "String_Concat";
    std::string testStr = R"("String"+ "_" + "Concat")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, DISABLED_Term_Plus_Mixed_HappyPath) {
    std::string expectedOutcome = "101_String2_101";
    std::string testStr = R"(101+ "_" + "String" + 2 + "_" + 101)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_EQ(interpreter.InterpretAsString(statements), expectedOutcome);
    ASSERT_FALSE(errors->HasError());
}

TEST(InterpreterExpressionTests, DISABLED_Term_Plus_UnsuportedType_RightOperand_ReportsError) {
    std::string testStr = R"("string" + true)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be either numbers or strings.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Term_Plus_UnsuportedType_LeftOperand_ReportsError) {
    std::string testStr = R"(false + "string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be either numbers or strings.)"));
}

TEST(InterpreterExpressionTests, Factor_Divide_DivideByZero_ReportsError) {
    std::string testStr = R"(5 / 0)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Divide by zero error.)"));
}

TEST(InterpreterExpressionTests, InterpretBoolean_ReportsError_CodeCoverage) {
    std::string testStr = R"(5 / 0)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    EXPECT_ANY_THROW(interpreter.InterpretBoolean(statements));
}

TEST(InterpreterExpressionTests, InterpretBoolean_HappyPath_CodeCoverage) {
    {
        std::string testStr = R"( 0 )";
        const auto [statements, errors] = ScanAndParse(testStr);
        ASSERT_FALSE(errors->HasError());

        Interpreter interpreter(errors);
        EXPECT_FALSE(interpreter.InterpretBoolean(statements));
    }
    {
        std::string testStr = R"( 5 == (10 / 2) )";
        const auto [statements, errors] = ScanAndParse(testStr);
        ASSERT_FALSE(errors->HasError());

        Interpreter interpreter(errors);
        EXPECT_TRUE(interpreter.InterpretBoolean(statements));
    }
}

TEST(InterpreterExpressionTests, DISABLED_Factor_Mult_NotANumber_ReportsError) {
    std::string testStr = R"("string" * 5)";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be a number.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Factor_Divide_NotANumber_ReportsError) {
    std::string testStr = R"(5 / "string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be a number.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Term_Minus_NotANumber_ReportsError) {
    std::string testStr = R"(5 - "string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be a number.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Comparison_Greater_NotANumber_ReportsError) {
    std::string testStr = R"(5 > "string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be a number.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Comparison_GreaterEqual_NotANumber_ReportsError) {
    std::string testStr = R"(5 >= "string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be a number.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Comparison_Less_NotANumber_ReportsError) {
    std::string testStr = R"(5 < "string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be a number.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Comparison_LessEqual_NotANumber_ReportsError) {
    std::string testStr = R"(5 <= "string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operands must be a number.)"));
}

TEST(InterpreterExpressionTests, DISABLED_Unary_Minus_NotANumber_ReportsError) {
    std::string testStr = R"(-"string")";
    const auto [statements, errors] = ScanAndParse(testStr);
    ASSERT_FALSE(errors->HasError());

    Interpreter interpreter(errors);
    ASSERT_TRUE(interpreter.InterpretAsString(statements).empty());

    ASSERT_TRUE(errors->HasError());
    ASSERT_TRUE(errors->HasRuntimeError());
    EXPECT_THAT(errors->GetError(), testing::HasSubstr(R"(Operand must be a number.)"));
}