#include "BreakpointManager.h"
#include "DebuggerCallbacks.h"
#include "DebuggerOperations.h"

BreakpointManager::BreakpointManager(std::shared_ptr<DebuggerOperations> operations) :
    m_operations(operations) {}

bool BreakpointManager::CheckBreakpoints(BreakInfo& breakInfo) {
    breakInfo.breakpointNumber = MAX_BREAKPOINT_NUM;
    const auto pc = DebuggerCallback::GetPcReg();

    // TODO: a temp map of BreakInfo point with keys of address may be a more performant option than cycling through every breakpoint
    for (auto& breakpoint : m_breakpoints) {
        auto& info = breakpoint.second;
        if ((info.type == BreakType::Breakpoint) && info.isEnabled && (info.address == pc)) {
            ++info.timesHit;
            breakInfo = info;
            break;
        }
        if ((info.type == BreakType::BankBreakpoint) && info.isEnabled && DebuggerCallback::CheckBankableMemoryLocation(info.bankNumber, info.address)) {
            ++info.timesHit;
            breakInfo = info;
            break;
        }
        if ((info.type == BreakType::Watchpoint) && info.isEnabled) {
            const auto newWatchValue = DebuggerCallback::ReadMemory(info.address);
            if (info.newWatchValue != newWatchValue) {
                info.oldWatchValue = info.newWatchValue;
                info.newWatchValue = newWatchValue;
                ++info.timesHit;
                breakInfo = info;
                break;
            }
        }
    }

    const auto breakpointHit = (breakInfo.breakpointNumber != MAX_BREAKPOINT_NUM);

    switch (m_debugOp) {
    case DebugOperation::RunOp:
        if (!m_instructionsToStep && breakpointHit)
            return true;

        if (breakpointHit)
            --m_instructionsToStep;
        break;
    case DebugOperation::StepOp:
        if (!m_instructionsToStep || breakpointHit) {
            return true;
        }
        --m_instructionsToStep;
        break;
    case DebugOperation::FinishOp:
        if (m_operations) {
            const auto cmd = DebuggerCallback::ReadMemory(pc);
            const auto jumpInstructions = m_operations->GetJumpOpertions();

            const auto extendedOperation = jumpInstructions.extendedOperations.find(cmd);
            if (extendedOperation != jumpInstructions.extendedOperations.end()) {
                const auto extendedCommand = DebuggerCallback::ReadMemory(pc + jumpInstructions.opcodeLength);
                if (extendedOperation->second.find(extendedCommand) != extendedOperation->second.end()) {
                    return true;
                }
            }
            else if (jumpInstructions.operations.find(cmd) != jumpInstructions.operations.end()) {
                return true;
            }
            break;
        }
        [[fallthrough]]; // TODO: error message?
    default:
        // TODO: throw?
        break;
    };

    return false;
}

bool BreakpointManager::Run(const unsigned int numBreakToPass) {
    m_debugOp = DebugOperation::RunOp;
    m_instructionsToStep = numBreakToPass;
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
    BreakInfo breakpoint = {
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
    BreakInfo breakpoint = {
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
    BreakNum breakNum = m_breakPointCounter + 1;
    for (auto address = addressStart; address <= addressEnd; ++address) {
        BreakInfo breakpoint = {
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

BreakNum BreakpointManager::SetWatchpoint(BankNum /*bank*/, unsigned int /*addressStart*/, unsigned int /*addressEnd*/) {
    return std::numeric_limits<BreakNum>::max();
}

bool BreakpointManager::EnableBreakpoints(const std::vector<BreakNum>& list) {
    return ModifyBreak(list, true);
}

bool BreakpointManager::DisableBreakpoints(const std::vector<BreakNum>& list) {
    return ModifyBreak(list, false);
}

bool BreakpointManager::DeleteBreakpoints(const std::vector<BreakNum>& list) {
    // If no breakpoints are specified, delete them all
    if (list.empty())
        m_breakpoints.clear();

    // Delete breakpoints from list
    auto re = false;
    for (const auto& breakpointNum : list) {
        if (m_breakpoints.find(breakpointNum) != m_breakpoints.end()) {
            re = true;
            m_breakpoints.erase(breakpointNum);
        }
    }
    return re;
}

std::map<unsigned int, BreakInfo> BreakpointManager::GetBreakpointInfoList(const std::vector<unsigned int>& list) {
    if (list.empty())
        return m_breakpoints;

    std::map<BreakNum, BreakInfo> tempMap = {};
    for (const auto& breakpointNum : list) {
        if (m_breakpoints.find(breakpointNum) != m_breakpoints.end()) {
            tempMap.emplace(breakpointNum, m_breakpoints.at(breakpointNum));
        }
    }
    return tempMap;
}

bool BreakpointManager::ModifyBreak(std::vector<BreakNum> list, bool isEnabled) {
    bool re = false;
    for (const auto& breakpointNum : list) {
        if (m_breakpoints.find(breakpointNum) != m_breakpoints.end()) {
            re = true;
            m_breakpoints.at(breakpointNum).isEnabled = isEnabled;
        }
    }
    return re;
}
