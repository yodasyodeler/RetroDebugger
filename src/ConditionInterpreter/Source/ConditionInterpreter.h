#pragma once

#include <memory>
#include <string>

#include <IExpr.h>
#include <IDebuggerCallbacks.h>


namespace Rdb {

class ConditionInterpreter {
public:
    static std::unique_ptr<ConditionInterpreter> CreateCondition(const std::string& conditionString);

    bool EvaluateCondition();

private:
    ConditionInterpreter(Expr::IExprPtr expression);

    Expr::IExprPtr m_conditionExpression;
};
using ConditionPtr = std::unique_ptr<ConditionInterpreter>;

}