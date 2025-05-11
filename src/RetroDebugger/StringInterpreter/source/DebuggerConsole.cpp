#include "DebuggerConsole.h"

#include "DebuggerStringParser.h"

#include <fmt/format.h>

DebuggerConsole::DebuggerConsole(DebuggerInterpreter* interpreter) :
    m_interpreter(interpreter) {}

bool DebuggerConsole::AdvanceDebugger(const std::string& command) {
    auto words = DebuggerStringParser::ParseBuffer(command);
    auto leaveDebugger = ParseCommand(words);
    m_prevWords = words;
    return leaveDebugger;
}

std::string DebuggerConsole::GetPrompt() {
    static constexpr std::string_view DebuggerPrompt = "(rdb)";
    return std::string(DebuggerPrompt);
}

std::string DebuggerConsole::GetResponse() const {
    return m_interpreter->GetCommandResponse();
}

size_t DebuggerConsole::GetResponseLength() const {
    return m_interpreter->GetCommandResponseLength();
}

// TODO: look at using a map, may be easier to maintain, add alias etc.
// fall through if invalid argument
bool DebuggerConsole::ParseCommand(std::vector<std::string>& words) { // NOLINT (readability-function-cognitive-complexity) - Keeping command checking flat for maintainability.
    if (words.empty()) {
        return false;
    }

    auto cmd = words[0];
    if (cmd.empty()) {
        words = m_prevWords;
        cmd = words[0];
        if (cmd.empty()) {
            return false;
        }
    }

    if ((cmd == "h") || (cmd == "help")) {
        if (m_interpreter->Help(words)) { return false; }
    }
    else if ((cmd == "c") || (cmd == "continue")) {
        if (m_interpreter->Continue(words)) { return true; }
    }
    else if ((cmd == "s") || (cmd == "step")) {
        if (m_interpreter->Step(words)) { return true; }
    }
    else if ((cmd == "f") || (cmd == "finish")) {
        if (m_interpreter->Finish(words)) { return true; }
    }
    else if ((cmd == "b") || (cmd == "break")) {
        if (m_interpreter->SetBreakpoint(words)) { return false; }
    }
    else if (cmd == "enable") {
        if (m_interpreter->EnableBreak(words)) { return false; }
    }
    else if (cmd == "disable") {
        if (m_interpreter->DisableBreak(words)) { return false; }
    }
    else if ((cmd == "d") || (cmd == "delete")) {
        if (m_interpreter->DeleteBreakpoints(words)) { return false; }
    }
    else if ((cmd == "i") || (cmd == "info")) {
        if (m_interpreter->GetInfo(words)) { return false; }
    }
    else if ((cmd == "w") || (cmd == "watch")) {
        if (m_interpreter->SetWatch(words)) { return false; }
    }
    else if (cmd == "rwatch") {
        if (m_interpreter->SetReadWatch(words)) { return false; }
    }
    else if (cmd == "awatch") {
        if (m_interpreter->SetAnyWatch(words)) { return false; }
    }
    else if ((cmd == "p") || (cmd == "print")) {
        if (m_interpreter->Print(words)) { return false; }
    }
    else if ((cmd == "l") || (cmd == "list")) {
        if (m_interpreter->List(words)) { return false; }
    }
    else if (cmd == "set") {
        if (m_interpreter->Set(words)) { return false; }
    }
    else if (cmd == "show") {
        if (m_interpreter->Show(words)) { return false; }
    }
    // else if (cmd == "quit") {
    //     exit(EXIT_SUCCESS);
    // }
    else {
        m_interpreter->SetCommandResponse(fmt::format("Undefined command: \"{}\" Try \"help\" \n", cmd));
        return false;
    }

    m_interpreter->SetCommandResponse(fmt::format("Invalid arg, \"help {}\" for info on command and args\n", cmd));
    return false;
}
