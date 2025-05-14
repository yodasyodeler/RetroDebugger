#pragma once

#include "Token.h"
#include "RuntimeError.h"

#include <fmt/core.h>

#include <string>
#include <string_view>


// TODO: This is really a placeholder for now, I want to comeback and redo it all once I have a better idea of what I want here.
class Errors {
public:
    void Report(int line, std::string_view where, std::string_view message);

    void Error(int line, std::string_view message);
    void Error(const Token& token, std::string_view message);

    void ReportRuntimeError(const RuntimeError& error);

    bool HasError();
    bool HasParsingError();
    bool HasRuntimeError();

    void ClearError();
    std::string& GetError();

private:
    std::string m_lastError;
    bool m_hasError = false;
    bool m_hasRuntimeError = false;
};

using ErrorsPtr = std::shared_ptr<Errors>;
