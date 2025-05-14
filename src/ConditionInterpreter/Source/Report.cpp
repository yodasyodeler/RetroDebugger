#include "Report.h"


void Errors::Report(int line, std::string_view where, std::string_view message) {
    m_lastError = fmt::format("[Line {}] Error - \"{}\": {}\n", line, where, message);
    std::puts(m_lastError.c_str());
    m_hasError = true;
}

void Errors::Error(int line, std::string_view message) {
    Report(line, "", message);
}

void Errors::Error(const Token& token, std::string_view message) {
    if (token.GetType() == TokenType::END_OF_FILE) {
        Report(token.GetOffset(), " at end", message);
    }
    else {
        Report(token.GetOffset(), fmt::format(" at '{}'", token.GetLexeme()), message);
    }
}

void Errors::ReportRuntimeError(const RuntimeError& error) {
    m_lastError = fmt::format("{}\n[line {}]", error.what(), error.GetToken()->GetOffset());
    std::puts(m_lastError.c_str());
    m_hasRuntimeError = true;
}

bool Errors::HasError() { return m_hasError || m_hasRuntimeError; }

bool Errors::HasParsingError() {
    return m_hasError;
}

bool Errors::HasRuntimeError() {
    return m_hasRuntimeError;
}

void Errors::ClearError() {
    m_hasError = false;
    m_hasRuntimeError = false;
}

std::string& Errors::GetError() { return m_lastError; }
