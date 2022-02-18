#pragma once

#include <map>
#include <vector>
#include <string>

#include "DebuggerCommon.h"

namespace DebuggerPrintFormat {
//Help print
std::string PrintGeneralHelp();

std::string PrintCommandHelp(const std::string& command);
std::string PrintRegHelp();
std::string PrintPpuHelp();
std::string PrintApuHelp();
std::string PrintTimerHelp();

//Command Help
std::string PrintHelpHelp();

//Breakpoint print
std::string PrintBreakpointHit(BreakInfo breakInfo);
std::string PrintWatchpointHit(BreakInfo breakInfo);

//Info print
std::string PrintBreakInfo(const std::map<BreakNum, BreakInfo>& breakInfo);
std::string PrintLineInfo(unsigned int line);
std::string PrintAllRegisters(const RegSet& regset);
std::string PrintRegister(const std::string& name, unsigned int value);
std::string PrintMemoryMappedRegInfo(const RegInfo& info);
std::string PrintAddressInfo(const AddrInfo& info);

//Gameboy Instruction print
std::string PrintInstructions(const std::map<unsigned int, Operation>& commandInfo);

//Set Variable print
std::string PrintListsize(unsigned int listsize);
}