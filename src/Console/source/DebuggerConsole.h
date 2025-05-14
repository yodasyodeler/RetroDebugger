#pragma once

#include "DebuggerInterpreter.h"

namespace Rdb {

class DebuggerConsole {
public:
    explicit DebuggerConsole(DebuggerInterpreter* interpreter);

    bool AdvanceDebugger(const std::string& command);

    std::string GetResponse() const;

    size_t GetResponseLength() const;

    static std::string GetPrompt();

private:
    bool ParseCommand(std::vector<std::string>& words);

    DebuggerInterpreter* m_interpreter;
    std::vector<std::string> m_prevWords = { "" };
};

}