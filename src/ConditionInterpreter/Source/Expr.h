#pragma once

#include "Token.h"

#include "IExpr.h"


namespace Expr {

struct Binary : public IExpr
{
    Binary(IExprPtr left, TokenPtr oper, IExprPtr right);
    VisitorValue Accept(const Rdb::IAstVisitor* visitor) const override;

    IExprPtr m_left;
    TokenPtr m_oper;
    IExprPtr m_right;
};

struct Grouping : public IExpr
{
    Grouping(IExprPtr expression);
    VisitorValue Accept(const Rdb::IAstVisitor* visitor) const override;

    IExprPtr m_expression;
};

struct Literal : public IExpr
{
    Literal(LiteralObject value);
    VisitorValue Accept(const Rdb::IAstVisitor* visitor) const override;

    LiteralObject m_value;
};

struct Logical : public IExpr
{
    Logical(IExprPtr left, TokenPtr oper, IExprPtr right);
    VisitorValue Accept(const Rdb::IAstVisitor* visitor) const override;

    IExprPtr m_left;
    TokenPtr m_oper;
    IExprPtr m_right;
};

struct Unary : public IExpr
{
    Unary(TokenPtr oper, IExprPtr right);
    VisitorValue Accept(const Rdb::IAstVisitor* visitor) const override;

    TokenPtr m_oper;
    IExprPtr m_right;
};

struct Variable : public IExpr
{
    Variable(TokenPtr name);
    VisitorValue Accept(const Rdb::IAstVisitor* visitor) const override;

    TokenPtr m_name;
};

}