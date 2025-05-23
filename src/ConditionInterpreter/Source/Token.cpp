#include "Token.h"

#include "fmt/core.h"

#include <cmath>

using namespace std::literals::string_literals;


template<class AnyType>
struct always_false : std::false_type {};

template<class AnyType>
constexpr bool always_false_v = always_false<AnyType>::value;

// ToString helper
std::string to_string(const LiteralObject& literal) {
    return std::visit([](auto&& arg) -> std::string {
        using LiteralType = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<LiteralType, bool>) {
            return arg ? "true" : "false";
        }
        else if constexpr (std::is_same_v<LiteralType, NumericValue>) {
            if (arg.IsDouble()) {
                double interPtr = {};
                double value = arg.Get<double>();
                if (bool isInteger = !std::modf(value, &interPtr)) {
                    return std::to_string(static_cast<int>(value));
                }
                else {
                    return std::to_string(value);
                }
            }
            else {
                return std::to_string(arg.Get<int>());
            }
        }
        else if constexpr (std::is_same_v<LiteralType, std::pair<NumericValue, NumericValue>>) {
            return fmt::format("{}:{}", arg.first.Get<int>(), arg.second.Get<int>());
        }
        else if constexpr (std::is_same_v<LiteralType, std::string>) {
            return arg;
        }
        else if constexpr (std::is_same_v<LiteralType, NilType>) {
            return "nil";
        }
        else if constexpr (std::is_same_v<LiteralType, std::monostate>) {
            return ""; // TODO: how should I handle this?
        }
        else {
            static_assert(always_false_v<LiteralType>, "non-exhaustive visitor!");
        }
    },
        literal);
}

// Methods
Token::Token(TokenType type, std::string_view lexeme, std::string literal, int offset) :
    m_type(type), m_lexeme(lexeme), m_literal(literal), m_offset(offset) {
}

Token::Token(TokenType type, std::string_view lexeme, int literal, int offset) :
    m_type(type), m_lexeme(lexeme), m_literal(NumericValue{ literal }), m_offset(offset) {
}

Token::Token(TokenType type, std::string_view lexeme, std::pair<int, int> literal, int offset) :
    m_type(type), m_lexeme(lexeme), m_literal(std::pair{ NumericValue{ literal.first }, NumericValue{ literal.second } }), m_offset(offset) {
}

Token::Token(TokenType type, std::string_view lexeme, double literal, int offset) :
    m_type(type), m_lexeme(lexeme), m_literal(NumericValue{ literal }), m_offset(offset) {
}

Token::Token(TokenType type, std::string_view lexeme, bool literal, int offset) :
    m_type(type), m_lexeme(lexeme), m_literal(literal), m_offset(offset) {
}

Token::Token(TokenType type, std::string_view lexeme, NilType literal, int offset) :
    m_type(type), m_lexeme(lexeme), m_literal(literal), m_offset(offset) {
}

Token::Token(TokenType type, std::string_view lexeme, int offset) :
    m_type(type), m_lexeme(lexeme), m_offset(offset) {
}

std::string Token::ToString() {
    return fmt::format("{} {} {}", to_string(m_type), m_lexeme, to_string(m_literal));
}

TokenType Token::GetType() const { return m_type; }

int Token::GetOffset() const { return m_offset; }

std::string Token::GetLexeme() const { return m_lexeme; }

LiteralObject Token::GetLiteral() const { return m_literal; }

bool Token::IsLiteralNil() const noexcept {
    return IsNil(m_literal);
}

bool Token::GetLiteralBool() const {
    if (!IsBool(m_literal)) { throw std::runtime_error("Must be a 'bool' type."); }

    return std::get<bool>(m_literal);
}

double Token::GetLiteralDouble() const {
    if (!IsNumeric(m_literal)) { throw std::runtime_error("Must be a 'Numeric' type."); }

    return std::get<NumericValue>(m_literal).Get<double>();
}


int Token::GetLiteralInt() const {
    if (!IsNumeric(m_literal)) { throw std::runtime_error("Must be a 'Numeric' type."); }

    return std::get<NumericValue>(m_literal).Get<int>();
}

std::pair<int, int> Token::GetLiteralBankNumber() const {
    if (!IsNumericPair(m_literal)) { throw std::runtime_error("Must be a 'NumericPair' type."); }

    const auto numPair = std::get<std::pair<NumericValue, NumericValue>>(m_literal);
    return { numPair.first.Get<int>(), numPair.second.Get<int>() };
}

std::string Token::GetLiteralString() const {
    if (!IsString(m_literal)) { throw std::runtime_error("Must be a 'std::string' type."); }

    return std::get<std::string>(m_literal);
}
