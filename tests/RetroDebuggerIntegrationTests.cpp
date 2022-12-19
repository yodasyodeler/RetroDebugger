#include <gtest/gtest.h>

#include "RetroDebuggerTests_assets.h"
#include "RetroDebugger_config.h"

#include <RetroDebuggerApi.h>

/******************************************************************************
 * TODOs
 *
 ******************************************************************************/

namespace IntegrationTests {

class RetroDebuggerIntegrationTests : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}

    /// @brief Test function that emulates a possible usage of the
    /// @param input
    /// @return The output as an ostream string
    std::ostringstream TestCommandPrompt(std::istringstream& input) {
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
                output << prompt;
                std::string message;
                std::getline(input, message); // <- In actual use this would use 'cin': "std::getline(std::cin, message);"
                if (Rdb::ProcessCommandString(message) == 1) { break; }

                output << Rdb::GetCommandResponse();

                if (input.eof()) { break; } // <- This is only included for testing purposes to stop testing when the input stream is empty. Omit this line if using this example.
            } while (true);
        }

        return output;
    }

private:
    bool m_enterDebugger = true; // Enter
};

TEST_F(RetroDebuggerIntegrationTests, GetCommandPrompt_ValidValue) {
    ASSERT_EQ(Rdb::GetCommandPrompt(), std::string("(rdb)"));
}

TEST_F(RetroDebuggerIntegrationTests, GetRdbVersion_ValidValue) {
    ASSERT_EQ(Rdb::GetRdbVersion(), RetroDebugger::Config::ProjectVersion);
}

TEST_F(RetroDebuggerIntegrationTests, IntegrationTest_NoInput_DisplaysPrompt) {
    std::istringstream input;
    Rdb::ParseXmlFile(std::string(RetroDebuggerTests::Assets::GameboyOperationsDebuggerXml));
    auto output = TestCommandPrompt(input);

    std::string expectedOutput = "Break detected\n(rdb)";
}

}
