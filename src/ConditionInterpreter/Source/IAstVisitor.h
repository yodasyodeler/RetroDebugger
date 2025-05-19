#pragma once

#include "Types.h"

// Forward decelerations to avoid cyclical dependencies
namespace Expr {
struct Binary;
struct Ternary;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;
}

namespace Rdb {

class IAstVisitor {
public:
    virtual ~IAstVisitor() = default;

    virtual VisitorValue VisitBinary(const Expr::Binary* expr) const = 0;
    virtual VisitorValue VisitGrouping(const Expr::Grouping* expr) const = 0;
    virtual VisitorValue VisitLiteral(const Expr::Literal* expr) const = 0;
    virtual VisitorValue VisitLogical(const Expr::Logical* expr) const = 0;
    virtual VisitorValue VisitUnary(const Expr::Unary* expr) const = 0;
    virtual VisitorValue VisitVariable(const Expr::Variable* expr) const = 0;
};

}