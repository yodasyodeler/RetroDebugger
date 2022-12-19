#include "DebuggerCallbacks.h"

#include <limits>

Rdb::GetProgramCounterFunc DebuggerCallback::m_getPcReg_cb; // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - TODO: look into this more. Not solving now so blocking the warning for now.
Rdb::ReadMemoryFunc DebuggerCallback::m_readMemory_cb; // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - TODO: look into this more. Not solving now so blocking the warning for now.
Rdb::CheckBankableMemoryLocationFunc DebuggerCallback::m_CheckBankableMemoryLocation_cb; // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - TODO: look into this more. Not solving now so blocking the warning for now.
Rdb::ReadBankableMemoryFunc DebuggerCallback::m_readBankableMemory_cb; // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - TODO: look into this more. Not solving now so blocking the warning for now.
Rdb::GetRegSetFunc DebuggerCallback::m_getRegSet_cb; // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - TODO: look into this more. Not solving now so blocking the warning for now.

// Callback wrappers
unsigned int DebuggerCallback::GetPcReg() {
    if (m_getPcReg_cb) {
        return m_getPcReg_cb();
    }
    return std::numeric_limits<unsigned int>::max();
}

unsigned int DebuggerCallback::ReadMemory(unsigned int address) {
    if (m_readMemory_cb) {
        return m_readMemory_cb(address);
    }
    return std::numeric_limits<unsigned int>::max();
}

bool DebuggerCallback::CheckBankableMemoryLocation(BankNum bank, unsigned int address) {
    if (m_CheckBankableMemoryLocation_cb) {
        return m_CheckBankableMemoryLocation_cb(bank, address);
    }
    return false;
}

unsigned int DebuggerCallback::ReadBankableMemory(BankNum bank, unsigned int address) {
    if (m_readBankableMemory_cb) {
        return m_readBankableMemory_cb(bank, address);
    }
    return std::numeric_limits<unsigned int>::max();
}

RegSet DebuggerCallback::GetRegSet() {
    if (m_getRegSet_cb) {
        return m_getRegSet_cb();
    }
    return {};
}

// Callback Setter APIs
void DebuggerCallback::SetGetPcRegCallback(Rdb::GetProgramCounterFunc getPcReg_cb) {
    m_getPcReg_cb = std::move(getPcReg_cb);
}

void DebuggerCallback::SetReadMemoryCallback(Rdb::ReadMemoryFunc readMemory_cb) {
    m_readMemory_cb = std::move(readMemory_cb);
}

void DebuggerCallback::SetCheckBankableMemoryLocationCallback(Rdb::CheckBankableMemoryLocationFunc CheckBankableMemoryLocation_cb) {
    m_CheckBankableMemoryLocation_cb = std::move(CheckBankableMemoryLocation_cb);
}

void DebuggerCallback::SetReadBankableMemoryCallback(Rdb::ReadBankableMemoryFunc readBankableMemory_cb) {
    m_readBankableMemory_cb = std::move(readBankableMemory_cb);
}

void DebuggerCallback::SetGetRegSetCallback(Rdb::GetRegSetFunc getRegSet_cb) {
    m_getRegSet_cb = std::move(getRegSet_cb);
}