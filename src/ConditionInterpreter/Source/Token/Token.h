#pragma once

#include "Token/TokenType.h"

#include "Types.h"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

class Token {
public:
    Token(TokenType type, std::string_view lexeme, std::string literal, int offset);
    explicit Token(TokenType type, std::string_view lexeme, double literal, int offset);
    explicit Token(TokenType type, std::string_view lexeme, bool literal, int offset);
    explicit Token(TokenType type, std::string_view lexeme, NilType literal, int offset);
    Token(TokenType type, std::string_view lexeme, int offset);

    std::string ToString();

    TokenType GetType() const;
    int GetOffset() const;
    std::string GetLexeme() const;
    LiteralObject GetLiteral() const;

    // Attempt to convert literal values, throws on failures
    bool IsLiteralNil() const noexcept; // Won't throw
    bool GetLiteralBool() const;
    double GetLiteralDouble() const;
    std::string GetLiteralString() const;

private:
    TokenType m_type;
    int m_offset; // Column and line can be calculated from this value
    std::string m_lexeme; // TODO: should this be a string_view?
    LiteralObject m_literal;
};

using TokenList = std::vector<Token>;
using TokenPtr = std::shared_ptr<Token>;