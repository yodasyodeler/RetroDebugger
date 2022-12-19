#include "BreakpointManager.h"
#include "DebuggerCallbacks.h"
#include "DebuggerOperations.h"

#include <limits>

BreakpointManager::BreakpointManager(std::shared_ptr<DebuggerOperations> operations) :
    m_operations(std::move(operations)) {}

bool BreakpointManager::CheckBreakpoints(BreakInfo& breakInfo) {
    breakInfo.breakpointNumber = MaxBreakpointNumber;
    CheckBreakInfo(breakInfo); //

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
    const BreakInfo breakpoint = {
        address,
        m_breakPointCounter++,
        0,
        0,
        // 0, //ignoreCount not implemented yet
        // 0, //enableCount not implemented yet
        0,
        0,
        BreakType::Breakpoint,
        BreakDisposition::Keep,
        true
    };
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetBreakpoint(const BankNum bank, const unsigned int address) {
    const BreakInfo breakpoint = {
        address,
        m_breakPointCounter++,
        bank,
        0,
        // 0, //ignoreCount not implemented yet
        // 0, //enableCount not implemented yet
        0,
        0,
        BreakType::BankBreakpoint,
        BreakDisposition::Keep,
        true
    };
    m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);

    return breakpoint.breakpointNumber;
}

BreakNum BreakpointManager::SetWatchpoint(const unsigned int addressStart, unsigned int addressEnd) {
    if (addressStart > addressEnd) { return std::numeric_limits<BreakNum>::max(); }
    if (addressEnd == std::numeric_limits<BreakNum>::max()) { addressEnd = addressStart; }

    // TODO: this is experimental, but could be optimized by adding a range breakpoint. Also may make sense to make BreakInfo a polymorphed class to make it easier to read and maintain
    const BreakNum breakNum = m_breakPointCounter + 1;
    for (auto address = addressStart; address <= addressEnd; ++address) {
        const BreakInfo breakpoint = {
            address,
            m_breakPointCounter++,
            0,
            0,
            // 0, //ignoreCount not implemented yet
            // 0, //enableCount not implemented yet
            0,
            DebuggerCallback::ReadMemory(address),
            BreakType::Watchpoint,
            BreakDisposition::Keep,
            true
        };
        m_breakpoints.emplace(breakpoint.breakpointNumber, breakpoint);
    }

    return breakNum;
}

// BreakNum BreakpointManager::SetWatchpoint(BankNum /*bank*/, unsigned int /*addressStart*/, unsigned int /*addressEnd*/) {
//     return std::numeric_limits<BreakNum>::max();
// }

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
        if (m_breakpoints.find(breakpointNum) != m_breakpoints.end()) {
            breakPointDeleted = true;
            m_breakpoints.erase(breakpointNum);
        }
    }
    return breakPointDeleted;
}

std::map<unsigned int, BreakInfo> BreakpointManager::GetBreakpointInfoList(const std::vector<unsigned int>& list) {
    if (list.empty()) { return m_breakpoints; }

    std::map<BreakNum, BreakInfo> tempMap = {};
    for (const auto& breakpointNum : list) {
        if (m_breakpoints.find(breakpointNum) != m_breakpoints.end()) {
            tempMap.emplace(breakpointNum, m_breakpoints.at(breakpointNum));
        }
    }
    return tempMap;
}

void BreakpointManager::CheckBreakInfo(BreakInfo& info) {
    const auto pcReg = DebuggerCallback::GetPcReg();
    for (auto& breakpoint : m_breakpoints) {
        auto& breakInfo = breakpoint.second;
        if (breakInfo.isEnabled) {
            if ((breakInfo.type == BreakType::Breakpoint) && (breakInfo.address == pcReg)) {
                ++breakInfo.timesHit;
                info = breakInfo;
                break;
            }
            if ((breakInfo.type == BreakType::BankBreakpoint) && DebuggerCallback::CheckBankableMemoryLocation(info.bankNumber, info.address)) {
                ++breakInfo.timesHit;
                info = breakInfo;
                break;
            }
            if (breakInfo.type == BreakType::Watchpoint) {
                const auto newWatchValue = DebuggerCallback::ReadMemory(info.address);
                if (breakInfo.newWatchValue != newWatchValue) {
                    breakInfo.oldWatchValue = breakInfo.newWatchValue;
                    breakInfo.newWatchValue = newWatchValue;
                    ++breakInfo.timesHit;
                    info = breakInfo;
                    break;
                }
            }
        }
    }
}

bool BreakpointManager::HandleBreakInfo(const BreakInfo& info) {
    const auto breakpointHit = (info.breakpointNumber != MaxBreakpointNumber);
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
            const auto jumpInstructions = m_operations->GetJumpOpertions();

            const auto extendedOperation = jumpInstructions.extendedOperations.find(cmd);
            if (extendedOperation != jumpInstructions.extendedOperations.end()) {
                const auto extendedCommand = DebuggerCallback::ReadMemory(pcReg + jumpInstructions.opcodeLength);
                if (extendedOperation->second.find(extendedCommand) != extendedOperation->second.end()) {
                    return true;
                }
            }
            else if (jumpInstructions.operations.find(cmd) != jumpInstructions.operations.end()) {
                return true;
            }
        }
        break;
    case DebugOperation::WatchOp: // TODO:
        break;
    };
    return false;
}

bool BreakpointManager::ModifyBreak(const std::vector<BreakNum>& list, bool isEnabled) {
    bool foundBreakpoint = false;
    for (const auto& breakpointNum : list) {
        if (m_breakpoints.find(breakpointNum) != m_breakpoints.end()) {
            foundBreakpoint = true;
            m_breakpoints.at(breakpointNum).isEnabled = isEnabled;
        }
    }
    return foundBreakpoint;
}
