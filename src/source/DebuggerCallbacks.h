#pragma once

#include "RetroDebuggerApi.h"

namespace DebuggerCallback {
unsigned int GetPcReg();
unsigned int ReadMemory(unsigned int address);
bool CheckBankableMemoryLocation(BankNum bank, unsigned int address);
unsigned int ReadBankableMemory(BankNum bank, unsigned int address);
RegSet GetRegSet();

// Set Callbacks
void SetGetPcRegCallback(GetProgramCounterFunc getPc_cb);
void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb);
void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);
void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankableMemory_cb);
void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb);
}