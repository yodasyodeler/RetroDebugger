#include "BreakpointManager.h"
#include "DebuggerCallbacks.h"
#include "DebuggerOperations.h"

#include <limits>

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
        // 0, //ignoreCount not implemented yet
        // 0, //enableCount not implemented yet
        .timesHit = 0,
        .oldWatchValue = 0,
        .currentWatchValue = 0,
        .type = BreakType::Breakpoint,
        .disp = BreakDisposition::Keep,
        .isEnabled = true,
        .externalHit = false,
        .regName = {},
    };
}

BreakInfo WatchPoint(BreakNum breakNumber, unsigned int address, BankNum bankNumber = AnyBank) {
    return BreakInfo{
        .address = address,
        .breakpointNumber = breakNumber,
        .bankNumber = bankNumber,
        // 0, //ignoreCount not implemented yet
        // 0, //enableCount not implemented yet
        .timesHit = 0,
        .oldWatchValue = 0,
        .currentWatchValue = DebuggerCallback::ReadMemory(address),
        .type = BreakType::Watchpoint,
        .disp = BreakDisposition::Keep,
        .isEnabled = true,
        .externalHit = false,
        .regName = {},
    };
}

BreakInfo ReadWatchPoint(BreakNum breakNumber, unsigned int address, BankNum bankNumber = AnyBank) {
    return BreakInfo{
        .address = address,
        .breakpointNumber = breakNumber,
        .bankNumber = AnyBank,
        // 0, //ignoreCount not implemented yet
        // 0, //enableCount not implemented yet
        .timesHit = 0,
        .oldWatchValue = 0,
        .currentWatchValue = DebuggerCallback::ReadMemory(address),
        .type = BreakType::ReadWatchpoint,
        .disp = BreakDisposition::Keep,
        .isEnabled = true,
        .externalHit = false,
        .regName = {},
    };
}

// TODO: May want to consider redoing design so we return the BreakInfo instead of tunneling a BreakInfo object.
// Current design has breakpoint info passed in as ref so it can be assigned, this represents an invalid or uninteresting breakpoint.
BreakInfo ContinuePoint() {
    return BreakInfo{
        .address = std::numeric_limits<unsigned int>::max(),
        .breakpointNumber = MaxBreakpointNumber,
        .bankNumber = AnyBank,
        // 0, //ignoreCount not implemented yet
        // 0, //enableCount not implemented yet
        .timesHit = 0,
        .oldWatchValue = 0,
        .currentWatchValue = 0,
        .type = BreakType::Invalid,
        .disp = BreakDisposition::Delete,
        .isEnabled = false,
        .externalHit = false,
        .regName = {},
    };
}

// Watch a register
BreakInfo WatchPoint(BreakNum breakNumber, const std::string& registerName) {
    const auto regset = DebuggerCallback::GetRegSet();
    if (const auto iter = regset.find(registerName);
        iter != regset.end()) {
        return BreakInfo{
            .address = std::numeric_limits<unsigned int>::max(),
            .breakpointNumber = breakNumber,
            .bankNumber = AnyBank,
            // 0, //ignoreCount not implemented yet
            // 0, //enableCount not implemented yet
            .timesHit = 0,
            .oldWatchValue = 0,
            .currentWatchValue = iter->second,
            .type = BreakType::Watchpoint,
            .disp = BreakDisposition::Keep,
            .isEnabled = true,
            .externalHit = false,
            .regName = registerName,
        };
    }

    // TODO: error handling
    return ContinuePoint();
}

BreakInfo ReadWatchPoint(BreakNum breakNumber, const std::string& registerName) {
    const auto regset = DebuggerCallback::GetRegSet();
    if (const auto iter = regset.find(registerName);
        iter != regset.end()) {
        return BreakInfo{
            .address = std::numeric_limits<unsigned int>::max(),
            .breakpointNumber = breakNumber,
            .bankNumber = AnyBank,
            // 0, //ignoreCount not implemented yet
            // 0, //enableCount not implemented yet
            .timesHit = 0,
            .oldWatchValue = 0,
            .currentWatchValue = iter->second,
            .type = BreakType::ReadWatchpoint,
            .disp = BreakDisposition::Keep,
            .isEnabled = true,
            .externalHit = false,
            .regName = {},
        };
    }

    // TODO: error handling
    return ContinuePoint();
}

}


BreakpointManager::BreakpointManager(std::shared_ptr<DebuggerOperations> operations) :
    m_operations(std::move(operations)) {}

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

BreakNum BreakpointManager::SetWatchpoint(const unsigned int address, BankNum bankNum) {
    const BreakInfo breakpoint = WatchPoint(m_breakPointCounter++, address, bankNum);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetReadWatchpoint(unsigned int address, BankNum bank) {
    const BreakInfo breakpoint = ReadWatchPoint(m_breakPointCounter++, address, bank);
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetWatchpoint(const std::string& registerName) {
    const BreakInfo breakpoint = WatchPoint(m_breakPointCounter++, registerName);
    if (breakpoint.breakpointNumber == MaxBreakpointNumber) { return MaxBreakpointNumber; }

    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);
    return breakpoint.breakpointNumber;
}

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

        if ((breakInfo.bankNumber == AnyBank || breakInfo.bankNumber == bankNum) && (breakInfo.address >= address && address < addressEnd)) {
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

        if ((breakInfo.bankNumber == AnyBank || breakInfo.bankNumber == bankNum) && (breakInfo.address >= address && address < addressEnd)) {
            breakInfo.externalHit = true;
        }
    }
}

BreakInfo BreakpointManager::CheckBreakInfo() {
    for (auto& [breakNum, breakInfo] : m_breakpoints) {
        if (breakInfo.isEnabled) {
            if (breakInfo.type == BreakType::Breakpoint && (breakInfo.bankNumber == AnyBank && breakInfo.address == DebuggerCallback::GetPcReg()) || // NON-Bank Breakpoint
                (breakInfo.bankNumber != AnyBank && DebuggerCallback::CheckBankableMemoryLocation(breakInfo.bankNumber, breakInfo.address))) { // Bank Breakpoint

                ++breakInfo.timesHit;
                return breakInfo;
            }
            if (breakInfo.type == BreakType::Watchpoint || breakInfo.type == BreakType::ReadWatchpoint || breakInfo.type == BreakType::AnyWatchpoint) {
                const auto currentWatchValue = breakInfo.bankNumber == AnyBank ? DebuggerCallback::ReadMemory(breakInfo.address) : DebuggerCallback::ReadBankableMemory(breakInfo.bankNumber, breakInfo.address);

                if (breakInfo.externalHit || breakInfo.currentWatchValue != currentWatchValue) {
                    breakInfo.oldWatchValue = breakInfo.currentWatchValue;
                    breakInfo.currentWatchValue = currentWatchValue;
                    ++breakInfo.timesHit;
                    breakInfo.externalHit = false; // Clear the external hit
                    return breakInfo;
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
                const auto pcReg = DebuggerCallback::GetPcReg();
                const auto cmd = DebuggerCallback::ReadMemory(pcReg);
                const auto jumpInstructions = m_operations->GetJumpOperations();

                const auto extendedOperation = jumpInstructions.extendedOperations.find(cmd);
                if (extendedOperation != jumpInstructions.extendedOperations.end()) {
                    const auto extendedCommand = DebuggerCallback::ReadMemory(pcReg + jumpInstructions.opcodeLength);
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
