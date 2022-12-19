#include "DebuggerPrintFormat.h"

#include "DebuggerCallbacks.h"

#include <NamedType/named_type.hpp>
#include <fmt/core.h>

#include <iomanip>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <map>

using namespace std::literals;

static constexpr size_t SizeOfByte = 8;
static constexpr size_t SizeOfWord = 16;

using Value = fluent::NamedType<int, struct ValueTag>;
using Length = fluent::NamedType<size_t, struct LengthTag>;

// TODO: Redo the to_string, and name something else. Also maybe move to a common debugger file?
static std::string to_string(Value value, Length length, bool isHex) {
    if (isHex) {
        if (length.get() > SizeOfWord) { return fmt::format("0x{:08X}", value.get()); }
        else if (length.get() > SizeOfByte) { return fmt::format("0x{:04X}", value.get()); }
        else { return fmt::format("0x{:03}", value.get()); }
    }
    else {
        if (length.get() > SizeOfWord) { return fmt::format("0d{:010}", value.get()); }
        else if (length.get() > SizeOfByte) { return fmt::format("0d{:05}", value.get()); }
        else { return fmt::format("0d{:03}", value.get()); }
    }
}

static std::string to_string(uint8_t value, bool isHex = false) {
    return to_string(Value(value), Length(SizeOfByte), isHex);
}

static std::string to_string(uint16_t value, bool isHex = false) {
    return to_string(Value(value), Length(SizeOfWord), isHex);
}

// static std::string to_string(int8_t value, bool isHex = false) {
//     return to_string(value, 8, isHex);
// }
//
// static std::string to_string(int16_t value, bool isHex = false) {
//     return to_string(value, 16, isHex);
// }

// TODO: Command "info line" doesn't have much use right now. Maybe should remove.
// TODO: Will need to do an optimization pass on string operations later.
// TODO: Look for a more expandable solution. C++20 std::format may be worth looking at when available.

static constexpr std::string_view generalHelp =
    "[h]elp <cmd> -- display help for specific command\n"
    "\n"
    "[c]ontinue -- continue execution of linkboy till a break\n"
    "[c]ontinue <count> -- continue execution of linkboy ignore breakpoints till count breakpoints have been encountered\n"
    "[s]tep -- execute one instruction then break\n"
    "[s]tep <count> -- execute count of instructions then break\n"
    "[f]inish -- continue execution till a jump instruction\n"
    "\n"
    "[b]reak -- set a breakpoint at current instruction\n"
    "[b]reak <address> -- set breakpoint at address\n"
    "enable <number>-- enable breakpoint number\n"
    "enable <number-number>-- enable breakpoint number range\n"
    "disable <number>-- disable breakpoint number\n"
    "disable <number-number>-- disable breakpoint number range\n"
    "[d]elete -- delete all breakpoints\n"
    "[d]elete <number> -- delete breakpoint number\n"
    "[d]elete <number-number> -- delete breakpoint number range\n"
    "[i]nfo break -- list breakpoints\n"
    "[i]nfo breakpoints -- list breakpoints\n"
    "[i]nfo line -- prints the current line number and its associated address\n"
    "\n"
    "[w]atch <reg> -- break on next instruction that writes to the specified reg and its value changes\n"
    "[w]atch <address> -- break on next instruction that writes to the specified address and its value changes\n"
    "[w]atch <address-address> -- break on next instruction that writes to the specified address and its value changes\n"
    "\n"
    "[p]rint <address> -- print value at address\n"
    "[l]ist -- print instructions at current address\n"
    "[l]ist <address> -- print instructions at address\n"
    "[l]ist <address-address> -- print instructions from range of addresses\n"
    "\n"
    "set <debugger variable> <count> -- set the size of list commands output\n"
    "show <debugger variable> -- print debugger variable value\n";


namespace DebuggerPrintFormat {
std::string PrintGeneralHelp() {
    return std::string(generalHelp);
}

std::string PrintHelpHelp() {
    return "TODO: write help for help command\n Help commands are WIP.\n";
}

std::string PrintCommandHelp(const std::string& /*command*/) { return "TODO: write help\n"; }

std::string PrintRegHelp() { return "TODO: write help\n"; }

std::string PrintPpuHelp() { return "TODO: write help\n"; }

std::string PrintApuHelp() { return "TODO: write help\n"; }

std::string PrintBreakpointHit(BreakInfo breakInfo) {
    std::stringstream msg;
    msg << "Breakpoint " << std::to_string(breakInfo.breakpointNumber) << " , at 0x" << to_string(static_cast<uint16_t>(breakInfo.address), true);
    return msg.str();
}

std::string PrintWatchpointHit(BreakInfo breakInfo) {
    std::stringstream msg;
    msg << "Watchpoint " << std::to_string(breakInfo.breakpointNumber) << ": at " << to_string(static_cast<uint16_t>(breakInfo.address), true) << "\n";
    msg << "Old value = " << std::to_string(breakInfo.oldWatchValue) << "\n";
    msg << "New value = " << std::to_string(breakInfo.newWatchValue);
    return msg.str();
}

std::string PrintTimerHelp() { return "TODO: write help\n"; }

std::string PrintBreakInfo(const std::map<BreakNum, BreakInfo>& breakInfo) {
    const char* Num = "Num";
    const char* Type = "Type";
    const char* Disp = "Disp";
    const char* Enb = "Enb";
    const char* Address = "Address";
    const char* What = "What";

    std::string breakInfoStr = fmt::format(
        "{:-<8s}{:-<15s}{:-<5s}{:-<4s}{:-<19s}{}\n",
        Num,
        Type,
        Disp,
        Enb,
        Address,
        What);
    for (const auto& info : breakInfo) {
        const auto what = (info.second.type == BreakType::BankBreakpoint) ? "Bank: "s + std::to_string(info.second.bankNumber) : ""s;
        breakInfoStr += fmt::format(
            "{:-<8d}{:-<15s}{:-<5s}{:-<4s}0x{:016X}{}\n",
            info.first,
            BreakTypeToString.at(info.second.type),
            BreakDispToString.at(info.second.disp),
            (info.second.isEnabled ? "y" : "n"),
            info.second.address,
            what);

        if (info.second.timesHit != 0U) {
            breakInfoStr += fmt::format("{} already hit {} times\n", BreakTypeToString.at(info.second.type), info.second.timesHit);
        }
    }
    return breakInfoStr;
}

// TODO: because I'm focusing on assembly there isn't much I can print for the line, however I think there is potential to add some nice to have info here.
// Line 7 of "source/main.cpp" starts at address 0x40158a <main()+26>
//    and ends at 0x40159d <main()+45>.
std::string PrintLineInfo(const unsigned int line) {
    std::stringstream msg;
    msg << "Line " << line << ", address 0x" << std::hex << line << "\n";
    return msg.str();
}

std::string PrintAllRegisters(const RegSet& regset) {
    std::string msg;
    for (const auto& reg : regset) {
        msg += "  " + PrintRegister(reg.first, reg.second) + "\n";
    }
    return msg;
}


std::string PrintRegister(const std::string& name, unsigned int value) {
    std::stringstream msg;
    msg << name << "(0x" << std::hex << value << ")";
    return msg.str();
}

std::string PrintMemoryMappedRegInfo(const RegInfo& /*info*/) {
    return "TODO: do me\n";
}

std::string PrintAddressInfo(const AddrInfo& info) {
    std::stringstream msg;
    msg << to_string(static_cast<uint16_t>(info.address), true) << "  " << to_string(static_cast<uint8_t>(info.value), true) << "\n"; // TODO: need to setup data width, address width
    return msg.str();
}

std::string PrintInstructions(const CommandList& commandInfo) {
    std::string temp;
    for (const auto& info : commandInfo) {
        std::stringstream msg;
        msg << to_string(static_cast<uint16_t>(info.first), true) << "  " << info.second.info->name << "\t  "; // TODO: opcodeLength is not accounted for

        // TODO: need to rethink this. Can't assume the memory value that is read from the callback.
        temp += msg.str();

        const auto byte = static_cast<uint8_t>(DebuggerCallback::ReadMemory(static_cast<unsigned int>(info.first) + 1));
        const auto word = static_cast<uint16_t>((DebuggerCallback::ReadMemory(static_cast<unsigned int>(info.first) + 2U) << 8U) | byte); // TODO: Code smell, this doesn't look like the best way to do this!

        for (const auto& arg : info.second.arguments) {
            if (arg->indirectArg && arg->type != ArgumentType::REG) { temp += "("; }

            if ((arg->type == ArgumentType::S8BIT) || (arg->type == ArgumentType::U8BIT)) { temp += to_string(byte, true); }
            else if ((arg->type == ArgumentType::S16BIT) || (arg->type == ArgumentType::U16BIT)) { temp += to_string(word, true); }
            // else if ((arg->type == ArgumentType::S32BIT) || (arg->type == ArgumentType::U32BIT)) {}
            else { temp += arg->name; }

            if (arg->indirectArg && arg->type != ArgumentType::REG) { temp += ")"; }
            if (arg != info.second.arguments.back()) { temp += ", "; }
        }
        temp += "\n";
    }
    return temp;
}

std::string PrintListsize(const unsigned int listsize) {
    return std::string("Number of source lines debugger will list by default is ") + std::to_string(listsize) + ".\n";
}
}