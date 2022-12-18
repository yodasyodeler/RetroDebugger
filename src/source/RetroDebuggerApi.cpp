#include "RetroDebuggerApi.h"
#include "DebuggerConsole.h"
#include "DebuggerInterpreter.h"
#include "DebuggerPrintFormat.h"
#include "DebuggerStringParser.h"

#include "RetroDebugger_config.h"

#include <iostream>
#include <numeric>

namespace Rdb {
// static BreakpointManager g_interpreter;
static Debugger g_debugger;
static DebuggerInterpreter g_interpreter(&g_debugger);
static DebuggerConsole g_console(&g_interpreter);

std::string GetRdbVersion() noexcept {
    return std::string(RetroDebugger::Config::ProjectVersion);
}

// Command interpreter calls
std::string GetCommandPrompt() noexcept {
    return g_console.GetPrompt();
}

std::string GetCommandResponse() {
    return g_console.GetResponse();
}

// size_t GetCommandResponseSize() {
//     return g_console.GetResponseLength();
// }

int ProcessCommandString(const std::string& message) {
    return (g_console.AdvanceDebugger(message)) ? 1 : 0; // TODO: move to enum
}

// Direct debugger calls
bool CheckBreakpoints(BreakInfo* breakInfo) {
    BreakInfo info = {};
    BreakInfo& infoRef = (breakInfo == nullptr) ? info : *breakInfo;

    const auto hitBreakpoint = g_debugger.CheckBreakpoints(infoRef);
    if (hitBreakpoint) {
        if (infoRef.type == BreakType::Breakpoint || infoRef.type == BreakType::BankBreakpoint) {
            g_interpreter.SetCommandResponse(DebuggerPrintFormat::PrintBreakpointHit(infoRef));
        }
        else if (infoRef.type == BreakType::Watchpoint) {
            g_interpreter.SetCommandResponse(DebuggerPrintFormat::PrintWatchpointHit(infoRef));
        }
    }
    return hitBreakpoint;
}

bool Run(const unsigned int numBreakpointsToSkip) {
    return g_debugger.Run(numBreakpointsToSkip);
}

bool RunInstructions(const unsigned int numBreakToPass) {
    return g_debugger.RunInstructions(numBreakToPass);
}

bool RunTillJump() {
    return g_debugger.RunTillJump();
}

bool SetBreakpoint(unsigned int address) {
    g_debugger.SetBreakpoint(address);
    return true; // TODO: this doesn't mean anything
}

bool EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }

    std::vector<unsigned int> breakRange(static_cast<size_t>(breakRange1 - breakRange0) + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return g_debugger.EnableBreakpoints(breakRange);
}

bool DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }

    std::vector<unsigned int> breakRange(static_cast<size_t>(breakRange1 - breakRange0) + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return g_debugger.DisableBreakpoints(breakRange);
}

bool DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }
    std::vector<unsigned int> breakRange(static_cast<size_t>(breakRange1 - breakRange0) + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return g_debugger.DeleteBreakpoints(breakRange);
}

BreakInfo GetBreakpointInfo(const unsigned int breakPointNum) {
    auto breakInfo = g_debugger.GetBreakpointInfoList({ breakPointNum });

    BreakInfo invalidBreakpoint = { static_cast<unsigned int>(-1), static_cast<unsigned int>(-1), static_cast<BreakNum>(-1), static_cast<unsigned int>(-1), 0, 0, BreakType::Invalid, BreakDisposition::Disable, false };
    return breakInfo.find(breakPointNum) != breakInfo.end() ? breakInfo.at(breakPointNum) : invalidBreakpoint;
}

bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo) {
    if (registerInfo != nullptr) {
        *registerInfo = g_debugger.GetRegisterInfoList();
        return true;
    }

    return false;
}

bool ParseXmlFile(const std::string& filename) {
    return g_debugger.ParseXmlFile(filename);
}

// Set Callbacks
void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb) {
    DebuggerCallback::SetGetPcRegCallback(std::move(getPc_cb));
}

void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb) {
    DebuggerCallback::SetReadMemoryCallback(std::move(readMemory_cb));
}

// TODO: I need to think about this callback more. The idea is to add the bank to a break/watch such as Bank 3 address 0x40C0 -> "3:0x40C0"
void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    return DebuggerCallback::SetCheckBankableMemoryLocationCallback(std::move(CheckBankableMemoryLocation_cb));
}

void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb) {
    DebuggerCallback::SetReadBankableMemoryCallback(std::move(readBankMemory_cb));
}


void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb) {
    DebuggerCallback::SetGetRegSetCallback(std::move(getRegSet_cb));
}

}