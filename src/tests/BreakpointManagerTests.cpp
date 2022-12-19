#include "BreakpointManager.h"
#include "gtest/gtest.h"

namespace DebuggerTests {
unsigned int g_pc = 0; // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - ignoring test code for now. TODO: make a gmock interface
unsigned int GetPcRegMock() { return g_pc; }

unsigned int g_memory = 0; // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - ignoring test code for now. TODO: make a gmock interface
unsigned int ReadRomMemory(unsigned int /*address*/) { return g_memory; }


class BreakpointManagerTests : public ::testing::Test {
public:
    void SetUp() override {
        DebuggerCallback::SetGetPcRegCallback(GetPcRegMock);
        DebuggerCallback::SetReadMemoryCallback(ReadRomMemory);
    }

    void TearDown() override {}

    BreakpointManager m_breakpointManager;
};

TEST_F(BreakpointManagerTests, Debugger_AddBreakCheckInfoDelete) {
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

TEST_F(BreakpointManagerTests, Debugger_AddDefaultBreakCheckInfoDelete) {
    g_pc = 0U;

    m_breakpointManager.DeleteBreakpoints();
    auto breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_EQ(breakInfo.size(), 0U);

    const auto breakNum = m_breakpointManager.SetBreakpoint(g_pc);
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) != breakInfo.end());
    EXPECT_EQ(breakInfo.at(breakNum).address, g_pc);

    m_breakpointManager.DeleteBreakpoints({ breakNum });
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) == breakInfo.end());
}

TEST_F(BreakpointManagerTests, Debugger_HitBreakpoint) {
    BreakInfo breakInfo;
    static constexpr auto address1 = 0x100;
    static constexpr auto address2 = 0x101;
    g_pc = address1;
    m_breakpointManager.SetBreakpoint(address2);

    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = address2;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, Debugger_DisableReEnableBreakpoint) {
    static constexpr auto address1 = 0x100;

    g_pc = address1;
    const auto breakNum = m_breakpointManager.SetBreakpoint(address1);

    BreakInfo breakInfo;
    m_breakpointManager.EnableBreakpoints({ breakNum });
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.DisableBreakpoints({ breakNum });
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.EnableBreakpoints({ breakNum });
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, Debugger_DisableReEnableBreakpointList) {
    static constexpr auto address1 = 0x100;
    static constexpr auto address2 = 0x101;
    static constexpr auto address3 = 0x102;

    g_pc = address1;
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(address1);
    std::vector<BreakNum> breakList = { breakNum1, breakNum1 + 1, breakNum1 + 2, breakNum1 + 3 };

    BreakInfo breakInfo;
    // Doesn't break when between 2 breakpoints
    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints(breakList)); // Range of breakpoints works if at least one point is a breakpoint
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    // Add 2 more breaks
    m_breakpointManager.SetBreakpoint(0);
    const auto breakNum3 = m_breakpointManager.SetBreakpoint(address3);

    m_breakpointManager.DisableBreakpoints(breakList);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = address3;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.EnableBreakpoints(breakList);
    g_pc = address1;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo)); // address1
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum1);
    g_pc = address2;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo)); // address2, TODO: should this check from breakInfo being reset?
    g_pc = address3;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo)); // address3
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum3);
}

TEST_F(BreakpointManagerTests, Debugger_DisableReEnableBreakpointMix) {
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
    g_pc = address1;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = address2;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum2);
    g_pc = address3;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints(breakList));
    EXPECT_TRUE(m_breakpointManager.DisableBreakpoints({ breakNum2 }));
    g_pc = address1;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum1);
    g_pc = address2;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = address3;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum3);
}

TEST_F(BreakpointManagerTests, Debugger_StepOneAndMany) {
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

TEST_F(BreakpointManagerTests, Debugger_StepBreaksOnBreakpoint) {
    static constexpr auto startAddress = 0x100;
    static constexpr auto address1 = 0x101;
    static constexpr auto address2 = 0x102;
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(address2);
    g_pc = startAddress;

    BreakInfo breakInfo;
    static constexpr auto numInstructionsSkip = 5;
    m_breakpointManager.RunInstructions(numInstructionsSkip);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = address1;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = address2;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_EQ(breakInfo.breakpointNumber, breakNum2);
}

TEST_F(BreakpointManagerTests, Debugger_ContinueBreaksOnBreakpoint) {
    static constexpr auto address1 = 0x100;
    static constexpr auto address2 = 0x101;
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(address2);
    g_pc = address1;

    BreakInfo breakInfo{};
    m_breakpointManager.Run(0);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = address2;
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