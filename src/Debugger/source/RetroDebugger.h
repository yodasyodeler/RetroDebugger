#include "Debugger.h"
#include "DebuggerCallbacks.h"
#include "DebuggerConsole.h"
#include "DebuggerInterpreter.h"

namespace Rdb {

class RetroDebugger {
public:
    [[nodiscard]] static std::string GetRdbVersion();

    [[nodiscard]] std::string GetCommandPrompt() noexcept;

    [[nodiscard]] std::string GetCommandResponse() const;

    int ProcessCommandString(const std::string& message);

    bool CheckBreakpoints(BreakInfo* breakInfo);

    bool Run(unsigned int numBreakpointsToSkip);

    bool RunInstructions(unsigned int numBreakToPass);

    bool RunTillJump();

    bool SetBreakpoint(unsigned int address);

    bool SetWatchpoint(unsigned int address);

    bool SetReadWatchpoint(unsigned int address);

    bool SetAnyWatchpoint(unsigned int address);

    bool EnableBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

    bool DisableBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

    bool DeleteBreakpoints();

    bool DeleteBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

    [[nodiscard]] BreakList GetBreakpointInfo();

    [[nodiscard]] BreakInfo GetBreakpointInfo(unsigned int breakPointNum);

    bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo);

    void ParseXmlFile(const std::string& filename);

    // Callbacks
    void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb);

    void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb);

    void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);

    void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb);

    void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb);

    // Hooks
    void ReadMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes);

    void WriteMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes);

private:
    std::shared_ptr<DebuggerCallbacks> m_callbacks = std::make_shared<DebuggerCallbacks>();
    Debugger m_debugger{ m_callbacks };
    DebuggerInterpreter m_interpreter{ &m_debugger, m_callbacks };
    DebuggerConsole m_console{ &m_interpreter };
};

}