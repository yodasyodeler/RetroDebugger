#pragma once
#include <map>
#include <vector>
#include <string>

#include "DebuggerCommon.h"
#include "DebuggerCallbacks.h"

static const BreakNum MAX_BREAKPOINT_NUM = UINT_MAX;
static const unsigned int MAX_ADDRESS = UINT_MAX;

class DebuggerOperations;

class BreakpointManager {
    enum class DebugOperation {
        RunOp = 0,
        StepOp,
        FinishOp,
        WatchOp,
    };

public:
    BreakpointManager() = default;
    BreakpointManager(std::shared_ptr<DebuggerOperations> operations);

    bool CheckBreakpoints(BreakInfo& breakpointNum); //TODO: what args are needed

    bool Run(unsigned int numBreakpointsToSkip = 0);
    bool RunInstructions(unsigned int numInstructions = 0);
    bool RunTillJump();
    BreakNum SetBreakpoint(unsigned int address);
    BreakNum SetBreakpoint(BankNum bank, unsigned int address);
    BreakNum SetWatchpoint(unsigned int addressStart, unsigned int addressEnd);
    BreakNum SetWatchpoint(BankNum bank, unsigned int addressStart, unsigned int addressEnd);
    bool EnableBreakpoints(const std::vector<BreakNum>& list);
    bool DisableBreakpoints(const std::vector<BreakNum>& list);
    bool DeleteBreakpoints(const std::vector<BreakNum>& list = {});
    std::map<BreakNum, BreakInfo> GetBreakpointInfoList(const std::vector<BreakNum>& list = {});

private:
    bool ModifyBreak(std::vector<BreakNum> list, bool isEnabled);

    std::map<BreakNum, BreakInfo> m_breakpoints = {};
    std::shared_ptr<DebuggerOperations> m_operations;

    DebugOperation m_debugOp = DebugOperation::RunOp;
    unsigned int m_instructionsToStep = 0;

    BreakNum m_breakPointCounter = 1;
};