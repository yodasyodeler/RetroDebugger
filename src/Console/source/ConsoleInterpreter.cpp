#include "ConsoleInterpreter.h"

#include "DebuggerPrintFormat.h"
#include "DebuggerStringParser.h"

#include <fmt/core.h>

#include <numeric>
#include <stdexcept>

namespace {
// Finds the first word in the command and the rest of the sentence. Will trim off spaces.
std::pair<std::string_view, std::string_view> SplitFirstWord(std::string_view command) {
    // Find where first word starts and ends
    // "word  sentence"
    //  ^   ^
    const auto startCount = command.find_first_not_of(' ');
    const auto endCount = command.find_first_of(' ', startCount);

    // Either all space or empty
    if (endCount == std::string_view::npos) {
        if (startCount == std::string_view::npos) {
            return { "", "" };
        }

        // Only 1 word
        if (startCount != std::string_view::npos) {
            return { { command.data() + startCount }, "" };
        }
    }

    // Trim leading spaces
    // "word  sentence"
    //        ^
    const auto startCount2 = command.find_first_not_of(' ', endCount);
    if (startCount2 == std::string_view::npos) {
        return { { command.data() + startCount, endCount }, {} };
    }
    return { { command.data() + startCount, endCount }, { command.data() + startCount2 } };
}

std::tuple<bool, BankNum, unsigned int> ParseAddress(std::string_view word) {
    auto wordStr = std::string(word);

    // <address>
    if (const auto [isNumber, address] = Rdb::ParseNumber(wordStr);
        isNumber) {
        return { isNumber, AnyBank, address };
    }

    // <bank>:<address>
    if (const auto [isNumber, bank, address] = Rdb::ParseNumberPair(wordStr, ":");
        isNumber) {
        return { isNumber, BankNum{ bank }, address };
    }

    return { false, {}, {} };
}
}

namespace Rdb {

ConsoleInterpreter::ConsoleInterpreter(std::shared_ptr<Debugger> debugger, std::shared_ptr<IDebuggerCallbacks> callbacks) :
    m_callbacks(std::move(callbacks)),
    m_debugger(std::move(debugger)) {}

bool ConsoleInterpreter::AdvanceDebugger(const std::string& command) {
    auto [word, sentence] = SplitFirstWord(command);

    if (word.empty()) {
        std::tie(word, sentence) = SplitFirstWord(command);
        if (word.empty()) {
            return false;
        }
    }
    else {
        m_command = command;
    }

    // Trim trailing spaces
    if (const auto lastChar = sentence.find_last_not_of(" ");
        lastChar != std::string_view::npos && !sentence.empty()) {
        sentence = { sentence.data(), lastChar + 1 };
    }

    try {
        if (word == "h" || word == "help") {
            if (HelpCommand(sentence)) { return false; }
        }
        else if (word == "c" || word == "continue") {
            if (ContinueCommand(sentence)) { return true; }
        }
        else if (word == "s" || word == "step") {
            if (StepCommand(sentence)) { return true; }
        }
        else if (word == "f" || word == "finish") {
            if (FinishCommand(sentence)) { return true; }
        }
        else if (word == "b" || word == "break") {
            if (BreakCommand(sentence)) { return false; }
        }
        else if (word == "condition") {
            if (ConditionCommand(sentence)) { return false; }
        }
        else if (word == "enable") {
            if (EnableBreakCommand(sentence)) { return false; }
        }
        else if (word == "disable") {
            if (DisableBreakCommand(sentence)) { return false; }
        }
        else if (word == "d" || word == "delete") {
            if (DeleteBreakCommand(sentence)) { return false; }
        }
        else if (word == "i" || word == "info") {
            if (InfoCommand(sentence)) { return false; }
        }
        else if (word == "w" || word == "watch") {
            if (WatchCommand(sentence)) { return false; }
        }
        else if (word == "rwatch") {
            if (RwatchCommand(sentence)) { return false; }
        }
        else if (word == "awatch") {
            if (AwatchCommand(sentence)) { return false; }
        }
        else if (word == "p" || word == "print") {
            if (PrintCommand(sentence)) { return false; }
        }
        else if (word == "l" || word == "list") {
            if (ListCommand(sentence)) { return false; }
        }
        else if (word == "set") {
            if (SetCommand(sentence)) { return false; }
        }
        else if (word == "show") {
            if (ShowCommand(sentence)) { return false; }
        }
        else {
            SetCommandResponse(fmt::format("Undefined command: \"{}\" Try \"help\" \n", word));
            return false;
        }
    }
    catch (const std::runtime_error&) {
        // TODO: maybe print some more info on the error here.
        // fall through
    }

    SetCommandResponse(fmt::format("Invalid arg, \"help {}\" for info on command and args\n", word));
    return false;
}

std::string ConsoleInterpreter::GetCommandResponse() const {
    return m_settings.commandResponse;
}

void ConsoleInterpreter::SetCommandResponse(std::string response) {
    m_settings.commandResponse = std::move(response);
}

size_t ConsoleInterpreter::GetCommandResponseLength() const {
    return m_settings.commandResponse.size();
}

std::string ConsoleInterpreter::GetPrompt() {
    static constexpr std::string_view DebuggerPrompt = "(rdb)";
    return std::string(DebuggerPrompt);
}

// Commands
bool ConsoleInterpreter::HelpCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    if (word.empty()) {
        SetCommandResponse(DebuggerPrintFormat::PrintGeneralHelp());
    }
    else if (sentence.empty()) {
        SetCommandResponse(DebuggerPrintFormat::PrintCommandHelp(word));
    }
    else {
        SetCommandResponse(DebuggerPrintFormat::PrintHelpHelp());
    }
    return true;
}

bool ConsoleInterpreter::ContinueCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // continue
    if (word.empty()) {
        m_debugger->Run();
        m_settings.listNext = false;
        return true;
    }

    // continue <number>
    if (const auto [isNumber, number] = Rdb::ParseNumber(std::string(word));
        isNumber && sentence.empty()) {
        m_debugger->Run(number);
        m_settings.listNext = false;
        return true;
    }

    return false;
}

bool ConsoleInterpreter::StepCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // Step
    if (word.empty()) {
        m_debugger->RunInstructions();
        m_settings.listNext = false;
        return true;
    }

    // Step <number>
    if (const auto [isNumber, number] = Rdb::ParseNumber(std::string(word));
        isNumber && sentence.empty()) {
        m_debugger->RunInstructions(number);
        m_settings.listNext = false;
        return true;
    }

    return false;
}

bool ConsoleInterpreter::FinishCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // finish
    if (word.empty()) {
        m_debugger->RunTillJump();
        m_settings.listNext = false;
        return true;
    }

    return false;
}

bool ConsoleInterpreter::BreakCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // Break
    if (word.empty()) {
        m_debugger->SetBreakpoint(m_callbacks->GetPcReg());
        return true;
    }

    // break <address>...
    if (auto [isNumber, bankNum, address] = ParseAddress(word);
        isNumber) {
        const auto [subcommandWord, expressionSentence] = SplitFirstWord(sentence);

        // TODO: This creates the Breakpoint even if the condition fails, what does GDB do.
        //       Should this do a pre-check of the condition?

        // break <address>
        auto breakNum = m_debugger->SetBreakpoint(bankNum, address);

        // break <address> if <condition_expression>
        if (subcommandWord == "if") {
            m_debugger->SetCondition(breakNum, std::string(expressionSentence));
        }
        return true;
    }
    return false;
}

bool ConsoleInterpreter::ConditionCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // condition <break_number>
    // condition <break_number> <condition_expression>
    if (const auto [isNumber, number] = Rdb::ParseNumber(std::string(word));
        isNumber) {
        m_debugger->SetCondition(BreakNum{ number }, std::string{ sentence });
        return true;
    }
    return false;
}

bool ConsoleInterpreter::EnableBreakCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // enable <break_list>
    if (const auto [areNumbers, numbers] = Rdb::ParseList(std::string(word));
        areNumbers && sentence.empty()) {
        m_debugger->EnableBreakpoints(numbers);
        return true;
    }
    return false;
}

bool ConsoleInterpreter::DisableBreakCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // disable <break_list>
    if (const auto [areNumbers, numbers] = Rdb::ParseList(std::string(word));
        areNumbers && sentence.empty()) {
        m_debugger->DisableBreakpoints(numbers);
        return true;
    }
    return false;
}

bool ConsoleInterpreter::DeleteBreakCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // delete
    if (word.empty()) {
        m_debugger->DeleteBreakpoints();
        return true;
    }

    // delete <break_list>
    if (const auto [areNumbers, numbers] = Rdb::ParseList(std::string(word));
        areNumbers && sentence.empty()) {
        m_debugger->DeleteBreakpoints(numbers);
        return true;
    }
    return false;
}

bool ConsoleInterpreter::InfoCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    if (word == "break" || word == "breakpoint" || word == "watchpoint") {
        const auto [numWord, restOfSentence] = SplitFirstWord(sentence);

        // info (break | breakpoint | watchpoint)
        if (numWord.empty()) {
            auto info = m_debugger->GetBreakpointInfoList();
            if (!info.empty()) {
                SetCommandResponse(DebuggerPrintFormat::PrintBreakInfo(info));
            }
            return true;
        }

        // info (break | breakpoint | watchpoint) <break_list>
        if (const auto [areNumbers, numbers] = Rdb::ParseList(std::string(numWord));
            areNumbers && restOfSentence.empty()) {
            auto info = m_debugger->GetBreakpointInfoList(numbers);
            if (!info.empty()) {
                SetCommandResponse(DebuggerPrintFormat::PrintBreakInfo(info));
            }
            return true;
        }
    }

    // info line
    if (word == "line" && sentence.empty()) {
        SetCommandResponse(DebuggerPrintFormat::PrintLineInfo(m_callbacks->GetPcReg()));
        return true;
    }

    return false;
}

bool ConsoleInterpreter::WatchCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // watch <address>
    if (auto [isNumber, bankNum, address] = ParseAddress(word);
        isNumber && sentence.empty()) {
        return m_debugger->SetWatchpoint(address, bankNum) != std::numeric_limits<BreakNum>::max();
    }

    // TODO:
    // watch <register>
    /*if (!word.empty() && sentence.empty()) {
        return m_debugger->SetWatchpoint(std::string(word)) != std::numeric_limits<BreakNum>::max();
    }*/

    return false;
}

bool ConsoleInterpreter::RwatchCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // watch <address>
    if (auto [isNumber, bankNum, address] = ParseAddress(word);
        isNumber && sentence.empty()) {
        return m_debugger->SetReadWatchpoint(address, bankNum) != std::numeric_limits<BreakNum>::max();
    }

    // TODO:
    // watch <register>
    /*if (!word.empty() && sentence.empty()) {
        return m_debugger->SetReadWatchpoint(std::string(word)) != std::numeric_limits<BreakNum>::max();
    }*/

    return false;
}

bool ConsoleInterpreter::AwatchCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // watch <address>
    if (auto [isNumber, bankNum, address] = ParseAddress(word);
        isNumber && sentence.empty()) {
        return m_debugger->SetAnyWatchpoint(address, bankNum) != std::numeric_limits<BreakNum>::max();
        return true;
    }

    // TODO:
    // watch <register>
    /*if (!word.empty() && sentence.empty()) {
        return m_debugger->SetAnyWatchpoint(std::string(word)) != std::numeric_limits<BreakNum>::max();
    }*/

    return false;
}

bool ConsoleInterpreter::PrintCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    if (sentence.empty()) {
        // print ("reg" || "register")
        if (word == "reg" || word == "registers") {
            const auto regSet = m_callbacks->GetRegSet();
            SetCommandResponse(DebuggerPrintFormat::PrintAllRegisters(regSet));
            return true;
        }

        // print <address>
        if (const auto [isNumber, number] = Rdb::ParseNumber(std::string(word));
            isNumber) {
            const auto info = m_debugger->GetRomInfo(number);
            SetCommandResponse(DebuggerPrintFormat::PrintAddressInfo(info));
            return true;
        }

        // print <register>
        const auto regSet = m_callbacks->GetRegSet();
        if (auto reg = regSet.find(std::string(word));
            reg != regSet.end()) {
            SetCommandResponse(DebuggerPrintFormat::PrintRegister(reg->first, reg->second) + "\n");
            return true;
        }
    }

    return false;
}

bool ConsoleInterpreter::ListCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    const auto handleResponse = [this](const CommandList& commands) {
        if (!commands.empty()) {
            auto command = commands.rbegin();
            auto lastAddress = command->first;

            const auto& args = command->second.arguments;
            m_settings.listAddress = std::accumulate(args.begin(), args.end(), lastAddress + 1, [](auto sum, auto arg) { return sum + GetArgTypeLength(arg->type); });
        }
    };

    // list
    if (word.empty()) {
        auto address = m_settings.listNext ? m_settings.listAddress : m_callbacks->GetPcReg();
        auto commands = m_debugger->GetCommandInfoList(address, static_cast<unsigned int>(m_settings.listSize));
        SetCommandResponse(DebuggerPrintFormat::PrintInstructions(m_callbacks, commands));
        handleResponse(commands);
        m_settings.listNext = true;
        return true;
    }

    // list <address>
    if (const auto [isNumber, address] = Rdb::ParseNumber(std::string(word));
        isNumber && sentence.empty()) {
        auto commands = m_debugger->GetCommandInfoList(address, static_cast<unsigned int>(m_settings.listSize));
        SetCommandResponse(DebuggerPrintFormat::PrintInstructions(m_callbacks, commands));
        handleResponse(commands);
        m_settings.listNext = true;
        return true;
    }

    // list <address-address>
    if (const auto [isNumber, address1, address2] = Rdb::ParseNumberPair(std::string(word), "-");
        isNumber && sentence.empty()) {
        if (address1 > address2) {
            m_settings.listNext = false;
            return false;
        }

        auto commands = m_debugger->GetCommandInfoList(address1, size_t{ address2 }); // address to address
        SetCommandResponse(DebuggerPrintFormat::PrintInstructions(m_callbacks, commands));
        handleResponse(commands);
        m_settings.listNext = true;
        return true;
    }

    m_settings.listNext = false;
    return false;
}

bool ConsoleInterpreter::SetCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    // set "listsize" <number>
    if (word == "listsize") {
        const auto [listSize, extraWords] = SplitFirstWord(sentence);

        if (const auto [isNumber, number] = Rdb::ParseNumber(std::string(listSize));
            isNumber && extraWords.empty()) {
            m_settings.listSize = number;
        }
    }
    return false;
}

bool ConsoleInterpreter::ShowCommand(std::string_view command) {
    m_settings.commandResponse.clear();
    const auto [word, sentence] = SplitFirstWord(command);

    if (word == "listsize" && sentence.empty()) {
        SetCommandResponse(DebuggerPrintFormat::PrintListsize(m_settings.listSize));
        return true;
    }
    return false;
}

}