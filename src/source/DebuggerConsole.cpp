#include "DebuggerConsole.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <windows.h>

#include "DebuggerStringParser.h"

static const std::string DebuggerPrompt = "(rdb)";


DebuggerConsole::DebuggerConsole(DebuggerInterpreter* interpreter) :
    m_interpreter(interpreter) {}

bool DebuggerConsole::AdvanceDebugger(const std::string& command) {
    auto words = DebuggerStringParser::ParseBuffer(command);
    auto leaveDebugger = ParseCommand(words);
    m_prevWords = words;
    return leaveDebugger;
}

std::string DebuggerConsole::GetPrompt() {
    return DebuggerPrompt;
}

std::string DebuggerConsole::GetResponse() {
    return m_interpreter->GetCommandResponse();
}

size_t DebuggerConsole::GetResponseLength() {
    return m_interpreter->GetCommandResponseLength();
}

// TODO: look at using a map, may be easier to maintain, add alias etc.
// fall through if invalid argument
bool DebuggerConsole::ParseCommand(std::vector<std::string>& words) {
    std::stringstream stream;

    if (words.empty())
        return false;

    auto cmd = words[0];
    if (cmd == "") {
        words = m_prevWords;
        cmd = words[0];
        if (cmd == "") {
            return false;
        }
    }

    if ((cmd == "h") || (cmd == "help")) {
        if (m_interpreter->Help(words)) return false;
    }
    else if ((cmd == "c") || (cmd == "continue")) {
        if (m_interpreter->Continue(words)) return true;
    }
    else if ((cmd == "s") || (cmd == "step")) {
        if (m_interpreter->Step(words)) return true;
    }
    else if ((cmd == "f") || (cmd == "finish")) {
        if (m_interpreter->Finish(words)) return true;
    }
    else if ((cmd == "b") || (cmd == "break")) {
        if (m_interpreter->SetBreakpoint(words)) return false;
    }
    else if (cmd == "enable") {
        if (m_interpreter->EnableBreak(words)) return false;
    }
    else if (cmd == "disable") {
        if (m_interpreter->DisableBreak(words)) return false;
    }
    else if ((cmd == "d") || (cmd == "delete")) {
        if (m_interpreter->DeleteBreakpoints(words)) return false;
    }
    else if ((cmd == "i") || (cmd == "info")) {
        if (m_interpreter->GetInfo(words)) return false;
    }
    else if ((cmd == "w") || (cmd == "watch")) {
        if (m_interpreter->SetWatch(words)) return false;
    }
    else if ((cmd == "p") || (cmd == "print")) {
        if (m_interpreter->Print(words)) return false;
    }
    else if ((cmd == "l") || (cmd == "list")) {
        if (m_interpreter->List(words)) return false;
    }
    else if (cmd == "set") {
        if (m_interpreter->Set(words)) return false;
    }
    else if (cmd == "show") {
        if (m_interpreter->Show(words)) return false;
    }
    else if (cmd == "quit") {
        exit(EXIT_SUCCESS);
    }
    else {
        stream << "Undefined command: \"" << cmd << "\" Try \"help\" \n";
        m_interpreter->SetCommandResponse(stream.str());
        return false;
    }

    stream << "Invalid arg, \"help " << cmd << "\" for info on command and args\n";
    m_interpreter->SetCommandResponse(stream.str());
    return false;
}
