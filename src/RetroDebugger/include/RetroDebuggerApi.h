#pragma once

#include "RetroDebugger_export.h"

// TODO: Should these APIs support C language. Remove C++ types? Otherwise look at adding nodiscard and noexcept where makes sense.
// TODO: Research dll best practice, not sure if this should be exposed.
#include "RetroDebuggerCallbackDefines.h"

namespace Rdb {

/// @brief Gets the RetroDebugger version
/// Gets the RetroDebugger version.
/// Version is formatted as following "<MAJOR>.<MINOR>.<PATCH>".
/// @return RetroDebugger version string.
RDB_EXPORT std::string GetRdbVersion [[nodiscard]] ();

/// @brief Returns a string for RetroDebugger that can be used as a prompt.
/// @return A string of RetroDebugger command prompt.
RDB_EXPORT std::string GetCommandPrompt [[nodiscard]] () noexcept;

RDB_EXPORT std::string GetCommandResponse [[nodiscard]] ();

RDB_EXPORT int ProcessCommandString(const std::string& message);

// Direct calls
RDB_EXPORT bool CheckBreakpoints(BreakInfo* breakInfo);

RDB_EXPORT bool Run(unsigned int numBreakpointsToSkip);

RDB_EXPORT bool RunInstructions(unsigned int numBreakToPass);

RDB_EXPORT bool RunTillJump();

RDB_EXPORT bool SetBreakpoint(unsigned int address);

RDB_EXPORT void SetCondition(unsigned int breakNum, const std::string& condition);

RDB_EXPORT bool SetWatchpoint(unsigned int address);

RDB_EXPORT bool SetReadWatchpoint(unsigned int address);

RDB_EXPORT bool SetAnyWatchpoint(unsigned int address);

RDB_EXPORT bool EnableBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

RDB_EXPORT bool DisableBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

RDB_EXPORT bool DeleteBreakpoints();

RDB_EXPORT bool DeleteBreakpoints(unsigned int breakRange0, unsigned int breakRange1);

// RDB_EXPORT RegInfo GetRegInfo(int register);

RDB_EXPORT BreakList GetBreakpointInfo [[nodiscard]] ();

RDB_EXPORT BreakInfo GetBreakpointInfo [[nodiscard]] (unsigned int breakPointNum);

// RDB_EXPORT CommandInfo GetCommandInfo(unsigned int location, unsigned int& instruction);

RDB_EXPORT bool GetRegisterInfo(std::vector<RegisterInfoPtr>* registerInfo);

RDB_EXPORT void ParseXmlFile(const std::string& filename); // TODO: Should this API be restructured to use exceptions.

// Callbacks
RDB_EXPORT void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb);

RDB_EXPORT void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb);

RDB_EXPORT void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);

RDB_EXPORT void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankMemory_cb);

RDB_EXPORT void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb);

// Hooks
RDB_EXPORT void ReadMemoryHook(unsigned int address, const std::vector<std::byte>& bytes);
RDB_EXPORT void WriteMemoryHook(unsigned int address, const std::vector<std::byte>& bytes);

RDB_EXPORT void ReadMemoryHook(unsigned int bankNum, unsigned int address, const std::vector<std::byte>& bytes);
RDB_EXPORT void WriteMemoryHook(unsigned int bankNum, unsigned int address, const std::vector<std::byte>& bytes);
}