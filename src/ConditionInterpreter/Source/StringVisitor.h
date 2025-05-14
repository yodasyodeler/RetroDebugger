#pragma once

#include "Expr.h"
#include "IAstVisitor.h"

#include "Token.h" // TODO: may want to move this into a cpp file.

using namespace std::literals::string_literals;

struct StringVisitor : public IAstVisitor
{
    VisitorValue VisitBinary(const Expr::Binary* expr) const override { return "("s + expr->m_oper->GetLexeme() + " "s + std::get<std::string>(expr->m_left->Accept(this)) + " "s + std::get<std::string>(expr->m_right->Accept(this)) + ")"s; }
    VisitorValue VisitGrouping(const Expr::Grouping* expr) const override { return "(group "s + std::get<std::string>(expr->m_expression->Accept(this)) + ")"s; }
    VisitorValue VisitLiteral(const Expr::Literal* expr) const override { return to_string(expr->m_value); }
    VisitorValue VisitUnary(const Expr::Unary* expr) const override { return "("s + expr->m_oper->GetLexeme() + " "s + std::get<std::string>(expr->m_right->Accept(this)) + ")"s; }
    VisitorValue VisitVariable(const Expr::Variable* expr) const override { return expr->m_name->GetLexeme(); }
};
