

#include "DebuggerInterpreter.h"

#include "DebuggerPrintFormat.h"
#include "DebuggerStringParser.h"
#include "IDebuggerCallbacks.h"

#include <numeric>
#include <sstream>

namespace Rdb {

DebuggerInterpreter::DebuggerInterpreter(Debugger* debugger, std::shared_ptr<IDebuggerCallbacks> callbacks) :
    m_callbacks(std::move(callbacks)),
    m_debugger(debugger) {}

std::string DebuggerInterpreter::GetCommandResponse() const {
    return m_commandResponse;
}

void DebuggerInterpreter::SetCommandResponse(std::string response) {
    m_commandResponse = std::move(response);
}

size_t DebuggerInterpreter::GetCommandResponseLength() const {
    return m_commandResponse.size();
}

bool DebuggerInterpreter::Help(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();

    if (cmdCount == 1) {
        SetCommandResponse(DebuggerPrintFormat::PrintGeneralHelp());
    }
    else if (cmdCount == 2) {
        SetCommandResponse(DebuggerPrintFormat::PrintCommandHelp(words[1]));
    }
    else {
        SetCommandResponse(DebuggerPrintFormat::PrintHelpHelp());
    }
    return true;
}

bool DebuggerInterpreter::Continue(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();
    auto number = 0u;

    if (cmdCount == 1 || (cmdCount == 2 && DebuggerStringParser::ParseNumber(words[1], number))) {
        m_debugger->Run(number);
        m_listNext = false;
        return true;
    }
    return false;
}

bool DebuggerInterpreter::Step(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();
    auto number = 0u;

    if ((cmdCount == 1) || (cmdCount == 2 && DebuggerStringParser::ParseNumber(words[1], number))) {
        m_debugger->RunInstructions(number);
        m_listNext = false;
        return true;
    }
    return false;
}

bool DebuggerInterpreter::Finish(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    if (words.size() == 1) {
        m_debugger->RunTillJump();
        m_listNext = false;
        return true;
    }
    return false;
}

bool DebuggerInterpreter::SetBreakpoint(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();

    if (cmdCount == 1) {
        m_debugger->SetBreakpoint(m_callbacks->GetPcReg());
        return true;
    }
    if (cmdCount == 2) {
        unsigned int bankNumber{};
        unsigned int breakNumber{};
        if (DebuggerStringParser::ParseNumber(words[1], breakNumber)) {
            m_debugger->SetBreakpoint(breakNumber);
            return true;
        }
        else if (DebuggerStringParser::ParseNumberPair(words[1], bankNumber, breakNumber, ":")) {
            m_debugger->SetBreakpoint(bankNumber, breakNumber);
            return true;
        }
    }
    return false;
}

bool DebuggerInterpreter::EnableBreak(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();
    std::vector<unsigned int> numbers;

    if (cmdCount == 2 && DebuggerStringParser::ParseList(words[1], numbers)) {
        m_debugger->EnableBreakpoints(numbers);
        return true;
    }
    return false;
}

bool DebuggerInterpreter::DisableBreak(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();
    std::vector<unsigned int> numbers;

    if (cmdCount == 2 && DebuggerStringParser::ParseList(words[1], numbers)) {
        m_debugger->DisableBreakpoints(numbers);
        return true;
    }
    return false;
}

bool DebuggerInterpreter::DeleteBreakpoints(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();
    std::vector<unsigned int> numbers;

    if (cmdCount == 1) {
        m_debugger->DeleteBreakpoints();
        return true;
    }
    else if (cmdCount == 2 && DebuggerStringParser::ParseList(words[1], numbers)) {
        m_debugger->DeleteBreakpoints(numbers);
        return true;
    }
    return false;
}

bool DebuggerInterpreter::GetInfo(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();

    if (cmdCount <= 1) { return false; }

    if ((words[1] == "break") || (words[1] == "breakpoint") || (words[1] == "watchpoint")) {
        std::vector<unsigned int> numbers;
        if (cmdCount == 2) {
            auto info = m_debugger->GetBreakpointInfoList();
            if (!info.empty()) {
                SetCommandResponse(DebuggerPrintFormat::PrintBreakInfo(info));
            }
            return true;
        }
        else if (cmdCount == 3 && DebuggerStringParser::ParseList(words[2], numbers)) {
            auto info = m_debugger->GetBreakpointInfoList(numbers);
            if (!info.empty()) {
                SetCommandResponse(DebuggerPrintFormat::PrintBreakInfo(info));
            }
            return true;
        }
    }
    else if (words[1] == "line") {
        if (cmdCount == 2) {
            SetCommandResponse(DebuggerPrintFormat::PrintLineInfo(m_callbacks->GetPcReg()));
            return true;
        }
    }
    return false;
}

bool DebuggerInterpreter::SetWatch(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();

    if (cmdCount == 2) {
        unsigned int address{};
        if (DebuggerStringParser::ParseNumber(words[1], address)) {
            return m_debugger->SetWatchpoint(address) != std::numeric_limits<BreakNum>::max();
        }
        /*else if (DebuggerStringParser::ParseNumberPair(words[1], addressStart, addressEnd, "-")) {
            return m_debugger->SetWatchpoint(addressStart) != std::numeric_limits<BreakNum>::max();
        }*/
    }
    return false;
}

bool DebuggerInterpreter::SetReadWatch(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();

    if (cmdCount == 2) {
        unsigned int address{};
        if (DebuggerStringParser::ParseNumber(words[1], address)) {
            return m_debugger->SetReadWatchpoint(address) != std::numeric_limits<BreakNum>::max();
        }
    }
    return false;
}

bool DebuggerInterpreter::SetAnyWatch(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();

    if (cmdCount == 2) {
        unsigned int address{};
        if (DebuggerStringParser::ParseNumber(words[1], address)) {
            return m_debugger->SetAnyWatchpoint(address) != std::numeric_limits<BreakNum>::max();
        }
    }
    return false;
}

bool DebuggerInterpreter::Print(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();

    if (cmdCount == 2) {
        unsigned int number{};
        const auto regSet = m_callbacks->GetRegSet();
        auto reg = regSet.find(words[1]);
        if (reg != regSet.end()) {
            SetCommandResponse(DebuggerPrintFormat::PrintRegister(reg->first, reg->second) + "\n");
            return true;
        }
        // TODO: case insensitive
        else if ((words[1] == "registers") || (words[1] == "reg")) {
            SetCommandResponse(DebuggerPrintFormat::PrintAllRegisters(regSet));
            return true;
        }
        // else if (false) {
        //     const auto info = m_debugger->GetRegInfo(0); //TODO: not implemented
        //     SetCommandResponse(DebuggerPrintFormat::PrintMemoryMappedRegInfo(info));
        //     return true;
        // }
        else if (DebuggerStringParser::ParseNumber(words[1], number)) {
            const auto info = m_debugger->GetRomInfo(number);
            SetCommandResponse(DebuggerPrintFormat::PrintAddressInfo(info));
            return true;
        }
    }
    return false;
}

// TODO: Make it closer to Linux GDB, only supports starting address, not what is expected of list.
bool DebuggerInterpreter::List(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    const auto cmdCount = words.size();
    unsigned int number = m_callbacks->GetPcReg();
    unsigned int listSize = m_listSize;

    bool isAddressRange = false; // TODO: hack for now. Should rethink this to make it simpler

    if ((cmdCount == 1) || (cmdCount == 2)) {
        if (cmdCount == 1 && m_listNext) { number = static_cast<unsigned int>(m_listAddress); }
        else if (unsigned int number2 = 0;
            cmdCount == 2 && DebuggerStringParser::ParseNumberPair(words[1], number, number2, "-")) {
            if (number > number2) {
                m_listNext = false;
                return false;
            }
            isAddressRange = true;
            listSize = number2;
        }
        else if (cmdCount == 2 && !DebuggerStringParser::ParseNumber(words[1], number)) {
            m_listNext = false;
            return false;
        }

        auto commands = isAddressRange ? m_debugger->GetCommandInfoList(static_cast<size_t>(number), static_cast<size_t>(listSize)) : m_debugger->GetCommandInfoList(static_cast<size_t>(number), listSize);

        SetCommandResponse(DebuggerPrintFormat::PrintInstructions(m_callbacks, commands));
        if (!commands.empty()) {
            auto command = commands.rbegin();
            auto lastAddress = command->first;

            const auto& args = command->second.arguments;
            m_listAddress = std::accumulate(args.begin(), args.end(), lastAddress + 1, [](auto sum, auto arg) { return sum + GetArgTypeLength(arg->type); });
        }
        m_listNext = true;
        return true;
    }
    m_listNext = false;
    return false;
}

bool DebuggerInterpreter::Set(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    unsigned int number{};

    if (words.size() != 3) { return false; } // TODO: look for more uses of set to verify if this check is valid.

    if (words[1] == "listsize" && DebuggerStringParser::ParseNumber(words[2], number)) {
        return SetListsize(number);
    }
    return false;
}

bool DebuggerInterpreter::Show(const std::vector<std::string>& words) {
    m_commandResponse.clear();
    if (words[1] == "listsize") {
        SetCommandResponse(DebuggerPrintFormat::PrintListsize(m_listSize));
        return true;
    }
    return false;
}

// Debug variable Setters
bool DebuggerInterpreter::SetListsize(const unsigned int listsize) {
    m_commandResponse.clear();
    m_listSize = listsize;
    return true;
}

}