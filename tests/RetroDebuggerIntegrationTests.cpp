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
    ASSERT_EQ(Rdb::GetCommandPrompt(), std::string("(rdb)"));
}

TEST_F(RetroDebuggerIntegrationTests, GetRdbVersion_ValidValue) {
    ASSERT_EQ(Rdb::GetRdbVersion(), RetroDebugger::Config::ProjectVersion);
}

}
