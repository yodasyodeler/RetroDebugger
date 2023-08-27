#include "Debugger.h"

#include "DebuggerOperations.h"

Debugger::Debugger() :
    m_operations(std::make_shared<DebuggerOperations>()), m_breakManager(m_operations) {}

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

BreakNum Debugger::SetBreakpoint(const BankNum bank, const unsigned int address) {
    // TODO: should the address be checked?
    return m_breakManager.SetBreakpoint(bank, address);
}

BreakNum Debugger::SetWatchpoint(const unsigned int addressStart, const unsigned int addressEnd) {
    return m_breakManager.SetWatchpoint(addressStart, addressEnd);
}

// BreakNum Debugger::SetWatchpoint(const BankNum bank, const unsigned int addressStart, const unsigned int addressEnd) {
//     return m_breakManager.SetWatchpoint(bank, addressStart, addressEnd);
// }

bool Debugger::EnableBreakpoints(const std::vector<BreakNum>& list) {
    return m_breakManager.EnableBreakpoints(list);
}

bool Debugger::DisableBreakpoints(const std::vector<BreakNum>& list) {
    return m_breakManager.DisableBreakpoints(list);
}

bool Debugger::DeleteBreakpoints(const std::vector<BreakNum>& list) {
    return m_breakManager.DeleteBreakpoints(list);
}

BreakList Debugger::GetBreakpointInfoList(const std::vector<unsigned int>& list) {
    return m_breakManager.GetBreakpointInfoList(list);
}

// RegInfo Debugger::GetRegInfo(const int /*reg*/) { return {}; } // TODO: need to redo RegInfo

AddrInfo Debugger::GetRomInfo(const unsigned int address) { return { address, DebuggerCallback::ReadMemory(address) }; }

std::vector<RegisterInfoPtr> Debugger::GetRegisterInfoList() {
    return m_operations->GetRegisters();
}

void Debugger::ResetOperations() {
    m_operations->Reset();
}

void Debugger::SetOperations(const XmlOperationsMap& operations) {
    m_operations->SetOperations(operations);
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
