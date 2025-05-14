#include "gtest/gtest.h"

#include "Expr.h"
#include "StringVisitor.h"
#include "Token.h"

/* TODO:

*/

using namespace std::string_view_literals;
using namespace std::string_literals;

TEST(ExprTests, Expr_StringVisitor_VisitExpression) {
    const auto expectedStr = "(* (- 123) (group 45.67))"s;
    auto token_minus = std::make_shared<Token>(TokenType::MINUS, "-"sv, -1);
    auto literal_123 = std::make_shared<Expr::Literal>("123"s);
    auto unary_minus_123 = std::make_shared<Expr::Unary>(token_minus, literal_123);

    auto literal_45_67 = std::make_shared<Expr::Literal>("45.67"s);
    auto group_number = std::make_shared<Expr::Grouping>(literal_45_67);

    auto token_star = std::make_shared<Token>(TokenType::STAR, "*"sv, -1);
    auto star_binary_group = std::make_shared<Expr::Binary>(unary_minus_123, token_star, group_number);

    StringVisitor visitor;
    const auto visitReturn = star_binary_group->Accept(&visitor);

    ASSERT_EQ(expectedStr, std::get<std::string>(visitReturn));
}