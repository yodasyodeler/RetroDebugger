#include "Interpreter.h"

#include "Scanner/Expr.h"

#include <algorithm>
#include <array>

namespace {
constexpr bool IsZero(const LiteralObject& literal) noexcept {
    return (std::holds_alternative<double>(literal) && std::get<double>(literal) == 0.0) || (std::holds_alternative<int>(literal) && std::get<int>(literal) == 0);
}
}


Interpreter::Interpreter(ErrorsPtr errors) :
    m_errors(std::move(errors)),
    m_printer([](std::string_view message) { fmt::print("{}", message); }) {}

std::string Interpreter::InterpretAsString(const Expr::IExprPtr& expr) {
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

        case TokenType::MINUS:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<double>(left) - std::get<double>(right);
        case TokenType::SLASH:
            CheckNumberOperand(expr->m_oper, left, right);
            if (auto rightNum = std::get<double>(right)) {
                return std::get<double>(left) / std::get<double>(right);
            }
            // Fall threw means divide by zero.
            throw RuntimeError(expr->m_oper, "Divide by zero error.");

        case TokenType::STAR:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<double>(left) * std::get<double>(right);

        case TokenType::PLUS: {
            const auto IsUnsupportedType = [](const VisitorValue& value) {
                return !(IsDouble(value) || IsString(value));
            };
            if (std::ranges::any_of(std::array<VisitorValue, 2>{ left, right }, IsUnsupportedType)) {
                throw RuntimeError(expr->m_oper, "Operands must be either numbers or strings.");
            }

            // If one or both are strings, convert both to strings.
            if (IsString(left) || IsString(right)) {
                return to_string(left) + to_string(right);
            }

            return std::get<double>(left) + std::get<double>(right);
        }
        case TokenType::GREATER:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<double>(left) > std::get<double>(right);
        case TokenType::GREATER_EQUAL:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<double>(left) >= std::get<double>(right);
        case TokenType::LESS:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<double>(left) < std::get<double>(right);
        case TokenType::LESS_EQUAL:
            CheckNumberOperand(expr->m_oper, left, right);
            return std::get<double>(left) <= std::get<double>(right);

        case TokenType::BANG_EQUAL:
            return !IsEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return IsEqual(left, right);
    };

    return {};
}

VisitorValue Interpreter::VisitGrouping(const Expr::Grouping* expr) const {
    return EvaluateExpression(expr->m_expression.get());
}

VisitorValue Interpreter::VisitLiteral(const Expr::Literal* expr) const {
    return expr->m_value;
}

VisitorValue Interpreter::VisitUnary(const Expr::Unary* expr) const {
    const auto right = EvaluateExpression(expr->m_right.get());

    switch (expr->m_oper->GetType()) {
        case TokenType::BANG:
            return VisitorValue{ !IsTruthy(right) };
        case TokenType::MINUS:
            if (IsInt(right)) {
                return VisitorValue{ -std::get<int>(right) };
            }
            CheckNumberOperand(expr->m_oper, right);
            return VisitorValue{ -std::get<double>(right) };
        case TokenType::STAR:
            if (IsInt(right)) {
                return VisitorValue{ -std::get<int>(right) };
            }
            CheckNumberOperand(expr->m_oper, right);
            // TODO: ReadMemory callback to get the value
            return VisitorValue{ static_cast<int>(std::get<double>(right)) };
    };

    // TODO: Should be unreachable
    return {};
}

VisitorValue Interpreter::VisitVariable(const Expr::Variable* expr) const {

    // TODO: no longer a variable, but instead a system specific look up value

    return VisitorValue{ 0 };
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

bool Interpreter::IsEqual(const VisitorValue& left, const VisitorValue& right) const {
    return left == right;
}


VisitorValue Interpreter::EvaluateExpression(const Expr::IExpr* expr) const {
    return expr->Accept(this);
}

void Interpreter::CheckNumberOperand(const TokenPtr& oper, const VisitorValue& right) const {
    if (IsDouble(right)) { return; }
    if (IsDouble(right)) { return; }

    throw RuntimeError(oper, "Operand must be a number.");
}

void Interpreter::CheckNumberOperand(const TokenPtr& oper, const VisitorValue& left, const VisitorValue& right) const {
    if (IsDouble(left) && IsDouble(right)) { return; }

    throw RuntimeError(oper, "Operands must be a number.");
}