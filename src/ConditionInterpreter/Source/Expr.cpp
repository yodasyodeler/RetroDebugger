#include "Expr.h"

namespace Expr {

// Binary
Binary::Binary(IExprPtr left, TokenPtr oper, IExprPtr right) :
    m_left(std::move(left)), m_oper(std::move(oper)), m_right(std::move(right)) {}

VisitorValue Binary::Accept(const Rdb::IAstVisitor* visitor) const {
    return visitor->VisitBinary(this);
}


// Grouping
Grouping::Grouping(IExprPtr expression) :
    m_expression(std::move(expression)) {}

VisitorValue Grouping::Accept(const Rdb::IAstVisitor* visitor) const {
    return visitor->VisitGrouping(this);
}


// Literal
Literal::Literal(LiteralObject value) :
    m_value(std::move(value)) {}

VisitorValue Literal::Accept(const Rdb::IAstVisitor* visitor) const {
    return visitor->VisitLiteral(this);
}


// Logical
Logical::Logical(IExprPtr left, TokenPtr oper, IExprPtr right) :
    m_left(std::move(left)), m_oper(std::move(oper)), m_right(std::move(right)) {}

VisitorValue Logical::Accept(const Rdb::IAstVisitor* visitor) const {
    return visitor->VisitLogical(this);
}


// Unary
Unary::Unary(TokenPtr oper, IExprPtr right) :
    m_oper(std::move(oper)), m_right(std::move(right)) {}

VisitorValue Unary::Accept(const Rdb::IAstVisitor* visitor) const {
    return visitor->VisitUnary(this);
}


// Variable
Variable::Variable(TokenPtr name) :
    m_name(std::move(name)) {
}

VisitorValue Variable::Accept(const Rdb::IAstVisitor* visitor) const {
    return visitor->VisitVariable(this);
}

}