#pragma once

#include "RetroDebugger_export.h"

// TODO: Should these APIs support C language. Remove C++ types? Otherwise look at adding nodiscard and noexcept where makes sense.
// TODO: Research dll best practice, not sure if this should be exposed.
#include "DebuggerCommon.h"
#include <functional>

typedef std::function<unsigned int()> GetProgramCounterFunc;

typedef std::function<unsigned int(unsigned int)> ReadMemoryFunc;

typedef std::function<unsigned int(BankNum bank, unsigned int)> ReadBankableMemoryFunc;

typedef std::function<bool(BankNum bank, unsigned int address)> CheckBankableMemoryLocationFunc;

typedef std::function<RegSet()> GetRegSetFunc;

// TODO: If using C style API add prefix. Otherwise add namespace to avoid name collisions
#ifdef __cplusplus
extern "C" {
#endif
RDB_EXPORT size_t GetRdbVersion(std::string* version);

RDB_EXPORT size_t GetCommandPrompt(std::string* message);

RDB_EXPORT size_t GetCommandResponse(std::string* message);

RDB_EXPORT size_t GetCommandResponseSize();

RDB_EXPORT int ProcessCommandString(std::string* message);

// Direct calls
RDB_EXPORT bool CheckBreakpoints(BreakInfo* breakInfo);

RDB_EXPORT bool Run(const unsigned int numBreakpointsToSkip);

RDB_EXPORT bool RunInstructions(const int numBreakToPass);

RDB_EXPORT bool RunTillJump();

RDB_EXPORT bool SetBreakpoint(const int address);

RDB_EXPORT bool EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

RDB_EXPORT bool DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

RDB_EXPORT bool DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

// RDB_EXPORT RegInfo GetRegInfo(const int register);

RDB_EXPORT BreakInfo GetBreakpointInfo(const unsigned int breakPointNum);

// RDB_EXPORT CommandInfo GetCommandInfo(const unsigned int location, unsigned int& instruction);

RDB_EXPORT bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo);

RDB_EXPORT bool ParseXmlFile(const std::string& filename);

// Callbacks
RDB_EXPORT void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb);

RDB_EXPORT void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb);

RDB_EXPORT void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);

RDB_EXPORT void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readMemory_cb);

RDB_EXPORT void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb);
#ifdef __cplusplus
}
#endif