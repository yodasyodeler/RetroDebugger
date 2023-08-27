#include "RetroDebuggerApi.h"

#include "RetroDebugger.h"

namespace Rdb {

RetroDebugger m_debugger; // NOLINT (cert-err58-cpp, cppcoreguidelines-avoid-non-const-global-variables) - TODO: Look into this more. Issue is that it can throw and can't be caught.

std::string GetRdbVersion() {
    return m_debugger.GetRdbVersion();
}

// Command interpreter calls
std::string GetCommandPrompt() noexcept {
    return m_debugger.GetCommandPrompt();
}

std::string GetCommandResponse() {
    return m_debugger.GetCommandResponse();
}

int ProcessCommandString(const std::string& message) {
    return m_debugger.ProcessCommandString(message);
}

// Direct debugger calls
bool CheckBreakpoints(BreakInfo* breakInfo) {
    return m_debugger.CheckBreakpoints(breakInfo);
}

bool Run(const unsigned int numBreakpointsToSkip) {
    return m_debugger.Run(numBreakpointsToSkip);
}

bool RunInstructions(const unsigned int numBreakToPass) {
    return m_debugger.RunInstructions(numBreakToPass);
}

bool RunTillJump() {
    return m_debugger.RunTillJump();
}

bool SetBreakpoint(unsigned int address) {
    return m_debugger.SetBreakpoint(address);
}

bool EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    return m_debugger.EnableBreakpoints(breakRange0, breakRange1);
}

bool DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    return m_debugger.DisableBreakpoints(breakRange0, breakRange1);
}

bool DeleteBreakpoints() {
    return m_debugger.DeleteBreakpoints();
}

bool DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    return m_debugger.DeleteBreakpoints(breakRange0, breakRange1);
}

BreakList GetBreakpointInfo [[nodiscard]] () {
    return m_debugger.GetBreakpointInfo();
}


BreakInfo GetBreakpointInfo(const unsigned int breakPointNum) {
    return m_debugger.GetBreakpointInfo(breakPointNum);
}

bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo) {
    return m_debugger.GetRegisterInfo(registerInfo);
}

void ParseXmlFile(const std::string& filename) {
    m_debugger.ParseXmlFile(filename);
}

// Set Callbacks
void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb) {
    m_debugger.SetGetPcRegCallback(std::move(getPc_cb));
}

void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb) {
    m_debugger.SetReadMemoryCallback(std::move(readMemory_cb));
}

// TODO: I need to think about this callback more. The idea is to add the bank to a break/watch such as Bank 3 address 0x40C0 -> "3:0x40C0"
void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    m_debugger.SetCheckBankableMemoryLocationCallback(std::move(CheckBankableMemoryLocation_cb));
}

void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb) {
    m_debugger.SetReadBankableMemoryCallback(std::move(readBankMemory_cb));
}


void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb) {
    m_debugger.SetGetRegSetCallback(std::move(getRegSet_cb));
}

}