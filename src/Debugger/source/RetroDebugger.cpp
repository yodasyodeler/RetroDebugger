#include "RetroDebugger.h"

#include "DebuggerCallbacks.h"
#include "DebuggerPrintFormat.h"
#include "DebuggerStringParser.h"
#include "DebuggerXmlParser.h"

#include "BuildInfo.h"

#include <fmt/core.h>

#include <iostream>
#include <numeric>

namespace {
template<typename DataType>
std::vector<DataType> CreateRange(DataType rangeStart, DataType rangeEnd) {
    std::vector<unsigned int> range(static_cast<size_t>(rangeEnd - rangeStart) + 1);
    std::iota(range.begin(), range.end(), rangeStart);

    return range;
}
}

namespace Rdb {

std::string RetroDebugger::GetRdbVersion() {
    return std::string(::RetroDebugger::Version);
}

// Command interpreter calls
std::string RetroDebugger::GetCommandPrompt() noexcept {
    return m_console.GetPrompt();
}

std::string RetroDebugger::GetCommandResponse() const {
    return m_console.GetCommandResponse();
}

int RetroDebugger::ProcessCommandString(const std::string& message) {
    return (m_console.AdvanceDebugger(message)) ? 1 : 0; // TODO: move to enum, (1: leave debugger, 0: continue looping on input)
}

// Direct debugger calls
bool RetroDebugger::CheckBreakpoints(BreakInfo* breakInfo) {
    BreakInfo info = {};
    BreakInfo& infoRef = (breakInfo == nullptr) ? info : *breakInfo;

    const auto hitBreakpoint = m_debugger->CheckBreakpoints(infoRef);
    if (hitBreakpoint) {
        if (infoRef.type == BreakType::Breakpoint) {
            m_console.SetCommandResponse(DebuggerPrintFormat::PrintBreakpointHit(infoRef));
        }
        else if (infoRef.type == BreakType::Watchpoint) {
            m_console.SetCommandResponse(DebuggerPrintFormat::PrintWatchpointHit(infoRef));
        }
    }
    return hitBreakpoint;
}

bool RetroDebugger::Run(const unsigned int numBreakpointsToSkip) {
    return m_debugger->Run(numBreakpointsToSkip);
}

bool RetroDebugger::RunInstructions(const unsigned int numBreakToPass) {
    return m_debugger->RunInstructions(numBreakToPass);
}

bool RetroDebugger::RunTillJump() {
    return m_debugger->RunTillJump();
}

bool RetroDebugger::SetBreakpoint(unsigned int address) {
    return m_debugger->SetBreakpoint(address) != std::numeric_limits<BreakNum>::max();
}

void RetroDebugger::SetCondition(BreakNum breakNum, const std::string& condition) {
    m_debugger->SetCondition(breakNum, condition);
}

bool RetroDebugger::SetWatchpoint(unsigned int address) {
    return m_debugger->SetWatchpoint(address) != std::numeric_limits<BreakNum>::max();
}

bool RetroDebugger::SetReadWatchpoint(unsigned int address) {
    return m_debugger->SetReadWatchpoint(address) != std::numeric_limits<BreakNum>::max();
}

bool RetroDebugger::SetAnyWatchpoint(unsigned int address) {
    return m_debugger->SetAnyWatchpoint(address) != std::numeric_limits<BreakNum>::max();
}

bool RetroDebugger::SetWatchpoint(const std::string& name) {
    return m_debugger->SetWatchpoint(name) != std::numeric_limits<BreakNum>::max();
}

bool RetroDebugger::EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }

    return m_debugger->EnableBreakpoints(CreateRange(breakRange0, breakRange1));
}

bool RetroDebugger::DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }

    return m_debugger->DisableBreakpoints(CreateRange(breakRange0, breakRange1));
}

bool RetroDebugger::DeleteBreakpoints() {
    return m_debugger->DeleteBreakpoints({});
}

bool RetroDebugger::DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1) { return false; }

    return m_debugger->DeleteBreakpoints(CreateRange(breakRange0, breakRange1));
}

BreakList RetroDebugger::GetBreakpointInfo() {
    return m_debugger->GetBreakpointInfoList({});
}

BreakInfo RetroDebugger::GetBreakpointInfo(const unsigned int breakPointNum) {
    auto breakInfo = m_debugger->GetBreakpointInfoList({ breakPointNum });

    static constexpr auto maxUInt = std::numeric_limits<unsigned int>::max();
    const BreakInfo invalidBreakpoint = { maxUInt, BreakNum{ maxUInt }, BankNum{ maxUInt }, maxUInt, 0, 0, BreakType::Invalid, BreakDisposition::Disable, false };
    return breakInfo.find(BreakNum{ breakPointNum }) != breakInfo.end() ? breakInfo.at(BreakNum{ breakPointNum }) : invalidBreakpoint;
}

bool RetroDebugger::GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo) {
    if (registerInfo != nullptr) {
        *registerInfo = m_debugger->GetRegisterInfoList();
        return true;
    }

    return false;
}

void RetroDebugger::ParseXmlFile(const std::string& filename) {
    m_debugger->ResetOperations();
    DebuggerXmlParser xmlParser;
    xmlParser.ParseFile(filename);
    m_debugger->SetOperations(xmlParser.GetOperations());
}

// Set Callbacks
void RetroDebugger::SetGetPcRegCallback(GetProgramCounterFunc getPc_cb) {
    m_callbacks->SetGetPcRegCallback(std::move(getPc_cb));
}

void RetroDebugger::SetReadMemoryCallback(ReadMemoryFunc readMemory_cb) {
    m_callbacks->SetReadMemoryCallback(std::move(readMemory_cb));
}

// TODO: I need to think about this callback more. The idea is to add the bank to a break/watch such as Bank 3 address 0x40C0 -> "3:0x40C0"
void RetroDebugger::SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    m_callbacks->SetCheckBankableMemoryLocationCallback(std::move(CheckBankableMemoryLocation_cb));
}

void RetroDebugger::SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb) {
    m_callbacks->SetReadBankableMemoryCallback(std::move(readBankMemory_cb));
}


void RetroDebugger::SetGetRegSetCallback(GetRegSetFunc getRegSet_cb) {
    m_callbacks->SetGetRegSetCallback(std::move(getRegSet_cb));
}

void RetroDebugger::ReadMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes) {
    m_debugger->ReadMemoryHook(bankNum, address, bytes);
}

void RetroDebugger::WriteMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes) {
    m_debugger->ReadMemoryHook(bankNum, address, bytes);
}

}