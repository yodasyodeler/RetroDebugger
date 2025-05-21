#include "BreakpointManager.h"

#include "ConditionInterpreter.h"
#include "DebuggerError.h"
#include "DebuggerOperations.h"
#include "IDebuggerCallbacks.h"

#include <fmt/core.h>

#include <limits>
#include <memory>

namespace {
BreakNum operator++(BreakNum& breakNum, int) {
    auto num = breakNum;
    breakNum = BreakNum{ static_cast<unsigned int>(breakNum) + 1u };
    return num;
}

BreakInfo BreakPoint(BreakNum breakNumber, unsigned int address, BankNum bankNumber = AnyBank) {
    return BreakInfo{
        .address = address,
        .breakpointNumber = breakNumber,
        .bankNumber = bankNumber,
        .type = BreakType::Breakpoint,
    };
}

BreakInfo WatchPoint(const std::shared_ptr<Rdb::IDebuggerCallbacks>& callbacks, BreakNum breakNumber, unsigned int address, BankNum bankNumber = AnyBank) {
    return BreakInfo{
        .address = address,
        .breakpointNumber = breakNumber,
        .bankNumber = bankNumber,
        .currentWatchValue = callbacks->ReadMemory(address),
        .type = BreakType::Watchpoint,
    };
}

BreakInfo ReadWatchPoint(const std::shared_ptr<Rdb::IDebuggerCallbacks>& callbacks, BreakNum breakNumber, unsigned int address, BankNum bankNumber = AnyBank) {
    return BreakInfo{
        .address = address,
        .breakpointNumber = breakNumber,
        .bankNumber = bankNumber,
        .currentWatchValue = callbacks->ReadMemory(address),
        .type = BreakType::ReadWatchpoint,
    };
}

BreakInfo AnyWatchPoint(const std::shared_ptr<Rdb::IDebuggerCallbacks>& callbacks, BreakNum breakNumber, unsigned int address, BankNum bankNumber = AnyBank) {
    return BreakInfo{
        .address = address,
        .breakpointNumber = breakNumber,
        .bankNumber = bankNumber,
        .currentWatchValue = callbacks->ReadMemory(address),
        .type = BreakType::AnyWatchpoint,
    };
}

// TODO: May want to consider redoing design so we return the BreakInfo instead of tunneling a BreakInfo object.
// Current design has breakpoint info passed in as ref so it can be assigned, this represents an invalid or uninteresting breakpoint.
BreakInfo ContinuePoint() {
    return BreakInfo{
        .breakpointNumber = Rdb::MaxBreakpointNumber,
        .disp = BreakDisposition::Delete,
        .isEnabled = false,
    };
}

// Watch a register
BreakInfo WatchPoint(const std::shared_ptr<Rdb::IDebuggerCallbacks>& callbacks, BreakNum breakNumber, const std::string& registerName) {
    const auto regset = callbacks->GetRegSet();
    if (const auto iter = regset.find(registerName);
        iter != regset.end()) {
        return BreakInfo{
            .address = std::numeric_limits<unsigned int>::max(),
            .breakpointNumber = breakNumber,
            .currentWatchValue = iter->second,
            .type = BreakType::Watchpoint,
            .regName = registerName,
        };
    }
    else {
        throw Rdb::DebuggerError(fmt::format("Failed to watch unrecognized identifier '{}'.", registerName));
    }
}

BreakInfo ReadWatchPoint(const std::shared_ptr<Rdb::IDebuggerCallbacks>& callbacks, BreakNum breakNumber, const std::string& registerName) {
    const auto regset = callbacks->GetRegSet();
    if (const auto iter = regset.find(registerName);
        iter != regset.end()) {
        return BreakInfo{
            .address = std::numeric_limits<unsigned int>::max(),
            .breakpointNumber = breakNumber,
            .currentWatchValue = iter->second,
            .type = BreakType::ReadWatchpoint,
            .regName = registerName,
        };
    }
    else {
        throw Rdb::DebuggerError(fmt::format("Failed to watch unrecognized identifier '{}'.", registerName));
    }
}

BreakInfo AnyWatchPoint(const std::shared_ptr<Rdb::IDebuggerCallbacks>& callbacks, BreakNum breakNumber, const std::string& registerName) {
    const auto regset = callbacks->GetRegSet();
    if (const auto iter = regset.find(registerName);
        iter != regset.end()) {
        return BreakInfo{
            .address = std::numeric_limits<unsigned int>::max(),
            .breakpointNumber = breakNumber,
            .currentWatchValue = iter->second,
            .type = BreakType::AnyWatchpoint,
            .regName = registerName,
        };
    }
    else {
        throw Rdb::DebuggerError(fmt::format("Failed to watch unrecognized identifier '{}'.", registerName));
    }
}

unsigned int GetWatchpointValue(const std::shared_ptr<Rdb::IDebuggerCallbacks>& callbacks, const BreakInfo& breakInfo) {
    if (breakInfo.isEnabled) {
        if (breakInfo.regName.empty()) {
            return breakInfo.bankNumber == AnyBank ? callbacks->ReadMemory(breakInfo.address) : callbacks->ReadBankableMemory(breakInfo.bankNumber, breakInfo.address);
        }
        else {
            const auto regset = callbacks->GetRegSet();
            if (auto regsetIter = regset.find(breakInfo.regName);
                regsetIter != regset.end()) {
                return regsetIter->second;
            }
            // Fallthrough if register doesn't exist, this shouldn't be possible in normal operation as regset is not expected to change entries while running.
        }
    }
    return {};
}

}


namespace Rdb {

BreakpointManager::BreakpointManager(std::shared_ptr<DebuggerOperations> operations, std::shared_ptr<IDebuggerCallbacks> callbacks) :
    m_operations(std::move(operations)),
    m_callbacks(std::move(callbacks)) {}

bool BreakpointManager::CheckBreakpoints(BreakInfo& breakInfo) {
    breakInfo = CheckBreakInfo();
    return HandleBreakInfo(breakInfo);
}

bool BreakpointManager::Run(const unsigned int numBreakpointsToSkip) {
    m_debugOp = DebugOperation::RunOp;
    m_instructionsToStep = numBreakpointsToSkip;
    return false;
}

bool BreakpointManager::RunInstructions(const unsigned int numInstructions) {
    m_debugOp = DebugOperation::StepOp;
    m_instructionsToStep = numInstructions;
    return true;
}

bool BreakpointManager::RunTillJump() {
    m_debugOp = DebugOperation::FinishOp;
    return true;
}

BreakNum BreakpointManager::SetBreakpoint(const unsigned int address) {
    const BreakInfo breakpoint = BreakPoint(m_breakPointCounter++, address);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetBreakpoint(const BankNum bank, const unsigned int address) {
    const BreakInfo breakpoint = BreakPoint(m_breakPointCounter++, address, bank);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

void BreakpointManager::SetCondition(BreakNum breakNum, const std::string& condition) {
    auto iter = m_breakpoints.find(breakNum);
    if (iter == m_breakpoints.end()) {
        throw Rdb::DebuggerError(fmt::format("{}", static_cast<unsigned int>(breakNum)));
    }
    iter->second.condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, condition);
}

BreakNum BreakpointManager::SetWatchpoint(const unsigned int address, BankNum bankNum) {
    const BreakInfo breakpoint = WatchPoint(m_callbacks, m_breakPointCounter++, address, bankNum);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetReadWatchpoint(unsigned int address, BankNum bank) {
    const BreakInfo breakpoint = ReadWatchPoint(m_callbacks, m_breakPointCounter++, address, bank);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetAnyWatchpoint(unsigned int address, BankNum bank) {
    const BreakInfo breakpoint = AnyWatchPoint(m_callbacks, m_breakPointCounter++, address, bank);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetWatchpoint(const std::string& name) {
    const BreakInfo breakpoint = WatchPoint(m_callbacks, m_breakPointCounter++, name);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

// BreakNum BreakpointManager::SetReadWatchpoint(const std::string& name) {
//    const BreakInfo breakpoint = ReadWatchPoint(m_callbacks, m_breakPointCounter++, name);
//    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);
//
//    return breakpoint.breakpointNumber;
//}
//
// BreakNum BreakpointManager::SetAnyWatchpoint(const std::string& name) {
//    const BreakInfo breakpoint = AnyWatchPoint(m_callbacks, m_breakPointCounter++, name);
//    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);
//
//    return breakpoint.breakpointNumber;
//}

bool BreakpointManager::EnableBreakpoints(const std::vector<BreakNum>& list) {
    return ModifyBreak(list, true);
}

bool BreakpointManager::DisableBreakpoints(const std::vector<BreakNum>& list) {
    return ModifyBreak(list, false);
}

bool BreakpointManager::DeleteBreakpoints(const std::vector<BreakNum>& list) {
    // If no breakpoints are specified, delete them all
    if (list.empty()) {
        m_breakpoints.clear();
        m_breakPointCounter = BreakNum{ 1u };
    }

    // Delete breakpoints from list
    auto breakPointDeleted = false;
    for (const auto& breakpointNum : list) {
        if (m_breakpoints.contains(breakpointNum)) {
            breakPointDeleted = true;
            m_breakpoints.erase(breakpointNum);
        }
    }
    return breakPointDeleted;
}

BreakList BreakpointManager::GetBreakpointInfoList(const std::vector<BreakNum>& list) {
    if (list.empty()) { return m_breakpoints; }

    BreakList tempMap = {};
    for (const auto& breakpointNum : list) {
        if (auto iter = m_breakpoints.find(breakpointNum);
            iter != m_breakpoints.end()) {
            tempMap.emplace(*iter);
        }
    }
    return tempMap;
}

void BreakpointManager::ReadMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes) {

    const auto addressEnd = address + bytes.size(); // Note: This goes 1 past the end.
    for (auto& [breakNum, breakInfo] : m_breakpoints) {
        if (!breakInfo.isEnabled || (breakInfo.type != BreakType::ReadWatchpoint && breakInfo.type != BreakType::AnyWatchpoint)) {
            continue;
        }

        if ((breakInfo.bankNumber == AnyBank || breakInfo.bankNumber == bankNum) && (address <= breakInfo.address && breakInfo.address < addressEnd)) {
            breakInfo.externalHit = true;
        }
    }
}

void BreakpointManager::WriteMemoryHook(BankNum bankNum, unsigned int address, const std::vector<std::byte>& bytes) {

    const auto addressEnd = address + bytes.size(); // Note: This goes 1 past the end.
    for (auto& [breakNum, breakInfo] : m_breakpoints) {
        if (!breakInfo.isEnabled || (breakInfo.type != BreakType::Watchpoint && breakInfo.type != BreakType::AnyWatchpoint)) {
            continue;
        }

        if ((breakInfo.bankNumber == AnyBank || breakInfo.bankNumber == bankNum) && (address <= breakInfo.address && breakInfo.address < addressEnd)) {
            breakInfo.externalHit = true;
        }
    }
}

BreakInfo BreakpointManager::CheckBreakInfo() {
    for (auto& [breakNum, breakInfo] : m_breakpoints) {
        if (breakInfo.isEnabled) {
            if (breakInfo.type == BreakType::Breakpoint && (breakInfo.bankNumber == AnyBank && breakInfo.address == m_callbacks->GetPcReg()) || // NON-Bank Breakpoint
                (breakInfo.bankNumber != AnyBank && m_callbacks->CheckBankableMemoryLocation(breakInfo.bankNumber, breakInfo.address))) { // Bank Breakpoint

                if (breakInfo.condition == nullptr || breakInfo.condition->EvaluateCondition()) {
                    ++breakInfo.timesHit;
                    return breakInfo;
                }
            }
            if (breakInfo.type == BreakType::Watchpoint || breakInfo.type == BreakType::ReadWatchpoint || breakInfo.type == BreakType::AnyWatchpoint) {
                // Watching an address
                if (const auto currentWatchValue = GetWatchpointValue(m_callbacks, breakInfo);
                    breakInfo.externalHit || (breakInfo.type != BreakType::ReadWatchpoint && breakInfo.currentWatchValue != currentWatchValue)) {
                    if (breakInfo.condition == nullptr || breakInfo.condition->EvaluateCondition()) {
                        breakInfo.oldWatchValue = breakInfo.currentWatchValue;
                        breakInfo.currentWatchValue = currentWatchValue;
                        ++breakInfo.timesHit;
                        breakInfo.externalHit = false; // Clear the external hit
                        return breakInfo;
                    }
                }
            }
        }
    }

    return ContinuePoint();
}

bool BreakpointManager::HandleBreakInfo(const BreakInfo& info) {
    // TODO: Need to know how this should interact with continue like operations, for now always break on valid non-standard breakpoints.
    if (info.type != BreakType::Breakpoint && info.breakpointNumber != MaxBreakpointNumber) { return true; }

    // Check if we should actually break the program, run/step operations can ignore breaks.
    const auto breakpointHit = (info.type == BreakType::Breakpoint && info.breakpointNumber != MaxBreakpointNumber);
    switch (m_debugOp) {
        case DebugOperation::RunOp:
            if ((m_instructionsToStep == 0) && breakpointHit) {
                return true;
            }
            if (breakpointHit) {
                --m_instructionsToStep;
            }
            break;
        case DebugOperation::StepOp:
            if ((m_instructionsToStep == 0) || breakpointHit) {
                return true;
            }
            --m_instructionsToStep;
            break;
        case DebugOperation::FinishOp:
            if (m_operations) {
                const auto pcReg = m_callbacks->GetPcReg();
                const auto cmd = m_callbacks->ReadMemory(pcReg);
                const auto jumpInstructions = m_operations->GetJumpOperations();

                const auto extendedOperation = jumpInstructions.extendedOperations.find(cmd);
                if (extendedOperation != jumpInstructions.extendedOperations.end()) {
                    const auto extendedCommand = m_callbacks->ReadMemory(pcReg + jumpInstructions.opcodeLength);
                    if (extendedOperation->second.operations.contains(extendedCommand)) {
                        return true;
                    }
                }
                else if (jumpInstructions.operations.find(cmd) != jumpInstructions.operations.end()) {
                    return true;
                }
            }
            break;
    };
    return false;
}

bool BreakpointManager::ModifyBreak(const std::vector<BreakNum>& list, bool isEnabled) {
    bool foundBreakpoint = false;
    for (const auto& breakpointNum : list) {
        if (auto iter = m_breakpoints.find(breakpointNum);
            iter != m_breakpoints.end()) {
            foundBreakpoint = true;
            iter->second.isEnabled = isEnabled;
        }
    }
    return foundBreakpoint;
}

}