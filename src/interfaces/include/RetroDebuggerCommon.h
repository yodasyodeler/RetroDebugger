#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

// TODO: should there be a common namespace here?

// TODO: should this be unsigned? Linux GDB has uses for internal breakpoints(signed). I don't believe this will though.
enum class BreakNum : unsigned int;
enum class BankNum : unsigned int;

static constexpr BankNum AnyBank = BankNum{ std::numeric_limits<unsigned int>::max() };

// forward decl
namespace Rdb {
class ConditionInterpreter;
}

enum class BreakType : unsigned int {
    Invalid = static_cast<unsigned int>(-1),
    Watchpoint = 0,
    Breakpoint,
    Catchpoint,
    ReadWatchpoint,
    AnyWatchpoint,
};

enum class BreakDisposition {
    Keep = 0,
    Delete,
    Disable,
};

struct BreakInfo {
    unsigned int address = std::numeric_limits<unsigned int>::max();
    BreakNum breakpointNumber = BreakNum{ std::numeric_limits<unsigned int>::max() };
    BankNum bankNumber = AnyBank;
    // unsigned int ignoreCount{}; //TODO: unimplemented
    // unsigned int enableCount; //TODO: unimplemented
    unsigned int timesHit{};
    unsigned int oldWatchValue{};
    unsigned int currentWatchValue{};
    BreakType type = BreakType::Invalid;
    BreakDisposition disp = BreakDisposition::Keep; // TODO: no way to use, is implemented though
    bool isEnabled = true;
    bool externalHit = false;
    std::string regName = {};
    std::shared_ptr<Rdb::ConditionInterpreter> condition;
};
using BreakList = std::map<BreakNum, BreakInfo>;

struct RegisterInfo {
    std::string name;
};
using RegisterInfoPtr = std::shared_ptr<RegisterInfo>;

using RegSet = std::map<std::string, unsigned int>;