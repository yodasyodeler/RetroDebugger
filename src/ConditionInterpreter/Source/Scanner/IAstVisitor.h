#pragma once

#include "Types.h"

// Forward decelerations to avoid cyclical dependencies
namespace Expr {
struct Binary;
struct Assignment;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
}

class IAstVisitor {
public:
    virtual ~IAstVisitor() = default;

    virtual VisitorValue VisitBinary(const Expr::Binary* expr) const = 0;
    virtual VisitorValue VisitGrouping(const Expr::Grouping* expr) const = 0;
    virtual VisitorValue VisitLiteral(const Expr::Literal* expr) const = 0;
    virtual VisitorValue VisitUnary(const Expr::Unary* expr) const = 0;
    virtual VisitorValue VisitVariable(const Expr::Variable* expr) const = 0;
};
