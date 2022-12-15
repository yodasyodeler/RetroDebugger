#pragma once

#include "RetroDebuggerCommon.h"

namespace DebuggerCallback {
unsigned int GetPcReg();
unsigned int ReadMemory(unsigned int address);
bool CheckBankableMemoryLocation(BankNum bank, unsigned int address);
unsigned int ReadBankableMemory(BankNum bank, unsigned int address);
RegSet GetRegSet();

// Set Callbacks
void SetGetPcRegCallback(Rdb::GetProgramCounterFunc getPc_cb);
void SetReadMemoryCallback(Rdb::ReadMemoryFunc readMemory_cb);
void SetCheckBankableMemoryLocationCallback(Rdb::CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);
void SetReadBankableMemoryCallback(Rdb::ReadBankableMemoryFunc readBankableMemory_cb);
void SetGetRegSetCallback(Rdb::GetRegSetFunc getRegSet_cb);
}