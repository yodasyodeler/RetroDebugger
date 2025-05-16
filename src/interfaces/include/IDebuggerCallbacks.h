#pragma once

#include "RetroDebuggerCallbackDefines.h"

namespace Rdb {

class IDebuggerCallbacks {
public:
    virtual unsigned int GetPcReg() = 0;
    virtual unsigned int ReadMemory(unsigned int address) = 0;
    virtual bool CheckBankableMemoryLocation(BankNum bank, unsigned int address) = 0;
    virtual unsigned int ReadBankableMemory(BankNum bank, unsigned int address) = 0;
    virtual RegSet GetRegSet() = 0;
};

}