#include <RetroDebuggerApi.h>

#include "RetroDebuggerTests_assets.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

/******************************************************************************
 * TODOs
 *
 ******************************************************************************/

namespace IntegrationTests {

static constexpr auto MessageWhenEnteringDebugLoop = "Break detected\n(rdb) ";
static constexpr auto Prompt = "(rdb)";

class RetroDebuggerIntegrationTests : public ::testing::Test {
protected:
    void SetUp() override { Rdb::DeleteBreakpoints(); }

    void TearDown() override {}

    /// @brief Test function that emulates possible command line usage
    /// @param input
    /// @return The output as an ostream string
    std::ostringstream TestCommandPrompt(std::stringstream& input) {
        const auto prompt = Rdb::GetCommandPrompt();
        BreakInfo breakInfo;
        std::ostringstream output; // <- test substitute for cout

        // Example loop, Loops on input stream
        if (m_enterDebugger || Rdb::CheckBreakpoints(&breakInfo)) {
            if (m_enterDebugger) {
                output << "Break detected\n";
            }
            else if (auto response = Rdb::GetCommandResponse();
                     !response.empty()) {
                output << response << "\n";
            }

            do {
                output << prompt << " ";
                std::string message;
                std::getline(input, message); // <- In actual use this would use 'cin': "std::getline(std::cin, message);"
                if (Rdb::ProcessCommandString(message) == 1) {
                    m_enterDebugger = false;
                    break;
                }

                output << Rdb::GetCommandResponse();

                if (input.eof()) { // <- This is only included for testing purposes to stop testing when the input stream is empty. Omit this entire if statement if using this example.
                    m_enterDebugger = true; // Set 'true' to reenter
                    break;
                }
            } while (true);
        }

        return output;
    }

    const std::string ConsolePrompt = std::string(Prompt) + " ";
    bool m_enterDebugger = true; // When true enter the debug loop, otherwise don't.
};

TEST_F(RetroDebuggerIntegrationTests, GetCommandPrompt_ValidValue) {
    ASSERT_EQ(Rdb::GetCommandPrompt(), std::string(Prompt));
}

TEST_F(RetroDebuggerIntegrationTests, GetRdbVersion_ValidValue) {
    ASSERT_FALSE(Rdb::GetRdbVersion().empty());
}

// This is more of a regression test of the Example than the library.
// No debug event. So don't do anything.
TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_DoesntEnterWhen_enterDebugger_false) {
    m_enterDebugger = false; // Don't enter debug loop.

    std::stringstream input;
    auto output = TestCommandPrompt(input);

    std::string expectedOutput;
    ASSERT_EQ(expectedOutput, output.str());
}

TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_NoInput_DisplaysPrompt) {
    std::stringstream input;
    Rdb::ParseXmlFile(std::string(RetroDebuggerTests::Assets::GameboyOperationsDebuggerXml));
    auto output = TestCommandPrompt(input);

    std::string expectedOutput = MessageWhenEnteringDebugLoop;
    EXPECT_EQ(expectedOutput, output.str());
}

TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_Commandline_SetBreakpoint_GetBreakpointInfo_ReadModifyWrite) {

    //(rdb) b 0x100
    //(rdb) info break
    {
        std::stringstream input;
        input << "b 0x100\ninfo break"; // Not ending with '/n' so GetLine will return immediately on last command
        Rdb::ParseXmlFile(std::string(RetroDebuggerTests::Assets::GameboyOperationsDebuggerXml));
        auto output = TestCommandPrompt(input);

        auto expectedOutput = std::string(MessageWhenEnteringDebugLoop) + ConsolePrompt + // No return, in command prompt it would come from the input.
                              "Num     Type           Disp Enb Address            What\n"
                              "1       Breakpoint     Keep y   0x0000000000000100 \n";
        ASSERT_EQ(expectedOutput, output.str()); // Doing one full check.
    }

    //(rdb) b 0x500A
    //(rdb) disable 1
    //(rdb) i breakpoint
    {
        std::stringstream input;
        input << "b 0x500A\ndisable 1\ni breakpoint"; // Disable breakpoint 1 and enable
        auto output = TestCommandPrompt(input);
        const auto* expectedOutput =
            "Num     Type           Disp Enb Address            What\n"
            "1       Breakpoint     Keep n   0x0000000000000100 \n"
            "2       Breakpoint     Keep y   0x000000000000500A \n";
        EXPECT_THAT(output.str(), testing::HasSubstr(expectedOutput));
    }

    //(rdb) d 1
    //(rdb) info breakpoint
    {
        std::stringstream input;
        input << "d 1\ni breakpoint"; // Disable breakpoint 1 and enable
        auto output = TestCommandPrompt(input);
        const auto* expectedOutput =
            "Num     Type           Disp Enb Address            What\n"
            "2       Breakpoint     Keep y   0x000000000000500A \n";
        EXPECT_THAT(output.str(), testing::HasSubstr(expectedOutput));
    }
}

TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_Commandline_FinishCommand_RunTillJump) {

    static constexpr auto nopOperation = 0x00;
    static constexpr auto jumpOperation = 0xC2;

    Rdb::ParseXmlFile(std::string(RetroDebuggerTests::Assets::GameboyOperationsDebuggerXml));

    unsigned int pc = 0;
    unsigned int memory = nopOperation;
    Rdb::SetGetPcRegCallback([&pc]() { return pc; });
    Rdb::SetReadMemoryCallback([&memory](unsigned int /*value*/) { return memory; });

    std::stringstream input;
    input << "f";
    auto output = TestCommandPrompt(input);
    ASSERT_FALSE(m_enterDebugger);

    input.clear();
    input << "help"; // A command that will not run and leave the debugger. An empty command will reuse the last command, int this case is would run finish.
    TestCommandPrompt(input);
    ASSERT_FALSE(m_enterDebugger);
    TestCommandPrompt(input);
    ASSERT_FALSE(m_enterDebugger);
    TestCommandPrompt(input);
    ASSERT_FALSE(m_enterDebugger);


    memory = jumpOperation;
    output = TestCommandPrompt(input);
    ASSERT_TRUE(m_enterDebugger);
}

TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_DirectCalls_SetBreakpoint_GetBreakpointInfo_ReadModifyWrite) {

    static constexpr auto break1Address = 0x100;
    static constexpr auto break2Address = 0x500A;
    Rdb::ParseXmlFile(std::string(RetroDebuggerTests::Assets::GameboyOperationsDebuggerXml));
    //(rdb) b 0x100
    //(rdb) info break

    auto break1Num = 0U;
    auto break2Num = 0U;
    {
        Rdb::SetBreakpoint(break1Address);
        auto breakInfoList = Rdb::GetBreakpointInfo();
        ASSERT_EQ(breakInfoList.size(), 1U);

        break1Num = breakInfoList.begin()->first;
        EXPECT_EQ(breakInfoList[break1Num].address, break1Address);
        EXPECT_EQ(breakInfoList[break1Num].breakpointNumber, break1Num);
        EXPECT_EQ(breakInfoList[break1Num].isEnabled, true);
        EXPECT_EQ(breakInfoList[break1Num].disp, BreakDisposition::Keep);
        EXPECT_EQ(breakInfoList[break1Num].type, BreakType::Breakpoint);
    }

    //(rdb) b 0x500A
    //(rdb) disable 1
    //(rdb) i breakpoint
    {
        Rdb::SetBreakpoint(break2Address);
        Rdb::DisableBreakpoints(break1Num, break1Num);
        auto breakInfoList = Rdb::GetBreakpointInfo();
        ASSERT_EQ(breakInfoList.size(), 2U);

        if (auto iter = breakInfoList.begin(); iter->first != break1Num) {
            break2Num = iter->first;
        }
        else {
            break2Num = (++iter)->first;
        }

        EXPECT_EQ(breakInfoList[break1Num].address, break1Address);
        EXPECT_EQ(breakInfoList[break1Num].breakpointNumber, break1Num);
        EXPECT_EQ(breakInfoList[break1Num].isEnabled, false);
        EXPECT_EQ(breakInfoList[break1Num].disp, BreakDisposition::Keep);
        EXPECT_EQ(breakInfoList[break1Num].type, BreakType::Breakpoint);

        EXPECT_EQ(breakInfoList[break2Num].address, break2Address);
        EXPECT_EQ(breakInfoList[break2Num].breakpointNumber, break2Num);
        EXPECT_EQ(breakInfoList[break2Num].isEnabled, true);
        EXPECT_EQ(breakInfoList[break2Num].disp, BreakDisposition::Keep);
        EXPECT_EQ(breakInfoList[break2Num].type, BreakType::Breakpoint);
    }

    //(rdb) d 1
    //(rdb) info breakpoint
    {
        Rdb::DeleteBreakpoints(break1Num, break1Num);
        auto breakInfoList = Rdb::GetBreakpointInfo();
        ASSERT_EQ(breakInfoList.size(), 1U);

        EXPECT_EQ(breakInfoList[break2Num].address, break2Address);
        EXPECT_EQ(breakInfoList[break2Num].breakpointNumber, break2Num);
        EXPECT_EQ(breakInfoList[break2Num].isEnabled, true);
        EXPECT_EQ(breakInfoList[break2Num].disp, BreakDisposition::Keep);
        EXPECT_EQ(breakInfoList[break2Num].type, BreakType::Breakpoint);
    }
}

TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_Interpreter_List_WhenCommandsEmpty) {
    // Set up callbacks
    unsigned int pc = 0;
    static constexpr std::array<uint8_t, 10> memoryValues = { 0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C };
    Rdb::SetGetPcRegCallback([&pc]() { return pc; });
    Rdb::SetReadMemoryCallback([](unsigned int value) { return memoryValues[value % memoryValues.size()]; });

    // Fail load a file
    ASSERT_FALSE(Rdb::ParseXmlFile(""));

    // Try to list
    static constexpr auto listCommand = "list";
    ASSERT_EQ(Rdb::ProcessCommandString(listCommand), 0);

    // Check response is as expected
    static constexpr std::string_view expectedResponse =
        "0x0000  49\t  \n0x0001  254\t  \n0x0002  255\t  \n0x0003  175\t  \n0x0004  33\t  \n"
        "0x0005  255\t  \n0x0006  159\t  \n0x0007  50\t  \n0x0008  203\t  \n0x0009  124\t  \n";
    const auto response = Rdb::GetCommandResponse();
    EXPECT_EQ(response, expectedResponse);
}

}