#include "BreakpointManager.h"
#include "DebuggerCallbacks.h"

#include <gtest/gtest.h>

#include <algorithm>

/******************************************************************************
 * TODOs
 *  AddUnitTests for BreakpointManager with operations
 *      RunTillJump
 *          HandleBreakInfo - With
 *      SetBrakpoint(const BankNum bank, const unsigned int address)
 *      SetWatchpoint [no implemented]
 *      CheckBreakInfo  -   With watchpoint
 *      HandleBreakInfo -   Finish operation, operations [normal jump instructions, extended jump instructions]
 *
 ******************************************************************************/

namespace {
BreakNum operator++(BreakNum& breakNum, int) {
    auto num = breakNum;
    breakNum = BreakNum{ static_cast<unsigned int>(breakNum) + 1u };
    return num;
}
}

namespace DebuggerTests {


class BreakpointManagerTests : public ::testing::Test {
public:
    void SetUp() override {
        DebuggerCallback::SetGetPcRegCallback([this]() { return GetPcRegMock(); });
        DebuggerCallback::SetReadMemoryCallback([this](unsigned int address) { return ReadRomMemory(address); });
    }

    void TearDown() override {}

    unsigned int GetPcRegMock() { return m_pc; }
    unsigned int ReadRomMemory(unsigned int /*address*/) { return g_memory; }

    BreakpointManager m_breakpointManager;

    unsigned int m_pc = 0; // TODO: make a gmock interface
    unsigned int g_memory = 0; // TODO: make a gmock interface
};

TEST_F(BreakpointManagerTests, GetBreakpointInfoList_AddBreakCheckInfoDelete) {
    const auto ExpectedAddress = 0x100;

    m_breakpointManager.DeleteBreakpoints();
    auto breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_EQ(breakInfo.size(), 0U);

    const auto breakNum = m_breakpointManager.SetBreakpoint(ExpectedAddress);
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) != breakInfo.end());
    EXPECT_EQ(breakInfo.at(breakNum).address, ExpectedAddress);

    m_breakpointManager.DeleteBreakpoints({ breakNum });
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) == breakInfo.end());
}

TEST_F(BreakpointManagerTests, GetBreakpointInfoList_AddDefaultBreakCheckInfoDelete) {
    m_breakpointManager.DeleteBreakpoints();
    auto breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_EQ(breakInfo.size(), 0U);

    const auto breakNum = m_breakpointManager.SetBreakpoint(m_pc);
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) != breakInfo.end());
    EXPECT_EQ(breakInfo.at(breakNum).address, m_pc);

    m_breakpointManager.DeleteBreakpoints({ breakNum });
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) == breakInfo.end());
}

TEST_F(BreakpointManagerTests, GetBreakpointInfoList_GetSpecificBreakInfo) {
    static constexpr auto address1 = 0x101;
    static constexpr auto address2 = 0x102;
    static constexpr auto address3 = 0x103;

    const auto breakNum1 = m_breakpointManager.SetBreakpoint(address1);
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(address2);
    const auto breakNum3 = m_breakpointManager.SetBreakpoint(address3);

    const auto breakInfo = m_breakpointManager.GetBreakpointInfoList({ breakNum1, breakNum3 });
    EXPECT_TRUE(breakInfo.find(breakNum1) != breakInfo.end());
    EXPECT_TRUE(breakInfo.find(breakNum2) == breakInfo.end());
    EXPECT_TRUE(breakInfo.find(breakNum3) != breakInfo.end());
    EXPECT_EQ(breakInfo.at(breakNum1).address, address1);
    EXPECT_EQ(breakInfo.at(breakNum3).address, address3);
}

TEST_F(BreakpointManagerTests, CheckBreakpoints_HitBreakpoint) {
    BreakInfo breakInfo;
    static constexpr auto address1 = 0x100;
    static constexpr auto address2 = 0x101;
    m_pc = address1;
    m_breakpointManager.SetBreakpoint(address2);

    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    m_pc = address2;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, CheckBreakpoints_DisableReEnableBreakpoint) {
    static constexpr auto address1 = 0x100;

    m_pc = address1;
    const auto breakNum = m_breakpointManager.SetBreakpoint(address1);

    BreakInfo breakInfo;
    m_breakpointManager.EnableBreakpoints({ breakNum });
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.DisableBreakpoints({ breakNum });
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.EnableBreakpoints({ breakNum });
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, EnableBreakpoints_DisableBreakpoints_DisableReEnableBreakpointList) {
    static constexpr auto address1 = 0x100;
    static constexpr auto address2 = 0x101;
    static constexpr auto address3 = 0x102;

    m_pc = address1;
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(address1);
    auto breakNum = breakNum1;
    std::vector<BreakNum> breakList = { breakNum++, breakNum++, breakNum++, breakNum };

    BreakInfo breakInfo;
    // Doesn't break when between 2 breakpoints
    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints(breakList)); // Range of breakpoints works if at least one point is a breakpoint
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Add 2 more breaks
    m_breakpointManager.SetBreakpoint(0);
    const auto breakNum3 = m_breakpointManager.SetBreakpoint(address3);

    m_breakpointManager.DisableBreakpoints(breakList);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    m_pc = address3;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.EnableBreakpoints(breakList);
    m_pc = address1;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo)); // address1
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum1);
    m_pc = address2;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo)); // address2, TODO: should this check from breakInfo being reset?
    m_pc = address3;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo)); // address3
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum3);
}

TEST_F(BreakpointManagerTests, EnableBreakpoints_DisableBreakpoints_DisableReEnableBreakpointMix) {
    static constexpr auto address1 = 0x101;
    static constexpr auto address2 = 0x102;
    static constexpr auto address3 = 0x103;
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(address1);
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(address2);
    const auto breakNum3 = m_breakpointManager.SetBreakpoint(address3);

    std::vector<BreakNum> breakList = { breakNum1, breakNum2, breakNum3 };

    EXPECT_TRUE(m_breakpointManager.DisableBreakpoints(breakList));
    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints({ breakNum2 }));

    BreakInfo breakInfo;
    m_pc = address1;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    m_pc = address2;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum2);
    m_pc = address3;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints(breakList));
    EXPECT_TRUE(m_breakpointManager.DisableBreakpoints({ breakNum2 }));
    m_pc = address1;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum1);
    m_pc = address2;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    m_pc = address3;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum3);
}

TEST_F(BreakpointManagerTests, RunInstructions_CheckBreakpoints_StepOneAndMany) {
    BreakInfo breakInfo;

    m_breakpointManager.RunInstructions();
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.RunInstructions(1);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    static constexpr auto NumInstructionsToSkip = 5;
    m_breakpointManager.RunInstructions(NumInstructionsToSkip);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, CheckBreakpoints_StepBreaksOnBreakpoint) {
    static constexpr auto startAddress = 0x100;
    static constexpr auto address1 = 0x101;
    static constexpr auto address2 = 0x102;
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(address2);
    m_pc = startAddress;

    BreakInfo breakInfo;
    static constexpr auto numInstructionsSkip = 5;
    m_breakpointManager.RunInstructions(numInstructionsSkip);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    m_pc = address1;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    m_pc = address2;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum2);
}

TEST_F(BreakpointManagerTests, Debugger_ContinueBreaksOnBreakpoint) {
    static constexpr auto address1 = 0x100;
    static constexpr auto address2 = 0x101;
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(address2);
    m_pc = address1;

    BreakInfo breakInfo{};
    m_breakpointManager.Run(0);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    m_pc = address2;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum2);

    static constexpr auto numInstructionsSkip = 5;
    m_breakpointManager.Run(numInstructionsSkip);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum2);
}

// TEST_F(BreakpointManagerTests, Debugger_FinishStopsOnJumps) {}

TEST_F(BreakpointManagerTests, Debugger_InfoCheckSize) {
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(0x101);
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(0x101);
    const auto breakNum3 = m_breakpointManager.SetBreakpoint(0x101);

    auto breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_EQ(breakInfo.size(), 3);
    EXPECT_TRUE((breakInfo.find(breakNum1) != breakInfo.end()));
    EXPECT_TRUE((breakInfo.find(breakNum2) != breakInfo.end()));
    EXPECT_TRUE((breakInfo.find(breakNum3) != breakInfo.end()));

    m_breakpointManager.DeleteBreakpoints();
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_EQ(breakInfo.size(), 0);
}

// TEST_F(BreakpointManagerTests, Debugger_ListDiffrentListSizesOfBootRom) {
//     const auto expectedSize = 5;
//     const auto cmds = m_breakpointManager.GetCommandInfoList(0, expectedSize);
//     EXPECT_EQ(cmds.size(), expectedSize);
//
//     for (const auto& cmd : cmds) {
//         const auto readCmd = m_mmu.ReadRom(cmd.address);
//         EXPECT_EQ(cmd.commandOp, readCmd);
//     }
// }

// TEST_F(BreakpointManagerTests, DISABLED_Debugger_JumpContinuesOnAddress) {}

}