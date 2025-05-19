#include "ConditionInterpreter.h"

#include "BreakpointManager.h"
#include "MockDebuggerCallbacks.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


class ConditionInterpreterTests : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

    std::shared_ptr<Rdb::MockDebuggerCallbacks> m_callbacks = std::make_shared<Rdb::MockDebuggerCallbacks>();
    Rdb::BreakpointManager m_breakpointManager{ std::shared_ptr<Rdb::DebuggerOperations>{}, m_callbacks };


    unsigned int m_pc = 0; // TODO: make a gmock interface
    unsigned int g_memory = 0; // TODO: make a gmock interface
};

TEST_F(ConditionInterpreterTests, EmptyCondition_Throws) {
    ASSERT_ANY_THROW(Rdb::ConditionInterpreter::CreateCondition(m_callbacks, ""));
}

TEST_F(ConditionInterpreterTests, SimpleConditions_StaticValues_HappyPath) {
    // TRUE
    {
        const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, "5 == 5");
        EXPECT_TRUE(condition->EvaluateCondition());
    }

    // FALSE
    {
        const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, "10 == 5");
        EXPECT_FALSE(condition->EvaluateCondition());
    }

    // TRUE, a bit more complex
    {
        const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, "5 == 5 && (7 & 5) == 5");
        EXPECT_TRUE(condition->EvaluateCondition());
    }

    // Test Truthy evaluation
    {
        const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, "9");
        EXPECT_TRUE(condition->EvaluateCondition());
    }

    // Test Truthy evaluation
    {
        const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, "0");
        EXPECT_FALSE(condition->EvaluateCondition());
    }
}