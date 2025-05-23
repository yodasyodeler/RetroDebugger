#include "ConditionInterpreter.h"

#include "BreakpointManager.h"
#include "MockDebuggerCallbacks.h"

#include <fmt/core.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"


using namespace testing;

class ConditionInterpreterTests : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

    std::shared_ptr<Rdb::MockDebuggerCallbacks> m_callbacks = std::make_shared<Rdb::MockDebuggerCallbacks>();
    Rdb::BreakpointManager m_breakpointManager{ std::shared_ptr<Rdb::DebuggerOperations>{}, m_callbacks };


    unsigned int m_pc = 0; // TODO: make a gmock interface
    unsigned int g_memory = 0; // TODO: make a gmock interface
};

TEST_F(ConditionInterpreterTests, EmptyCondition_ReturnsNullptr) {
    ASSERT_EQ(Rdb::ConditionInterpreter::CreateCondition(m_callbacks, ""), nullptr);
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

TEST_F(ConditionInterpreterTests, SimpleConditions_CheckRegisterValue_HappyPath) {
    static constexpr std::string_view registerA = "RegisterA";
    static constexpr auto testValue = 5;
    RegSet regSet = { { std::string(registerA), 0 } };
    EXPECT_CALL(*m_callbacks, GetRegSet).Times(1).WillRepeatedly(Return(regSet));

    const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, fmt::format("{} == 5", registerA));
    EXPECT_FALSE(condition->EvaluateCondition());

    regSet[std::string(registerA)] = testValue;
    EXPECT_CALL(*m_callbacks, GetRegSet).Times(1).WillRepeatedly(Return(regSet));
    EXPECT_TRUE(condition->EvaluateCondition());
}


TEST_F(ConditionInterpreterTests, SimpleConditions_CheckAddress_HappyPath) {
    auto testMemory = 0;
    EXPECT_CALL(*m_callbacks, ReadMemory(0x100)).Times(1).WillRepeatedly(Return(testMemory));

    const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, "*0x100 == 5");
    EXPECT_FALSE(condition->EvaluateCondition());

    testMemory = 5;
    EXPECT_CALL(*m_callbacks, ReadMemory(0x100)).Times(1).WillRepeatedly(Return(testMemory));
    EXPECT_TRUE(condition->EvaluateCondition());
}

TEST_F(ConditionInterpreterTests, SimpleConditions_CheckBankAddress_HappyPath) {
    auto testMemory = 0;
    EXPECT_CALL(*m_callbacks, ReadBankableMemory(BankNum{ 1u }, 100)).Times(1).WillRepeatedly(Return(testMemory));

    const auto condition = Rdb::ConditionInterpreter::CreateCondition(m_callbacks, "*(1:100) == 5");
    EXPECT_FALSE(condition->EvaluateCondition());

    testMemory = 5;
    EXPECT_CALL(*m_callbacks, ReadBankableMemory(BankNum{ 1u }, 100)).Times(1).WillRepeatedly(Return(testMemory));
    EXPECT_TRUE(condition->EvaluateCondition());
}