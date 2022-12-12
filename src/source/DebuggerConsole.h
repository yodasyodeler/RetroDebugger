#pragma once

#include "DebuggerInterpreter.h"

class DebuggerConsole {
public:
    DebuggerConsole(DebuggerInterpreter* interpreter);

    bool AdvanceDebugger(const std::string& command);

    std::string GetResponse();

    size_t GetResponseLength();

    std::string GetPrompt();

private:
    bool ParseCommand(std::vector<std::string>& words);

    DebuggerInterpreter* m_interpreter;
    std::vector<std::string> m_prevWords = { "" };
};