#pragma once


#include <string>
#include <variant>


enum class NilType : uint8_t {};

// LOX represents all numbers as a floating point double. Including integers.
using LiteralObject = std::variant<std::monostate, bool, double, int, std::string, NilType>;
using VisitorValue = LiteralObject;

constexpr bool IsUninitialized(const LiteralObject& literal) noexcept { return std::holds_alternative<std::monostate>(literal); }
constexpr bool IsNil(const LiteralObject& literal) noexcept { return std::holds_alternative<NilType>(literal); }
constexpr bool IsBool(const LiteralObject& literal) noexcept { return std::holds_alternative<bool>(literal); }
constexpr bool IsDouble(const LiteralObject& literal) noexcept { return std::holds_alternative<double>(literal); }
constexpr bool IsInt(const LiteralObject& literal) noexcept { return std::holds_alternative<int>(literal); }
constexpr bool IsString(const LiteralObject& literal) noexcept { return std::holds_alternative<std::string>(literal); }

// TODO: Currently defined in Token.cpp
std::string to_string(const LiteralObject& literal);