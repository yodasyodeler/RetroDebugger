#pragma once
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

// TODO: should there be a common namespace here?

// TODO: should this be unsigned? Linux GDB has uses for internal breakpoints(signed). I don't believe this will though.
typedef unsigned int BreakNum;
typedef unsigned int BankNum;

enum class BreakType : unsigned int {
    Invalid = static_cast<unsigned int>(-1),
    Watchpoint = 0,
    Breakpoint,
    BankBreakpoint,
    Catchpoint,
};

enum class BreakDisposition {
    Keep = 0,
    Delete,
    Disable,
};

struct BreakInfo
{
    unsigned int address{};
    BreakNum breakpointNumber{};
    BankNum bankNumber{};
    // unsigned int ignoreCount{}; //TODO: unimplemented
    // unsigned int enableCount; //TODO: unimplemented
    unsigned int timesHit{};
    unsigned int oldWatchValue{};
    unsigned int newWatchValue{};
    BreakType type = BreakType::Invalid;
    BreakDisposition disp = BreakDisposition::Disable; // TODO: no way to use, is implemented though
    bool isEnabled = false;
};

struct RegisterInfo
{
    std::string name;
};
using RegisterInfoPtr = std::shared_ptr<RegisterInfo>;

using RegSet = std::map<std::string, unsigned int>;


namespace Rdb {
typedef std::function<unsigned int()> GetProgramCounterFunc;

typedef std::function<unsigned int(unsigned int)> ReadMemoryFunc;

typedef std::function<unsigned int(BankNum bank, unsigned int)> ReadBankableMemoryFunc;

typedef std::function<bool(BankNum bank, unsigned int address)> CheckBankableMemoryLocationFunc;

typedef std::function<RegSet()> GetRegSetFunc;
}