#pragma once

#include <memory>
#include <string>

#include <IDebuggerCallbacks.h>
#include <IExpr.h>


namespace Rdb {

class ConditionInterpreter {
public:
    static std::unique_ptr<ConditionInterpreter> CreateCondition(std::shared_ptr<IDebuggerCallbacks> callbacks, const std::string& conditionString);

    bool EvaluateCondition() const;

private:
    ConditionInterpreter(std::shared_ptr<IDebuggerCallbacks> callbacks, Expr::IExprPtr expression);

    std::shared_ptr<IDebuggerCallbacks> m_callbacks;
    Expr::IExprPtr m_conditionExpression;
};
using ConditionPtr = std::unique_ptr<ConditionInterpreter>;

}