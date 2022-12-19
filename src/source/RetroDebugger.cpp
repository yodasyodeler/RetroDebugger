#include "RetroDebugger.h"

#include "DebuggerPrintFormat.h"
#include "DebuggerStringParser.h"

#include "RetroDebugger_config.h"

#include <iostream>
#include <numeric>

namespace Rdb {

std::string RetroDebugger::GetRdbVersion() {
    return std::string(::RetroDebugger::Config::ProjectVersion);
}

// Command interpreter calls
std::string RetroDebugger::GetCommandPrompt() noexcept {
    return m_console.GetPrompt();
}

std::string RetroDebugger::GetCommandResponse() const {
    return m_console.GetResponse();
}

int RetroDebugger::ProcessCommandString(const std::string& message) {
    return (m_console.AdvanceDebugger(message)) ? 1 : 0; // TODO: move to enum
}

// Direct debugger calls
bool RetroDebugger::CheckBreakpoints(BreakInfo* breakInfo) {
    BreakInfo info = {};
    BreakInfo& infoRef = (breakInfo == nullptr) ? info : *breakInfo;

    const auto hitBreakpoint = m_debugger.CheckBreakpoints(infoRef);
    if (hitBreakpoint) {
        if (infoRef.type == BreakType::Breakpoint || infoRef.type == BreakType::BankBreakpoint) {
            m_interpreter.SetCommandResponse(DebuggerPrintFormat::PrintBreakpointHit(infoRef));
        }
        else if (infoRef.type == BreakType::Watchpoint) {
            m_interpreter.SetCommandResponse(DebuggerPrintFormat::PrintWatchpointHit(infoRef));
        }
    }
    return hitBreakpoint;
}

bool RetroDebugger::Run(const unsigned int numBreakpointsToSkip) {
    return m_debugger.Run(numBreakpointsToSkip);
}

bool RetroDebugger::RunInstructions(const unsigned int numBreakToPass) {
    return m_debugger.RunInstructions(numBreakToPass);
}

bool RetroDebugger::RunTillJump() {
    return m_debugger.RunTillJump();
}

bool RetroDebugger::SetBreakpoint(unsigned int address) {
    m_debugger.SetBreakpoint(address);
    return true; // TODO: this doesn't mean anything
}

bool RetroDebugger::EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }

    std::vector<unsigned int> breakRange(static_cast<size_t>(breakRange1 - breakRange0) + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return m_debugger.EnableBreakpoints(breakRange);
}

bool RetroDebugger::DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }

    std::vector<unsigned int> breakRange(static_cast<size_t>(breakRange1 - breakRange0) + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return m_debugger.DisableBreakpoints(breakRange);
}

bool RetroDebugger::DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }
    std::vector<unsigned int> breakRange(static_cast<size_t>(breakRange1 - breakRange0) + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return m_debugger.DeleteBreakpoints(breakRange);
}

BreakInfo RetroDebugger::GetBreakpointInfo(const unsigned int breakPointNum) {
    auto breakInfo = m_debugger.GetBreakpointInfoList({ breakPointNum });

    static constexpr auto maxUInt = std::numeric_limits<unsigned int>::max();
    const BreakInfo invalidBreakpoint = { maxUInt, maxUInt, maxUInt, maxUInt, 0, 0, BreakType::Invalid, BreakDisposition::Disable, false };
    return breakInfo.find(breakPointNum) != breakInfo.end() ? breakInfo.at(breakPointNum) : invalidBreakpoint;
}

bool RetroDebugger::GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo) {
    if (registerInfo != nullptr) {
        *registerInfo = m_debugger.GetRegisterInfoList();
        return true;
    }

    return false;
}

bool RetroDebugger::ParseXmlFile(const std::string& filename) {
    return m_debugger.ParseXmlFile(filename);
}

// Set Callbacks
void RetroDebugger::SetGetPcRegCallback(GetProgramCounterFunc getPc_cb) {
    DebuggerCallback::SetGetPcRegCallback(std::move(getPc_cb));
}

void RetroDebugger::SetReadMemoryCallback(ReadMemoryFunc readMemory_cb) {
    DebuggerCallback::SetReadMemoryCallback(std::move(readMemory_cb));
}

// TODO: I need to think about this callback more. The idea is to add the bank to a break/watch such as Bank 3 address 0x40C0 -> "3:0x40C0"
void RetroDebugger::SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    return DebuggerCallback::SetCheckBankableMemoryLocationCallback(std::move(CheckBankableMemoryLocation_cb));
}

void RetroDebugger::SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb) {
    DebuggerCallback::SetReadBankableMemoryCallback(std::move(readBankMemory_cb));
}


void RetroDebugger::SetGetRegSetCallback(GetRegSetFunc getRegSet_cb) {
    DebuggerCallback::SetGetRegSetCallback(std::move(getRegSet_cb));
}

}