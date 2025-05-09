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

TEST_F(BreakpointManagerTests, SetWatchpoint_CheckWhenHit_ValuesAreExpected) {
    static constexpr auto expectedValue = 5u;
    g_memory = expectedValue;

    static constexpr auto expectedAddress = 100u;
    auto breakNum = m_breakpointManager.SetWatchpoint(expectedAddress);
    EXPECT_EQ(breakNum, BreakNum{ 1 });
    const auto breakInfoList = m_breakpointManager.GetBreakpointInfoList();

    ASSERT_EQ(breakInfoList.size(), 1ul);
    ASSERT_TRUE(breakInfoList.contains(breakNum));

    auto watchpoint = breakInfoList.at(breakNum);
    EXPECT_EQ(watchpoint.address, expectedAddress);
    EXPECT_EQ(watchpoint.breakpointNumber, breakNum);
    EXPECT_EQ(watchpoint.bankNumber, AnyBank);
    EXPECT_EQ(watchpoint.timesHit, 0u);
    EXPECT_EQ(watchpoint.oldWatchValue, 0u);
    EXPECT_EQ(watchpoint.currentWatchValue, expectedValue);
    EXPECT_EQ(watchpoint.type, BreakType::Watchpoint);
    EXPECT_EQ(watchpoint.disp, BreakDisposition::Keep);
    EXPECT_TRUE(watchpoint.isEnabled);
    EXPECT_TRUE(watchpoint.regName.empty());

    BreakInfo breakInfo{};
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, BreakNum{ std::numeric_limits<unsigned int>::max() });

    static constexpr auto newExpectedValue = 7u;
    g_memory = newExpectedValue;
    m_breakpointManager.WriteMemoryHook(AnyBank, expectedAddress, { std::byte{ newExpectedValue } });

    ASSERT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    ASSERT_EQ(breakInfo.breakpointNumber, breakNum);
    ASSERT_EQ(breakInfo.oldWatchValue, expectedValue);
    ASSERT_EQ(breakInfo.currentWatchValue, newExpectedValue);
    ASSERT_EQ(breakInfo.timesHit, 1u);
}

TEST_F(BreakpointManagerTests, SetReadWatchpoint_CheckWhenHit_ValuesAreExpected) {
    static constexpr auto expectedValue = 5u;
    g_memory = expectedValue;

    static constexpr auto expectedAddress = 100u;
    auto breakNum = m_breakpointManager.SetReadWatchpoint(expectedAddress);
    EXPECT_EQ(breakNum, BreakNum{ 1 });
    const auto breakInfoList = m_breakpointManager.GetBreakpointInfoList();

    ASSERT_EQ(breakInfoList.size(), 1ul);
    ASSERT_TRUE(breakInfoList.contains(breakNum));

    auto watchpoint = breakInfoList.at(breakNum);
    EXPECT_EQ(watchpoint.address, expectedAddress);
    EXPECT_EQ(watchpoint.breakpointNumber, breakNum);
    EXPECT_EQ(watchpoint.bankNumber, AnyBank);
    EXPECT_EQ(watchpoint.timesHit, 0u);
    EXPECT_EQ(watchpoint.oldWatchValue, 0u);
    EXPECT_EQ(watchpoint.currentWatchValue, expectedValue);
    EXPECT_EQ(watchpoint.type, BreakType::ReadWatchpoint);
    EXPECT_EQ(watchpoint.disp, BreakDisposition::Keep);
    EXPECT_TRUE(watchpoint.isEnabled);
    EXPECT_TRUE(watchpoint.regName.empty());

    BreakInfo breakInfo{};
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, BreakNum{ std::numeric_limits<unsigned int>::max() });

    m_breakpointManager.ReadMemoryHook(AnyBank, expectedAddress, { std::byte{ expectedValue } });

    ASSERT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    ASSERT_EQ(breakInfo.breakpointNumber, breakNum);
    ASSERT_EQ(breakInfo.oldWatchValue, expectedValue);
    ASSERT_EQ(breakInfo.currentWatchValue, expectedValue);
    ASSERT_EQ(breakInfo.timesHit, 1u);
    ASSERT_FALSE(breakInfo.externalHit);
}


TEST_F(BreakpointManagerTests, SetWatchpoint_OnlyTriggeredOnWrites) {
    static constexpr auto expectedValue = 5u;
    g_memory = expectedValue;

    static constexpr auto expectedAddress = 100u;
    auto breakNum = m_breakpointManager.SetWatchpoint(expectedAddress);

    // No event - No break
    BreakInfo breakInfo{};
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Read event - No break
    m_breakpointManager.ReadMemoryHook(AnyBank, expectedAddress, { std::byte{ expectedValue } });
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Write event - on address breaks
    m_breakpointManager.WriteMemoryHook(AnyBank, expectedAddress, { std::byte{ expectedValue } });
    ASSERT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Write event - on different address, No break
    m_breakpointManager.WriteMemoryHook(AnyBank, expectedAddress + 1u, { std::byte{ expectedValue } });
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // No event - No break
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, SetReadWatchpoint_OnlyTriggeredOnWrites) {
    static constexpr auto expectedValue = 5u;
    g_memory = expectedValue;

    static constexpr auto expectedAddress = 100u;
    auto breakNum = m_breakpointManager.SetReadWatchpoint(expectedAddress);

    // No event - No break
    BreakInfo breakInfo{};
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Write event - No break
    m_breakpointManager.WriteMemoryHook(AnyBank, expectedAddress, { std::byte{ expectedValue } });
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Read event - on address breaks
    m_breakpointManager.ReadMemoryHook(AnyBank, expectedAddress, { std::byte{ expectedValue } });
    ASSERT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Read event - on different address, No break
    m_breakpointManager.ReadMemoryHook(AnyBank, expectedAddress + 1u, { std::byte{ expectedValue } });
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // No event - No break
    ASSERT_FALSE(m_breakpointManager.CheckBreakpoints(breakInfo));
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