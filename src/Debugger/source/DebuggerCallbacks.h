#pragma once

#include "IDebuggerCallbacks.h"

namespace Rdb {

class DebuggerCallback : public IDebuggerCallbacks{
public:
    unsigned int GetPcReg() override;
    unsigned int ReadMemory(unsigned int address) override;
    bool CheckBankableMemoryLocation(BankNum bank, unsigned int address) override;
    unsigned int ReadBankableMemory(BankNum bank, unsigned int address) override;
    RegSet GetRegSet() override;

    // Set Callbacks
    void SetGetPcRegCallback(Rdb::GetProgramCounterFunc getPcReg_cb);
    void SetReadMemoryCallback(Rdb::ReadMemoryFunc readMemory_cb);
    void SetCheckBankableMemoryLocationCallback(Rdb::CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb);
    void SetReadBankableMemoryCallback(Rdb::ReadBankableMemoryFunc readBankableMemory_cb);
    void SetGetRegSetCallback(Rdb::GetRegSetFunc getRegSet_cb);

private:
    // Callback functions
    Rdb::GetProgramCounterFunc m_getPcReg_cb;
    Rdb::ReadMemoryFunc m_readMemory_cb;
    Rdb::CheckBankableMemoryLocationFunc m_CheckBankableMemoryLocation_cb;
    Rdb::ReadBankableMemoryFunc m_readBankableMemory_cb;
    Rdb::GetRegSetFunc m_getRegSet_cb;
};

}