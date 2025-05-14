#pragma once

#include "Token/Token.h"

#include "Report.h"
#include "Scanner/Expr.h"

#include <stdexcept>

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& errorMessage) :
        std::runtime_error(errorMessage) {}
};

class Parser {
public:
    Parser(ErrorsPtr errors, TokenList tokens);

    Expr::IExprPtr Parse();

    bool IsDone();

private:
    Expr::IExprPtr ParseBooleanExpression();
    Expr::IExprPtr ParseAssignment();
    Expr::IExprPtr ParseComma();
    Expr::IExprPtr ParseEquality();
    Expr::IExprPtr ParseComparison();
    Expr::IExprPtr ParseTerm();
    Expr::IExprPtr ParseFactor();
    Expr::IExprPtr ParseUnary();
    Expr::IExprPtr ParsePrimary();

    // Token walkers
    bool MatchTokenType(TokenType tokenType);
    bool MatchTokenType(const std::vector<TokenType>& tokenTypes);
    bool CheckTokenType(TokenType type);
    bool IsAtEnd();

    Token& AdvanceToken();
    Token& PeekToken();
    Token& PreviousToken();
    Token& ConsumeToken(TokenType tokenType, std::string_view errorMessage);

    // Error handling
    ParseError TokenError(const Token& token, std::string_view errorMessage);
    void SynchronizeAfterError();

    size_t m_tokenIndex = 0;
    TokenList m_tokens;

    ErrorsPtr m_errors;
};