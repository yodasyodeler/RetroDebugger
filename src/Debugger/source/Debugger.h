#pragma once

#include "BreakpointManager.h"

namespace Rdb {

class Debugger {
public:
    Debugger(std::shared_ptr<IDebuggerCallbacks> callbacks);
    bool CheckBreakpoints(BreakInfo& breakInfo);

    bool Run(unsigned int numBreakpointsToSkip = 0);
    bool RunInstructions(unsigned int numInstructions = 0);
    bool RunTillJump();

    BreakNum SetBreakpoint(unsigned int address);
    BreakNum SetBreakpoint(BankNum bank, unsigned int address);

    void SetCondition(BreakNum breakNum, const std::string& condition);

    BreakNum SetWatchpoint(unsigned int address, BankNum bankNumber = AnyBank);
    BreakNum SetReadWatchpoint(unsigned int address, BankNum bankNumber = AnyBank);
    BreakNum SetAnyWatchpoint(unsigned int address, BankNum bankNumber = AnyBank);

    bool EnableBreakpoints(const std::vector<unsigned int>& list);
    bool DisableBreakpoints(const std::vector<unsigned int>& list);
    bool DeleteBreakpoints(const std::vector<unsigned int>& list = {});

    // RegInfo GetRegInfo(int reg);
    AddrInfo GetRomInfo(unsigned int address);

    CommandList GetCommandInfoList(size_t address, unsigned int numInstructions);
    CommandList GetCommandInfoList(size_t address, size_t endAddress); // TODO: May make sense to use a strongly typed Address type.
    BreakList GetBreakpointInfoList(const std::vector<unsigned int>& list = {});
    std::vector<RegisterInfoPtr> GetRegisterInfoList();

    // bool ParseXmlFile(const std::string& filename);
    void ResetOperations();
    void SetOperations(const XmlOperationsMap& operations);

    // Hooks
    void ReadMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes);
    void WriteMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes);

private:
    std::shared_ptr<IDebuggerCallbacks> m_callbacks;
    std::shared_ptr<DebuggerOperations> m_operations;
    BreakpointManager m_breakManager;
};

}