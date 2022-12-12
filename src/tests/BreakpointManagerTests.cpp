#include "BreakpointManager.h"
#include "gtest/gtest.h"

namespace DebuggerTests {
unsigned int g_pc = 0;
unsigned int GetPcRegMock() { return g_pc; }

unsigned int g_memory = 0;
unsigned int ReadRomMemory(unsigned int /*address*/) { return g_memory; }


class BreakpointManagerTests : public ::testing::Test {
protected:
    BreakpointManagerTests() {}

    ~BreakpointManagerTests() override {}

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
    EXPECT_EQ(breakInfo.size(), 0u);

    const auto breakNum = m_breakpointManager.SetBreakpoint({ ExpectedAddress });
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) != breakInfo.end());
    EXPECT_EQ(breakInfo.at(breakNum).address, ExpectedAddress);

    m_breakpointManager.DeleteBreakpoints({ breakNum });
    breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_TRUE(breakInfo.find(breakNum) == breakInfo.end());
}

TEST_F(BreakpointManagerTests, Debugger_AddDefaultBreakCheckInfoDelete) {
    g_pc = 0u;

    m_breakpointManager.DeleteBreakpoints();
    auto breakInfo = m_breakpointManager.GetBreakpointInfoList();
    EXPECT_EQ(breakInfo.size(), 0u);

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
    g_pc = 0x100;
    m_breakpointManager.SetBreakpoint(0x101);

    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x101;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, Debugger_DisableReEnableBreakpoint) {
    BreakInfo breakInfo;
    g_pc = 0x100;
    const auto breakNum = m_breakpointManager.SetBreakpoint(0x100);

    m_breakpointManager.EnableBreakpoints({ breakNum });
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.DisableBreakpoints({ breakNum });
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.EnableBreakpoints({ breakNum });
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, Debugger_DisableReEnableBreakpointList) {
    BreakInfo breakInfo;
    g_pc = 0x100;
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(0x100);
    std::vector<BreakNum> breakList = { breakNum1, breakNum1 + 1, breakNum1 + 2, breakNum1 + 3 };

    //Doesn't break when between 2 breakpoints
    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints(breakList)); //Range of breakpoints works if at least one point is a breakpoint
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    //Add 2 more breaks
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(0x0);
    const auto breakNum3 = m_breakpointManager.SetBreakpoint(0x102);

    m_breakpointManager.DisableBreakpoints(breakList);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x102;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.EnableBreakpoints(breakList);
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo)); //0x100
    g_pc = 0x101;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo)); //0x101
    g_pc = 0x102;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo)); //0x102
}

TEST_F(BreakpointManagerTests, Debugger_DisableReEnableBreakpointMix) {
    BreakInfo breakInfo;
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(0x101);
    const auto breakNum2 = m_breakpointManager.SetBreakpoint(0x102);
    const auto breakNum3 = m_breakpointManager.SetBreakpoint(0x103);

    std::vector<BreakNum> breakList = { breakNum1, breakNum2, breakNum3 };

    EXPECT_TRUE(m_breakpointManager.DisableBreakpoints(breakList));
    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints({ breakNum2 }));

    g_pc = 0x101;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x102;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x103;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));

    EXPECT_TRUE(m_breakpointManager.EnableBreakpoints(breakList));
    EXPECT_TRUE(m_breakpointManager.DisableBreakpoints({ breakNum2 }));
    g_pc = 0x101;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x102;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x103;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, Debugger_StepOneAndMany) {
    BreakInfo breakInfo;

    m_breakpointManager.RunInstructions();
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.RunInstructions(1);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.RunInstructions(5);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, Debugger_StepBreaksOnBreakpoint) {
    BreakInfo breakInfo;
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(0x102);
    g_pc = 0x100;

    m_breakpointManager.RunInstructions(5);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x101;
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x102;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

TEST_F(BreakpointManagerTests, Debugger_ContinueBreaksOnBreakpoint) {
    BreakInfo breakInfo;
    const auto breakNum1 = m_breakpointManager.SetBreakpoint(0x101);
    g_pc = 0x100;

    m_breakpointManager.Run(0);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    g_pc = 0x101;
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));

    m_breakpointManager.Run(5);
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(!m_breakpointManager.CheckBreakpoints(breakInfo));
    EXPECT_TRUE(m_breakpointManager.CheckBreakpoints(breakInfo));
}

//TEST_F(BreakpointManagerTests, Debugger_FinishStopsOnJumps) {}

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

//TEST_F(BreakpointManagerTests, Debugger_ListDiffrentListSizesOfBootRom) {
//    const auto expectedSize = 5;
//    const auto cmds = m_breakpointManager.GetCommandInfoList(0, expectedSize);
//    EXPECT_EQ(cmds.size(), expectedSize);
//
//    for (const auto& cmd : cmds) {
//        const auto readCmd = m_mmu.ReadRom(cmd.address);
//        EXPECT_EQ(cmd.commandOp, readCmd);
//    }
//}

//TEST_F(BreakpointManagerTests, DISABLED_Debugger_JumpContinuesOnAddress) {}

}