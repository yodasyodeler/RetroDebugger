#include "Debugger.h"

#include "DebuggerCallbacks.h"
#include "DebuggerOperations.h"

#include <algorithm>

namespace {
std::vector<BreakNum> ToBreakNumList(const std::vector<unsigned int>& list) {
    std::vector<BreakNum> breakpoints(list.size());
    std::ranges::transform(list, breakpoints.begin(), [](unsigned int breakpoint) { return BreakNum{ breakpoint }; });

    return breakpoints;
}
}

namespace Rdb {

Debugger::Debugger(std::shared_ptr<IDebuggerCallbacks> callbacks) :
    m_callbacks(std::move(callbacks)),
    m_operations(std::make_shared<DebuggerOperations>(m_callbacks)),
    m_breakManager(m_operations, m_callbacks) {}

bool Debugger::CheckBreakpoints(BreakInfo& breakInfo) {
    return m_breakManager.CheckBreakpoints(breakInfo);
}

bool Debugger::Run(const unsigned int numBreakpointsToSkip) {
    return m_breakManager.Run(numBreakpointsToSkip);
}

bool Debugger::RunInstructions(const unsigned int numInstructions) {
    return m_breakManager.RunInstructions(numInstructions);
}

bool Debugger::RunTillJump() {
    return m_breakManager.RunTillJump();
}

BreakNum Debugger::SetBreakpoint(const unsigned int address) {
    // TODO: should the address be checked?
    return m_breakManager.SetBreakpoint(address);
}

BreakNum Debugger::SetBreakpoint(BankNum bank, const unsigned int address) {
    // TODO: should the address be checked?
    return m_breakManager.SetBreakpoint(BankNum{ bank }, address);
}

void Debugger::SetCondition(BreakNum breakNum, const std::string& condition) {
    m_breakManager.SetCondition(breakNum, condition);
}

BreakNum Debugger::SetWatchpoint(const unsigned int address, BankNum bankNumber) {
    return m_breakManager.SetWatchpoint(address, bankNumber);
}

BreakNum Debugger::SetReadWatchpoint(unsigned int address, BankNum bankNumber) {
    return m_breakManager.SetReadWatchpoint(address, bankNumber);
}

BreakNum Debugger::SetAnyWatchpoint(unsigned int address, BankNum bankNumber) {
    return m_breakManager.SetAnyWatchpoint(address, bankNumber);
}

BreakNum Debugger::SetWatchpoint(const std::string& name) {
    return m_breakManager.SetWatchpoint(name);
}

// BreakNum Debugger::SetReadWatchpoint(const std::string& name) {
//     return m_breakManager.SetReadWatchpoint(name);
// }
//
// BreakNum Debugger::SetAnyWatchpoint(const std::string& name) {
//     return m_breakManager.SetAnyWatchpoint(name);
// }

bool Debugger::EnableBreakpoints(const std::vector<unsigned int>& list) {
    return m_breakManager.EnableBreakpoints(ToBreakNumList(list));
}

bool Debugger::DisableBreakpoints(const std::vector<unsigned int>& list) {
    return m_breakManager.DisableBreakpoints(ToBreakNumList(list));
}

bool Debugger::DeleteBreakpoints(const std::vector<unsigned int>& list) {
    return m_breakManager.DeleteBreakpoints(ToBreakNumList(list));
}

BreakList Debugger::GetBreakpointInfoList(const std::vector<unsigned int>& list) {
    return m_breakManager.GetBreakpointInfoList(ToBreakNumList(list));
}

// RegInfo Debugger::GetRegInfo(const int /*reg*/) { return {}; } // TODO: need to redo RegInfo

AddrInfo Debugger::GetRomInfo(const unsigned int address) {
    return { address, m_callbacks->ReadMemory(address) };
}

std::vector<RegisterInfoPtr> Debugger::GetRegisterInfoList() {
    return m_operations->GetRegisters();
}

void Debugger::ResetOperations() {
    m_operations->Reset();
}

void Debugger::SetOperations(const XmlOperationsMap& operations) {
    m_operations->SetOperations(operations);
}

void Debugger::ReadMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes) {
    m_breakManager.ReadMemoryHook(bankNum, address, bytes);
}

void Debugger::WriteMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes) {
    m_breakManager.WriteMemoryHook(bankNum, address, bytes);
}

CommandList Debugger::GetCommandInfoList(size_t address, const unsigned int numInstructions) {
    CommandList operations;
    for (auto i = 0U; i < numInstructions; ++i) {
        Operation operation;
        auto operationAddress = address;
        address += m_operations->GetOperation(address, operation);
        operations.emplace(operationAddress, operation);
    }
    return operations;
}

CommandList Debugger::GetCommandInfoList(size_t address, size_t endAddress) {
    CommandList operations;
    while (address <= endAddress) {
        Operation operation;
        auto operationAddress = address;
        address += m_operations->GetOperation(address, operation);
        operations.emplace(operationAddress, operation);
    }
    return operations;
}

}