#include "DebuggerCallbacks.h"

// Callback functions
static GetProgramCounterFunc g_getPcReg_cb = nullptr;
static ReadMemoryFunc g_readMemory_cb = nullptr;
static CheckBankableMemoryLocationFunc g_CheckBankableMemoryLocation_cb = nullptr;
static ReadBankableMemoryFunc g_readBankableMemory_cb = nullptr;
static GetRegSetFunc g_getRegSet_cb = nullptr;


// Callback wrappers
namespace DebuggerCallback {
unsigned int GetPcReg() {
    if (g_getPcReg_cb) {
        return g_getPcReg_cb();
    }
    return std::numeric_limits<unsigned int>::max();
}


unsigned int ReadMemory(unsigned int address) {
    if (g_readMemory_cb) {
        return g_readMemory_cb(address);
    }
    return std::numeric_limits<unsigned int>::max();
}

bool CheckBankableMemoryLocation(BankNum bank, unsigned int address) {
    if (g_CheckBankableMemoryLocation_cb) {
        return g_CheckBankableMemoryLocation_cb(bank, address);
    }
    return false;
}

unsigned int ReadBankableMemory(BankNum bank, unsigned int address) {
    if (g_readBankableMemory_cb) {
        return g_readBankableMemory_cb(bank, address);
    }
    return std::numeric_limits<unsigned int>::max();
}

RegSet GetRegSet() {
    if (g_getRegSet_cb) {
        return g_getRegSet_cb();
    }
    return {};
}

// Callback Setter APIs
void SetGetPcRegCallback(GetProgramCounterFunc getPcReg_cb) {
    g_getPcReg_cb = getPcReg_cb;
}


void SetReadMemoryCallback(ReadMemoryFunc readMemory_cb) {
    g_readMemory_cb = readMemory_cb;
}

void SetCheckBankableMemoryLocationCallback(CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    g_CheckBankableMemoryLocation_cb = CheckBankableMemoryLocation_cb;
}

void SetReadBankableMemoryCallback(ReadBankableMemoryFunc readBankableMemory_cb) {
    g_readBankableMemory_cb = readBankableMemory_cb;
}

void SetGetRegSetCallback(GetRegSetFunc getRegSet_cb) {
    g_getRegSet_cb = getRegSet_cb;
}

} // namespace DebuggerCallback