#pragma once

#include "RetroDebuggerCallbackDefines.h"

// TODO: Probably should just make this non-static methods
class DebuggerCallback {
public:
    static unsigned int GetPcReg();
    static unsigned int ReadMemory(unsigned int address);
    static bool CheckBankableMemoryLocation(BankNum bank, unsigned int address);
    static unsigned int ReadBankableMemory(BankNum bank, unsigned int address);
    static RegSet GetRegSet();

    // Set Callbacks
    static void SetGetPcRegCallback(Rdb::GetProgramCounterFunc getPcReg_cb);
    static void SetReadMemoryCallback(Rdb::ReadMemoryFunc readMemory_cb);
    static void SetCheckBankableMemoryLocationCallback(Rdb::CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);
    static void SetReadBankableMemoryCallback(Rdb::ReadBankableMemoryFunc readBankableMemory_cb);
    static void SetGetRegSetCallback(Rdb::GetRegSetFunc getRegSet_cb);

private:
    // Callback functions
    static Rdb::GetProgramCounterFunc m_getPcReg_cb;
    static Rdb::ReadMemoryFunc m_readMemory_cb;
    static Rdb::CheckBankableMemoryLocationFunc m_CheckBankableMemoryLocation_cb;
    static Rdb::ReadBankableMemoryFunc m_readBankableMemory_cb;
    static Rdb::GetRegSetFunc m_getRegSet_cb;
};