#include "Parser.h"

#include "Report.h"

Parser::Parser(ErrorsPtr errors, TokenList tokens) :
    m_tokens(std::move(tokens)),
    m_errors(std::move(errors)) {
}

Expr::IExprPtr Parser::Parse() noexcept {
    try {
        return ParseBooleanExpression();
    }
    catch (const ParseError& /*error*/) {
        SynchronizeAfterError();
        return nullptr;
    }
}

Expr::IExprPtr Parser::ParseWithThrow() {
    try {
        return ParseBooleanExpression();
    }
    catch (const ParseError& error) {
        SynchronizeAfterError();
        throw;
    }
}

bool Parser::IsDone() {
    // IsAtEnd should have the same meaning now
    return IsAtEnd();
}

Expr::IExprPtr Parser::ParseBooleanExpression() {
    auto expr = ParseAssignment();
    if (!IsAtEnd()) {
        throw TokenError(PeekToken(), "Expected end of expression.");
    }

    return expr;
}

Expr::IExprPtr Parser::ParseAssignment() {
    auto expr = ParseComma();

    // Ternary operator
    if (MatchTokenType(TokenType::QUESTION)) {
        const auto& questionOper = PreviousToken();

        const auto left = ParseComma();
        const auto& colonOper = ConsumeToken(TokenType::COLON, "Expect ':' after expression.");
        const auto right = ParseComma(); // TODO: this is a bit odd as we can chain assignments but ternaryOperators. Should this be in this method?

        const auto assignmentExpr = std::make_shared<Expr::Binary>(left, std::make_shared<Token>(colonOper), right);
        return std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(questionOper), assignmentExpr);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseComma() {
    auto expr = ParseLogicOr();
    while (MatchTokenType(TokenType::COMMA)) {
        const auto& oper = PreviousToken();
        const auto right = ParseLogicOr();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseLogicOr() {
    auto expr = ParseLogicAnd();

    while (MatchTokenType(TokenType::LOGIC_OR)) {
        const auto& oper = PreviousToken();
        const auto right = ParseLogicAnd();
        expr = std::make_shared<Expr::Logical>(expr, std::make_shared<Token>(oper), right);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseLogicAnd() {
    auto expr = ParseBitwiseOr();

    while (MatchTokenType(TokenType::LOGIC_AND)) {
        const auto& oper = PreviousToken();
        const auto right = ParseBitwiseOr();
        expr = std::make_shared<Expr::Logical>(expr, std::make_shared<Token>(oper), right);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseBitwiseOr() {
    auto expr = ParseBitwiseXor();

    while (MatchTokenType(TokenType::BITWISE_OR)) {
        const auto& oper = PreviousToken();
        const auto right = ParseBitwiseXor();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseBitwiseXor() {
    auto expr = ParseBitwiseAnd();

    while (MatchTokenType(TokenType::BITWISE_XOR)) {
        const auto& oper = PreviousToken();
        const auto right = ParseBitwiseAnd();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseBitwiseAnd() {
    auto expr = ParseEquality();

    while (MatchTokenType(TokenType::BITWISE_AND)) {
        const auto& oper = PreviousToken();
        const auto right = ParseEquality();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseEquality() {
    auto expr = ParseComparison();
    while (MatchTokenType({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL })) {
        Token oper = PreviousToken();
        auto right = ParseComparison();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }

    return expr;
}

Expr::IExprPtr Parser::ParseComparison() {
    auto expr = ParseTerm();

    while (MatchTokenType({ TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL })) {
        Token oper = PreviousToken();
        auto right = ParseTerm();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }
    return expr;
}

Expr::IExprPtr Parser::ParseTerm() {
    auto expr = ParseFactor();

    while (MatchTokenType({ TokenType::MINUS, TokenType::PLUS })) {
        Token oper = PreviousToken();
        auto right = ParseFactor();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }
    return expr;
}

Expr::IExprPtr Parser::ParseFactor() {
    auto expr = ParseUnary();

    while (MatchTokenType({ TokenType::SLASH, TokenType::STAR })) {
        Token oper = PreviousToken();
        auto right = ParseUnary();
        expr = std::make_shared<Expr::Binary>(expr, std::make_shared<Token>(oper), right);
    }
    return expr;
}

Expr::IExprPtr Parser::ParseUnary() {
    if (MatchTokenType({ TokenType::MINUS, TokenType::BANG, TokenType::STAR })) {
        Token oper = PreviousToken();
        auto right = ParseUnary();
        return std::make_shared<Expr::Unary>(std::make_shared<Token>(oper), right);
    }
    return ParsePrimary();
}

Expr::IExprPtr Parser::ParsePrimary() {
    // TODO: literals are currently strings, could move these to boolean, nullptr(maybe a custom isNil strong type?), number, string
    if (MatchTokenType(TokenType::FALSE)) { return std::make_shared<Expr::Literal>(LiteralObject{ false }); }
    if (MatchTokenType(TokenType::TRUE)) { return std::make_shared<Expr::Literal>(LiteralObject{ true }); }

    if (MatchTokenType(TokenType::IDENTIFIER)) {
        // TODO: Register look up
        return std::make_shared<Expr::Variable>(std::make_shared<Token>(PreviousToken()));
    }

    if (MatchTokenType({ TokenType::NUMBER })) {
        return std::make_shared<Expr::Literal>(PreviousToken().GetLiteral());
    }

    if (MatchTokenType(TokenType::LEFT_PAREN)) {
        auto expr = ParseAssignment();
        ConsumeToken(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Expr::Grouping>(expr);
    }

    throw TokenError(PeekToken(), "Expect expression.");
}

bool Parser::MatchTokenType(TokenType tokenType) {
    return MatchTokenType(std::vector{ tokenType });
}

bool Parser::MatchTokenType(const std::vector<TokenType>& tokenTypes) {
    for (const auto tokenType : tokenTypes) {
        if (CheckTokenType(tokenType)) {
            AdvanceToken();
            return true;
        }
    }
    return false;
}
bool Parser::CheckTokenType(TokenType type) {
    if (IsAtEnd()) { return false; }
    return PeekToken().GetType() == type;
}

bool Parser::IsAtEnd() {
    return PeekToken().GetType() == TokenType::END_OF_FILE;
}

Token& Parser::AdvanceToken() {
    if (!IsAtEnd()) {
        m_tokenIndex++;
    }
    return PreviousToken();
}

Token& Parser::PeekToken() {
    return m_tokens.at(m_tokenIndex);
}

Token& Parser::PreviousToken() {
    return m_tokens.at(m_tokenIndex - 1ull);
}

Token& Parser::ConsumeToken(TokenType tokenType, std::string_view errorMessage) {
    if (CheckTokenType(tokenType)) { return AdvanceToken(); }

    throw TokenError(PeekToken(), errorMessage);
}

ParseError Parser::TokenError(const Token& token, std::string_view errorMessage) {
    m_errors->Error(token, errorMessage);
    return ParseError(std::string(errorMessage));
}

void Parser::SynchronizeAfterError() {
    if (IsAtEnd()) { return; }

    AdvanceToken();
    while (!IsAtEnd()) {
        AdvanceToken();
    }
}