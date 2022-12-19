#pragma once

#include <string>
#include <vector>

#include "Debugger.h"

class DebuggerInterpreter {
public:
    explicit DebuggerInterpreter(Debugger* debugger);

    [[nodiscard]] std::string GetCommandResponse() const;
    void SetCommandResponse(std::string response);
    size_t GetCommandResponseLength() const;

    bool Help(const std::vector<std::string>& words);
    bool Continue(const std::vector<std::string>& words);
    bool Step(const std::vector<std::string>& words);
    bool Finish(const std::vector<std::string>& words);
    bool SetBreakpoint(const std::vector<std::string>& words);
    bool EnableBreak(const std::vector<std::string>& words);
    bool DisableBreak(const std::vector<std::string>& words);
    bool DeleteBreakpoints(const std::vector<std::string>& words);
    bool GetInfo(const std::vector<std::string>& words);
    bool SetWatch(const std::vector<std::string>& words);
    bool Print(const std::vector<std::string>& words);
    bool List(const std::vector<std::string>& words);
    bool Set(const std::vector<std::string>& words);
    bool Show(const std::vector<std::string>& words);

private:
    bool SetListsize(unsigned int listsize);

    bool m_listNext = false;
    unsigned int m_listsize = 10;
    size_t m_listAddress = 0;
    std::string m_commandResponse;
    Debugger* m_debugger;
};