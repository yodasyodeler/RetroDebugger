#pragma once

#include "IAstVisitor.h"

#include <memory>

namespace Expr {

struct IExpr {
    virtual ~IExpr() = default;

    virtual VisitorValue Accept(const IAstVisitor* visitor) const = 0;
};
using IExprPtr = std::shared_ptr<IExpr>; // TODO: Can this be a unique_ptr?

}