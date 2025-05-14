#pragma once

#include "Report.h"
#include "Token/Token.h"

#include <string_view>
#include <vector>

class Scanner {
public:
    Scanner(ErrorsPtr errors, std::string_view source);

    TokenList ScanTokens();

private:
    struct Cursor
    {
        int current = 0;
        int start = 0;
        int line = 1; // Line number is not 0 indexed
    };
    void ScanToken(Cursor& cursor);
    void ScanNumericLiteral(Cursor& cursor);
    void ScanIdentifier(Cursor& cursor);

    bool IsAtEnd(const Cursor& cursor);

    char Peek(const Cursor& cursor);
    char PeekAhead(Cursor cursor);

    char Pop(Cursor& cursor);


    std::string_view m_source;
    TokenList m_tokenList;

    ErrorsPtr m_errors;
};