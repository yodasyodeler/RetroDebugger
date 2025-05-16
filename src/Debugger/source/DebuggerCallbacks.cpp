#include "DebuggerCallbacks.h"

#include <limits>

namespace Rdb {

// Callback wrappers
unsigned int DebuggerCallbacks::GetPcReg() {
    if (m_getPcReg_cb) {
        return m_getPcReg_cb();
    }
    return std::numeric_limits<unsigned int>::max();
}

unsigned int DebuggerCallbacks::ReadMemory(unsigned int address) {
    if (m_readMemory_cb) {
        return m_readMemory_cb(address);
    }
    return std::numeric_limits<unsigned int>::max();
}

bool DebuggerCallbacks::CheckBankableMemoryLocation(BankNum bank, unsigned int address) {
    if (m_CheckBankableMemoryLocation_cb) {
        return m_CheckBankableMemoryLocation_cb(bank, address);
    }
    return false;
}

unsigned int DebuggerCallbacks::ReadBankableMemory(BankNum bank, unsigned int address) {
    if (m_readBankableMemory_cb) {
        return m_readBankableMemory_cb(bank, address);
    }
    return std::numeric_limits<unsigned int>::max();
}

RegSet DebuggerCallbacks::GetRegSet() {
    if (m_getRegSet_cb) {
        return m_getRegSet_cb();
    }
    return {};
}

// Callback Setter APIs
void DebuggerCallbacks::SetGetPcRegCallback(Rdb::GetProgramCounterFunc getPcReg_cb) {
    m_getPcReg_cb = std::move(getPcReg_cb);
}

void DebuggerCallbacks::SetReadMemoryCallback(Rdb::ReadMemoryFunc readMemory_cb) {
    m_readMemory_cb = std::move(readMemory_cb);
}

void DebuggerCallbacks::SetCheckBankableMemoryLocationCallback(Rdb::CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    m_CheckBankableMemoryLocation_cb = std::move(CheckBankableMemoryLocation_cb);
}

void DebuggerCallbacks::SetReadBankableMemoryCallback(Rdb::ReadBankableMemoryFunc readBankableMemory_cb) {
    m_readBankableMemory_cb = std::move(readBankableMemory_cb);
}

void DebuggerCallbacks::SetGetRegSetCallback(Rdb::GetRegSetFunc getRegSet_cb) {
    m_getRegSet_cb = std::move(getRegSet_cb);
}

}