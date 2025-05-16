#pragma once

#include "IDebuggerCallbacks.h"
#include "IExpr.h"
#include "Report.h"

#include <functional>

namespace Rdb {

class Interpreter : public IAstVisitor {
public:
    Interpreter(std::shared_ptr<IDebuggerCallbacks> callbacks, ErrorsPtr errors);

    std::string InterpretAsString(const Expr::IExprPtr& expr);
    bool InterpretBoolean(const Expr::IExprPtr& expr);

    VisitorValue VisitBinary(const Expr::Binary* expr) const override;
    VisitorValue VisitGrouping(const Expr::Grouping* expr) const override;
    VisitorValue VisitLiteral(const Expr::Literal* expr) const override;
    VisitorValue VisitUnary(const Expr::Unary* expr) const override;
    VisitorValue VisitVariable(const Expr::Variable* expr) const override;

    // Test helpers
    using PrinterMethod = std::function<void(std::string_view)>;
    void SetPrinter(PrinterMethod printMethod);

private:
    bool IsTruthy(const VisitorValue& literal) const;
    bool IsEqual(const VisitorValue& left, const VisitorValue& right) const;
    VisitorValue EvaluateExpression(const Expr::IExpr* expr) const;

    // Error checking
    void CheckNumberOperand(const TokenPtr& oper, const VisitorValue& right) const;
    void CheckNumberOperand(const TokenPtr& oper, const VisitorValue& left, const VisitorValue& right) const;

    PrinterMethod m_printer;
    ErrorsPtr m_errors;
};

}