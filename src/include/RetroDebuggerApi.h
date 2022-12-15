#pragma once

#include "RetroDebugger_export.h"

// TODO: Should these APIs support C language. Remove C++ types? Otherwise look at adding nodiscard and noexcept where makes sense.
// TODO: Research dll best practice, not sure if this should be exposed.
#include "RetroDebuggerCommon.h"

namespace Rdb {

/// @brief Gets the RetroDebugger version
/// Gets the RetroDebugger version.
/// Version is formatted as following "<MAJOR>.<MINOR>.<PATCH>".
/// @return RetroDebugger version string.
RDB_EXPORT [[nodiscard]] std::string GetRdbVersion() noexcept;

/// @brief Returns a string for RetroDebugger that can be used as a prompt.
/// @return A string of RetroDebugger command prompt.
RDB_EXPORT [[nodiscard]] std::string GetCommandPrompt() noexcept;

RDB_EXPORT [[nodiscard]] std::string GetCommandResponse();

// RDB_EXPORT size_t GetCommandResponseSize(); //TODO: when using string is this needed?

RDB_EXPORT int ProcessCommandString(const std::string& message);

// Direct calls
RDB_EXPORT bool CheckBreakpoints(BreakInfo* breakInfo);

RDB_EXPORT bool Run(const unsigned int numBreakpointsToSkip);

RDB_EXPORT bool RunInstructions(const unsigned int numBreakToPass);

RDB_EXPORT bool RunTillJump();

RDB_EXPORT bool SetBreakpoint(const int address);

RDB_EXPORT bool EnableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

RDB_EXPORT bool DisableBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

RDB_EXPORT bool DeleteBreakpoints(const unsigned int breakRange0, const unsigned int breakRange1);

// RDB_EXPORT RegInfo GetRegInfo(const int register);

RDB_EXPORT [[nodiscard]] BreakInfo GetBreakpointInfo(const unsigned int breakPointNum);

// RDB_EXPORT CommandInfo GetCommandInfo(const unsigned int location, unsigned int& instruction);

RDB_EXPORT bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo);

RDB_EXPORT bool ParseXmlFile(const std::string& filename); // TODO: Should this API be restructured to use exceptions.

// Callbacks
RDB_EXPORT void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb);

RDB_EXPORT void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb);

RDB_EXPORT void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);

RDB_EXPORT void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readMemory_cb);

RDB_EXPORT void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb);
}