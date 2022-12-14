#include "gtest/gtest.h"

#include "RetroDebugger_config.h"

#include <RetroDebuggerApi.h>

/******************************************************************************
 * TODOs
 *
 ******************************************************************************/

namespace IntegrationTests {

class RetroDebuggerIntegrationTests : public ::testing::Test {
protected:
    RetroDebuggerIntegrationTests() {
    }

    ~RetroDebuggerIntegrationTests() override {}

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(RetroDebuggerIntegrationTests, GetCommandPrompt_ValidValue) {
    std::string cmdPrompt;
    GetCommandPrompt(&cmdPrompt);
    ASSERT_EQ(cmdPrompt, std::string("(rdb)"));
}

TEST_F(RetroDebuggerIntegrationTests, GetRdbVersion_ValidValue) {
    std::string version;
    GetRdbVersion(&version);

    ASSERT_EQ(version, RetroDebugger::Config::ProjectVersion);
}

}
