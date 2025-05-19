#pragma once

#include <string>
#include <variant>

#include "NumericType.h"

enum class NilType : uint8_t {};

using NumericValue = Rdb::NumericValueBase<double, int>;
using LiteralObject = std::variant<std::monostate, bool, NumericValue, std::string, NilType>;
using VisitorValue = LiteralObject;

constexpr bool IsUninitialized(const LiteralObject& literal) noexcept { return std::holds_alternative<std::monostate>(literal); }
constexpr bool IsNil(const LiteralObject& literal) noexcept { return std::holds_alternative<NilType>(literal); }
constexpr bool IsBool(const LiteralObject& literal) noexcept { return std::holds_alternative<bool>(literal); }
constexpr bool IsNumeric(const LiteralObject& literal) noexcept { return std::holds_alternative<NumericValue>(literal); }
constexpr bool IsDouble(const LiteralObject& literal) noexcept { return IsNumeric(literal) && std::get<NumericValue>(literal).IsDouble(); }
constexpr bool IsInt(const LiteralObject& literal) noexcept { return IsNumeric(literal) && std::get<NumericValue>(literal).IsInt(); }
constexpr bool IsString(const LiteralObject& literal) noexcept { return std::holds_alternative<std::string>(literal); }

// TODO: Currently defined in Token.cpp
std::string to_string(const LiteralObject& literal);