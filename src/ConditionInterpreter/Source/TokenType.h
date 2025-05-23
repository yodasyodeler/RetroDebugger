#pragma once

#include <string_view>

// Commented out Tokens that are not supported for Conditions
enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    COLON,
    DOT,
    MINUS,
    PLUS,
    QUESTION,
    SEMICOLON,
    SLASH,
    STAR,
    BITWISE_AND, // &
    BITWISE_OR, // |
    BITWISE_XOR, // ^

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    // EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    LOGIC_AND, // &&
    LOGIC_OR, // ||

    // Literals.
    IDENTIFIER,
    // STRING,
    NUMBER,
    BANK_NUMBER,

    // Keywords.
    // AND,
    // CLASS,
    // ELSE,
    FALSE,
    // FUN,
    // FOR,
    // IF,
    // NIL,
    // OR,
    // PRINT,
    // RETURN,
    // SUPER,
    // THIS,
    TRUE,
    // VAR,
    // WHILE,

    END_OF_FILE
};

std::string_view to_string(const TokenType tokenType);