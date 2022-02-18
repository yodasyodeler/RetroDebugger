#pragma once

#ifdef DEBUGGER_EXPORTS
#define DBG_API __declspec(dllexport)
#else
#define DBG_API __declspec(dllimport)
#endif

//TODO: Research dll best practice, not sure if this should be exposed.
#include "DebuggerCommon.h"
#include <functional>

typedef std::function<unsigned int()> GetProgramCounterFunc;

typedef std::function<unsigned int(unsigned int)> ReadMemoryFunc;

typedef std::function<unsigned int(BankNum bank, unsigned int)> ReadBankableMemoryFunc;

typedef std::function<bool(BankNum bank, unsigned int address)> CheckBankableMemoryLocationFunc;

typedef std::function<RegSet()> GetRegSetFunc;

#ifdef __cplusplus
extern "C" {
#endif
    DBG_API int GetCommandPrompt(std::string* message);

    DBG_API int GetCommandResponse(std::string* message);

    DBG_API size_t GetCommandResponseSize();

    DBG_API int ProcessCommandString(std::string* message);

    //Direct calls
    DBG_API bool CheckBreakpoints(BreakInfo* breakInfo);

    DBG_API bool Run(const unsigned int numBreakpointsToSkip);

    DBG_API bool RunInstructions(const int numBreakToPass);

    DBG_API bool RunTillJump();

    DBG_API bool SetBreakpoint(const int address);

    DBG_API bool EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

    DBG_API bool DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

    DBG_API bool DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

    //DBG_API RegInfo GetRegInfo(const int register);

    DBG_API BreakInfo GetBreakpointInfo(const unsigned int breakPointNum);

    //DBG_API CommandInfo GetCommandInfo(const unsigned int location, unsigned int& instruction);

    DBG_API bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo);

    DBG_API bool ParseXmlFile(const std::string& filename);

    //Callbacks
    DBG_API void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb);

    DBG_API void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb);

    DBG_API void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);

    DBG_API void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readMemory_cb);

    DBG_API void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb);
#ifdef __cplusplus
}
#endif