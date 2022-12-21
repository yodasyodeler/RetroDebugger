#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RetroDebuggerTests_assets.h"
#include "RetroDebugger_config.h"

#include <RetroDebuggerApi.h>

/******************************************************************************
 * TODOs
 *
 ******************************************************************************/

namespace IntegrationTests {

static constexpr auto MessageWhenEnteringDebugLoop = "Break detected\n(rdb) ";
static constexpr auto Prompt = "(rdb)";

class RetroDebuggerIntegrationTests : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}

    /// @brief Test function that emulates a possible usage of the
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
                m_enterDebugger = false;
                output << prompt << " ";
                std::string message;
                std::getline(input, message); // <- In actual use this would use 'cin': "std::getline(std::cin, message);"
                if (Rdb::ProcessCommandString(message) == 1) { break; }

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
    ASSERT_EQ(Rdb::GetRdbVersion(), RetroDebugger::Config::ProjectVersion);
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

TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_SetBreakpoint_GetBreakpointInfo_ReadModifyWrite) {

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
        auto expectedOutput =
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
        auto expectedOutput =
            "Num     Type           Disp Enb Address            What\n"
            "2       Breakpoint     Keep y   0x000000000000500A \n";
        EXPECT_THAT(output.str(), testing::HasSubstr(expectedOutput));
    }
}

}
