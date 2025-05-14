#include "ConditionInterpreter.h"

#include "Interpreter/Interpreter.h"
#include "Parser/Parser.h"
#include "Report.h"
#include "Scanner/Scanner.h"

namespace Rdb {

ConditionPtr ConditionInterpreter::CreateCondition(const std::string& conditionString) {
    auto errors = std::make_shared<Errors>();
    Scanner scanner(errors, conditionString);
    const auto tokens = scanner.ScanTokens();
    if (errors->HasError()) { throw std::runtime_error(errors->GetError()); }

    Parser parser(errors, tokens);
    auto expr = parser.Parse();
    return std::unique_ptr<ConditionInterpreter>(new ConditionInterpreter(expr));
}

bool ConditionInterpreter::EvaluateCondition() {
    auto errors = std::make_shared<Errors>();
    Interpreter interpreter(errors);
    bool expressionResult = interpreter.InterpretBoolean(m_conditionExpression);
    if (errors->HasError()) { throw std::runtime_error(errors->GetError()); }

    return expressionResult;
}

ConditionInterpreter::ConditionInterpreter(Expr::IExprPtr expression) :
    m_conditionExpression(std::move(expression)) {}

}