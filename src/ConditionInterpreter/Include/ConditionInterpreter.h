#pragma once

#include <memory>
#include <string>

#include <RetroDebuggerCallbackDefines.h>
#include <Scanner/IExpr.h>


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