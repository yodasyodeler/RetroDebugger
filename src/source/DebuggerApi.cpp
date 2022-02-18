#include "DebuggerApi.h"
#include "DebuggerInterpreter.h"
#include "DebuggerStringParser.h"
#include "DebuggerPrintFormat.h"
#include "DebuggerConsole.h"

#include <numeric>
#include <iostream>

static const char* GdbPrompt = "(gdb)";
static std::vector<std::string> m_prevWords;

//static BreakpointManager g_interpreter;
static Debugger g_debugger;
static DebuggerInterpreter g_interpreter(&g_debugger);
static DebuggerConsole g_console(&g_interpreter);

//Command interpreter calls
int GetCommandPrompt(std::string* message) {
    if (message == nullptr) return -1; //TODO: move to enum
    *message = g_console.GetPrompt();

    return message->length();
}

int GetCommandResponse(std::string* message) {
    if (message == nullptr) return -1; //TODO: move to enum
    *message = g_console.GetResponse();

    return message->length();
}

size_t GetCommandResponseSize() {
    return g_console.GetResponseLength();
}

int ProcessCommandString(std::string* message) {
    if (message == nullptr) return -1; //TODO: move to enum
    return (g_console.AdvanceDebugger(*message)) ? 1 : 0; //TODO: move to enum
}

//Direct debugger calls
bool CheckBreakpoints(BreakInfo* breakInfo) {
    BreakInfo info = {};
    BreakInfo& infoRef = (!breakInfo) ? info : *breakInfo;

    const auto re = g_debugger.CheckBreakpoints(infoRef);
    if (re) {
        if (infoRef.type == BreakType::Breakpoint || infoRef.type == BreakType::BankBreakpoint) {
            g_interpreter.SetCommandResponse(DebuggerPrintFormat::PrintBreakpointHit(infoRef));
        }
        else if (infoRef.type == BreakType::Watchpoint) {
            g_interpreter.SetCommandResponse(DebuggerPrintFormat::PrintWatchpointHit(infoRef));
        }
    }
    return re;
}

bool Run(const unsigned int numBreakpointsToSkip) {
    return g_debugger.Run(numBreakpointsToSkip);
}

bool RunInstructions(const int numBreakToPass) {
    return g_debugger.RunInstructions(numBreakToPass);
}

bool RunTillJump() {
    return g_debugger.RunTillJump();
}

bool SetBreakpoint(const int address) {
    return g_debugger.SetBreakpoint(address);
}

bool EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1)
        return false;
    std::vector<unsigned int> breakRange(breakRange1 - breakRange0 + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return g_debugger.EnableBreakpoints(breakRange);
}

bool DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1)
        return false;
    std::vector<unsigned int> breakRange(breakRange1 - breakRange0 + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return g_debugger.DisableBreakpoints(breakRange);
}

bool DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1) {
    if (breakRange0 > breakRange1)
        return false;
    std::vector<unsigned int> breakRange(breakRange1 - breakRange0 + 1);
    std::iota(breakRange.begin(), breakRange.end(), breakRange0);

    return g_debugger.DeleteBreakpoints(breakRange);
}

BreakInfo GetBreakpointInfo(const unsigned int breakPointNum) {
    auto breakInfo = g_debugger.GetBreakpointInfoList({breakPointNum});

    BreakInfo invalidBreakpoint = {static_cast<unsigned int>(-1), static_cast<unsigned int>(-1), static_cast<BreakNum>(-1), static_cast<unsigned int>(-1), 0, 0, BreakType::Invalid, BreakDisposition::Disable, false};
    return breakInfo.find(breakPointNum) != breakInfo.end() ? breakInfo.at(breakPointNum) : invalidBreakpoint;
}

bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo) {
    if (registerInfo) {
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
    DebuggerCallback::SetGetPcRegCallback(getPc_cb);
}

void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb) {
    DebuggerCallback::SetReadMemoryCallback(readMemory_cb);
}

//TODO: I need to think about this callback more. The idea is to add the bank to a break/watch such as Bank 3 address 0x40C0 -> "3:0x40C0"
void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    return DebuggerCallback::SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocation_cb);
}

void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb) {
    DebuggerCallback::SetReadBankableMemoryCallback(readBankMemory_cb);
}


void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb) {
    DebuggerCallback::SetGetRegSetCallback(getRegSet_cb);
}