#include "Interpreter.h"

#include "Expr.h"

#include <algorithm>
#include <array>

namespace {
constexpr bool IsZero(const LiteralObject& literal) noexcept {
    auto ZeroValue = NumericValue{ 0.0 };
    return IsNumeric(literal) && std::get<NumericValue>(literal) == ZeroValue;
}

constexpr int GetValueAsInt(const LiteralObject& literal) {
    return std::get<NumericValue>(literal).Get<int>();
}

constexpr int GetValueAsDouble(const LiteralObject& literal) {
    return std::get<NumericValue>(literal).Get<double>();
}

constexpr bool AreNumbers(const VisitorValue& left, const VisitorValue& right) {
    return IsNumeric(left) && IsNumeric(right);
}
}


namespace Rdb {

Interpreter::Interpreter(std::shared_ptr<IDebuggerCallbacks> callbacks, ErrorsPtr errors) :
    m_callbacks(std::move(callbacks)),
    m_errors(std::move(errors)) {}

std::string Interpreter::InterpretAsString(const Expr::IExprPtr& expr) {
    if (expr == nullptr) { return {}; } // TODO: should this be a throw.
    try {
        const auto value = EvaluateExpression(expr.get());

        return to_string(value);
    }
    catch (const RuntimeError& error) {
        m_errors->ReportRuntimeError(error);
        return {}; // return empty string, error gets printed with above statement
    }
}

bool Interpreter::InterpretBoolean(const Expr::IExprPtr& expr) {
    if (expr == nullptr) { return true; } // TODO: should this be true, false, or throw.
    try {
        const auto value = EvaluateExpression(expr.get());

        return IsTruthy(value);
    }
    catch (const RuntimeError& error) {
        m_errors->ReportRuntimeError(error);
        throw;
    }
}

VisitorValue Interpreter::VisitBinary(const Expr::Binary* expr) const {
    const auto left = EvaluateExpression(expr->m_left.get());
    const auto waitToEvaluateRight = expr->m_oper->GetType() == TokenType::QUESTION;
    const auto right = waitToEvaluateRight ? VisitorValue{} : EvaluateExpression(expr->m_right.get());

    switch (expr->m_oper->GetType()) {
        // TODO: Reevaluate with branching support
        case TokenType::QUESTION: {
            const auto colonOperator = std::dynamic_pointer_cast<Expr::Binary>(expr->m_right);
            return IsTruthy(left) ? EvaluateExpression(colonOperator->m_left.get()) : EvaluateExpression(colonOperator->m_right.get());
        }

        case TokenType::COMMA:
            return right;

        case TokenType::BITWISE_OR:
            CheckNumberOperand(expr->m_oper, left, right);
            return GetValueAsInt(left) | GetValueAsInt(right);
        case TokenType::BITWISE_XOR:
            CheckNumberOperand(expr->m_oper, left, right);
            return GetValueAsInt(left) ^ GetValueAsInt(right);
        case TokenType::BITWISE_AND:
            CheckNumberOperand(expr->m_oper, left, right);
            return GetValueAsInt(left) & GetValueAsInt(right);

        case TokenType::MINUS:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<NumericValue>(left) - std::get<NumericValue>(right);
        case TokenType::SLASH:
            CheckNumberOperand(expr->m_oper, left, right);
            if (!IsZero(right)) {
                return std::get<NumericValue>(left) / std::get<NumericValue>(right);
            }
            // Fall threw means divide by zero.
            throw RuntimeError(expr->m_oper, "Divide by zero error.");

        case TokenType::STAR:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<NumericValue>(left) * std::get<NumericValue>(right);

        case TokenType::PLUS: {
            const auto IsUnsupportedType = [](const VisitorValue& value) {
                return !(IsNumeric(value) || IsString(value));
            };
            if (std::ranges::any_of(std::array<VisitorValue, 2>{ left, right }, IsUnsupportedType)) {
                throw RuntimeError(expr->m_oper, "Operands must be either numbers or strings.");
            }

            // If one or both are strings, convert both to strings.
            if (IsString(left) || IsString(right)) {
                return to_string(left) + to_string(right);
            }

            return std::get<NumericValue>(left) + std::get<NumericValue>(right);
        }
        case TokenType::GREATER:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<NumericValue>(left) > std::get<NumericValue>(right);
        case TokenType::GREATER_EQUAL:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<NumericValue>(left) >= std::get<NumericValue>(right);
        case TokenType::LESS:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<NumericValue>(left) < std::get<NumericValue>(right);
        case TokenType::LESS_EQUAL:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<NumericValue>(left) <= std::get<NumericValue>(right);

        case TokenType::BANG_EQUAL:
            return left != right;

        case TokenType::EQUAL_EQUAL:
            return left == right;
    };

    return {};
}

VisitorValue Interpreter::VisitGrouping(const Expr::Grouping* expr) const {
    return EvaluateExpression(expr->m_expression.get());
}

VisitorValue Interpreter::VisitLogical(const Expr::Logical* expr) const {
    auto left = EvaluateExpression(expr->m_left.get());

    // Logic short-circuit checks
    if (expr->m_oper->GetType() == TokenType::LOGIC_OR) {
        if (IsTruthy(left)) { return left; }
    }
    else { // LOGIC_AND
        if (!IsTruthy(left)) { return left; }
    }

    return EvaluateExpression(expr->m_right.get());
}

VisitorValue Interpreter::VisitLiteral(const Expr::Literal* expr) const {
    return expr->m_value;
}

VisitorValue Interpreter::VisitUnary(const Expr::Unary* expr) const {
    const auto right = EvaluateExpression(expr->m_right.get());

    switch (expr->m_oper->GetType()) {
        case TokenType::BANG:
            return VisitorValue{ !IsTruthy(right) };
        case TokenType::MINUS: {
            CheckNumberOperand(expr->m_oper, right);
            return VisitorValue{ NumericValue(0) - std::get<NumericValue>(right) };
        }
        case TokenType::STAR:
            if (IsNumericPair(right)) {
                const auto numberPair = std::get<std::pair<NumericValue, NumericValue>>(right);
                return VisitorValue{ static_cast<int>(m_callbacks->ReadBankableMemory(BankNum{ static_cast<unsigned int>(numberPair.first.Get<int>()) }, numberPair.second.Get<int>())) };
            }
            CheckNumberOperand(expr->m_oper, right);
            return VisitorValue{ static_cast<int>(m_callbacks->ReadMemory(std::get<NumericValue>(right).Get<int>())) };
    };

    // TODO: Should be unreachable
    return {};
}

VisitorValue Interpreter::VisitVariable(const Expr::Variable* expr) const {
    auto regset = m_callbacks->GetRegSet();

    if (const auto& iter = regset.find(expr->m_name->GetLexeme());
        iter == regset.end()) {
        throw RuntimeError(expr->m_name, "Not a recognized identifier.");
    }
    else {
        return VisitorValue{ static_cast<int>(iter->second) };
    }
}

void Interpreter::SetPrinter(PrinterMethod printMethod) {
    m_printer = printMethod;
}

bool Interpreter::IsTruthy(const VisitorValue& literal) const {
    if (IsZero(literal)) { return false; }
    if (IsNil(literal)) { return false; }
    if (IsBool(literal)) { return std::get<bool>(literal); }

    return true;
}


VisitorValue Interpreter::EvaluateExpression(const Expr::IExpr* expr) const {
    return expr->Accept(this);
}

void Interpreter::CheckNumberOperand(const TokenPtr& oper, const VisitorValue& right) const {
    if (IsNumeric(right)) { return; }

    throw RuntimeError(oper, "Operand must be a number.");
}

void Interpreter::CheckNumberOperand(const TokenPtr& oper, const VisitorValue& left, const VisitorValue& right) const {
    if (AreNumbers(left, right)) { return; }

    throw RuntimeError(oper, "Operands must be a number.");
}

}