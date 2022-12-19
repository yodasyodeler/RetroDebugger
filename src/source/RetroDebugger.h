#include "RetroDebuggerCommon.h"

#include "Debugger.h"
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

    bool EnableBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

    bool DisableBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

    bool DeleteBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

    [[nodiscard]] BreakInfo GetBreakpointInfo(unsigned int breakPointNum);

    bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo);

    bool ParseXmlFile(const std::string& filename);

    // Callbacks
    static void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb);

    static void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb);

    static void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);

    static void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb);

    static void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb);

private:
    Debugger m_debugger;
    DebuggerInterpreter m_interpreter{ &m_debugger };
    DebuggerConsole m_console{ &m_interpreter };
};

}