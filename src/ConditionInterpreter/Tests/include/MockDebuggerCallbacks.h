#pragma once

#include "IDebuggerCallbacks.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace Rdb {

class MockDebuggerCallbacks : public IDebuggerCallbacks {
public:
    MOCK_METHOD(unsigned int, GetPcReg, (), (override));
    MOCK_METHOD(unsigned int, ReadMemory, (unsigned int address), (override));
    MOCK_METHOD(bool, CheckBankableMemoryLocation, (BankNum bank, unsigned int address), (override));
    MOCK_METHOD(unsigned int, ReadBankableMemory, (BankNum bank, unsigned int address), (override));
    MOCK_METHOD(RegSet, GetRegSet, (), (override));
};

}