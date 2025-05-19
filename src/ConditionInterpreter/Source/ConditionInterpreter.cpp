#include "ConditionInterpreter.h"

#include "Interpreter.h"
#include "Parser.h"
#include "Report.h"
#include "Scanner.h"


namespace Rdb {

// Static Public
ConditionPtr ConditionInterpreter::CreateCondition(std::shared_ptr<IDebuggerCallbacks> callbacks, const std::string& conditionString) {
    if (conditionString.empty()) { return nullptr; }

    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, conditionString);
    const auto tokens = scanner.ScanTokens();
    if (errors->HasError()) { throw std::runtime_error(errors->GetError()); }

    Parser parser(errors, tokens);
    auto expr = parser.ParseWithThrow();
    return std::unique_ptr<ConditionInterpreter>(new ConditionInterpreter(callbacks, expr, conditionString));
}

// Public
bool ConditionInterpreter::EvaluateCondition() const {
    auto errors = std::make_shared<Errors>();
    Interpreter interpreter(m_callbacks, errors);
    bool expressionResult = interpreter.InterpretBoolean(m_conditionExpression);
    if (errors->HasError()) { throw std::runtime_error(errors->GetError()); }

    return expressionResult;
}

std::string ConditionInterpreter::GetAsString() const {
    return m_conditionString;
}

// Private
ConditionInterpreter::ConditionInterpreter(std::shared_ptr<IDebuggerCallbacks> callbacks, Expr::IExprPtr expression, const std::string& conditionString) :
    m_callbacks(std::move(callbacks)),
    m_conditionExpression(std::move(expression)),
    m_conditionString(conditionString) {}

}