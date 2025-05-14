#pragma once

#include "Token/Token.h"


#include <stdexcept>

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(TokenPtr token, const std::string& errorMessage) :
        std::runtime_error(errorMessage),
        m_token(std::move(token)) {}

    TokenPtr GetToken() const { return m_token; }

private:
    TokenPtr m_token;
};