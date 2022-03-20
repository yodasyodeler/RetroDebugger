#include "gtest/gtest.h"

#include <DebuggerApi.h>

#include <fmt/core.h>

/******************************************************************************
 * TODOs
 *
 ******************************************************************************/

namespace IntergrationTests {

class RetroDebuggerIntergrationTests : public ::testing::Test {
protected:
    RetroDebuggerIntergrationTests() {
    }

    ~RetroDebuggerIntergrationTests() override {}

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(RetroDebuggerIntergrationTests, GetCommandPrompt_ValidValue) {
    std::string cmdPrompt;
    GetCommandPrompt(&cmdPrompt);
    ASSERT_EQ(cmdPrompt, std::string("(rdb)"));
}

}
