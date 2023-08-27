#pragma once

#include "BreakpointManager.h"

class Debugger {
public:
    Debugger();
    bool CheckBreakpoints(BreakInfo& breakInfo);

    bool Run(unsigned int numBreakpointsToSkip = 0);
    bool RunInstructions(unsigned int numInstructions = 0);
    bool RunTillJump();

    BreakNum SetBreakpoint(unsigned int address);
    BreakNum SetBreakpoint(BankNum bank, unsigned int address);

    BreakNum SetWatchpoint(unsigned int addressStart, unsigned int addressEnd = std::numeric_limits<unsigned int>::max());
    // BreakNum SetWatchpoint(BankNum bank, unsigned int addressStart, unsigned int addressEnd = std::numeric_limits<unsigned int>::max());

    bool EnableBreakpoints(const std::vector<BreakNum>& list);
    bool DisableBreakpoints(const std::vector<BreakNum>& list);
    bool DeleteBreakpoints(const std::vector<BreakNum>& list = {});

    // RegInfo GetRegInfo(int reg);
    static AddrInfo GetRomInfo(unsigned int address);

    CommandList GetCommandInfoList(size_t address, unsigned int numInstructions);
    BreakList GetBreakpointInfoList(const std::vector<BreakNum>& list = {});
    std::vector<RegisterInfoPtr> GetRegisterInfoList();

    // bool ParseXmlFile(const std::string& filename);
    void ResetOperations();
    void SetOperations(const XmlOperationsMap& operations);

private:
    std::shared_ptr<DebuggerOperations> m_operations;
    BreakpointManager m_breakManager;
};