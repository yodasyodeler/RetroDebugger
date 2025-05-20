#pragma once

#include "Debugger.h"

#include <memory>
#include <string>
#include <string_view>


namespace Rdb {

struct ConsoleSettings {
    std::string previousCommand;
    std::string commandResponse;
    unsigned int listAddress = 0;
    bool listNext = false;
    size_t listSize = 10;
};

class ConsoleInterpreter {
public:
    ConsoleInterpreter(std::shared_ptr<Debugger> debugger, std::shared_ptr<IDebuggerCallbacks> callbacks);

    // Main entry point
    bool AdvanceDebugger(const std::string& command);

    // Command Response methods
    [[nodiscard]] std::string GetCommandResponse() const;
    void SetCommandResponse(std::string response);
    size_t GetCommandResponseLength() const;

    static std::string GetPrompt();

private:
    // Commands
    bool HelpCommand(std::string_view command);
    bool ContinueCommand(std::string_view command);
    bool StepCommand(std::string_view command);
    bool FinishCommand(std::string_view command);
    bool BreakCommand(std::string_view command);
    bool ConditionCommand(std::string_view command);
    bool EnableBreakCommand(std::string_view command);
    bool DisableBreakCommand(std::string_view command);
    bool DeleteBreakCommand(std::string_view command);
    bool InfoCommand(std::string_view command);
    bool WatchCommand(std::string_view command);
    bool RwatchCommand(std::string_view command);
    bool AwatchCommand(std::string_view command);
    bool PrintCommand(std::string_view command);
    bool ListCommand(std::string_view command);
    bool SetCommand(std::string_view command);
    bool ShowCommand(std::string_view command);


    // Member variables
    std::string m_command;
    ConsoleSettings m_settings;
    std::shared_ptr<IDebuggerCallbacks> m_callbacks;
    std::shared_ptr<Debugger> m_debugger;
};

}