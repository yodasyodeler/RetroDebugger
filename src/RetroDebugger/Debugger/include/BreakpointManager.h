#pragma once
#include "DebuggerCommon.h"

#include <limits>
#include <map>
#include <string>
#include <vector>

static constexpr BreakNum MaxBreakpointNumber = BreakNum{ std::numeric_limits<unsigned int>::max() };
static constexpr unsigned int MaxAddress = std::numeric_limits<unsigned int>::max();

class DebuggerOperations;

class BreakpointManager {
    enum class DebugOperation {
        RunOp = 0,
        StepOp,
        FinishOp,
    };

public:
    BreakpointManager() = default;
    explicit BreakpointManager(std::shared_ptr<DebuggerOperations> operations);

    bool CheckBreakpoints(BreakInfo& breakInfo);

    bool Run(unsigned int numBreakpointsToSkip = 0);
    bool RunInstructions(unsigned int numInstructions = 0);
    bool RunTillJump();
    BreakNum SetBreakpoint(unsigned int address);
    BreakNum SetBreakpoint(BankNum bank, unsigned int address);

    BreakNum SetWatchpoint(unsigned int address, BankNum bank = AnyBank);
    BreakNum SetReadWatchpoint(unsigned int address, BankNum bank = AnyBank);
    BreakNum SetAnyWatchpoint(unsigned int address, BankNum bank = AnyBank);

    // BreakNum SetWatchpoint(const std::string& registerName);
    // BreakNum SetReadWatchpoint(const std::string& registerName);
    // BreakNum SetAnyWatchpoint(const std::string& registerName);

    bool EnableBreakpoints(const std::vector<BreakNum>& list);
    bool DisableBreakpoints(const std::vector<BreakNum>& list);
    bool DeleteBreakpoints(const std::vector<BreakNum>& list = {});
    BreakList GetBreakpointInfoList(const std::vector<BreakNum>& list = {});

    // Hooks
    void ReadMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes);
    void WriteMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes);

private:
    BreakInfo CheckBreakInfo();
    bool HandleBreakInfo(const BreakInfo& info);
    bool ModifyBreak(const std::vector<BreakNum>& list, bool isEnabled);

    std::map<BreakNum, BreakInfo> m_breakpoints = {};
    std::shared_ptr<DebuggerOperations> m_operations;

    DebugOperation m_debugOp = DebugOperation::RunOp;
    unsigned int m_instructionsToStep = 0;

    BreakNum m_breakPointCounter = BreakNum{ 1 };
};