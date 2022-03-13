#include "gtest/gtest.h"

#include "DebuggerOperations.h"
#include "DebuggerXmlParser.h"

#include "DebuggerCallbacks.h"

#include <array>

/******************************************************************************
 * TODOs
 *
 ******************************************************************************/

namespace DebuggerXmlParserTests {

const auto GameboyTestFile = "../GameboyOperationsDebugger.xml";
const auto InvalidEmptyTestFile = "../invalid_empty.xml";

class DebuggerOperationsTests : public ::testing::Test {
protected:
    DebuggerOperationsTests() {
        m_parser = std::make_shared<DebuggerXmlParser>();
        m_operations = std::make_unique<DebuggerOperations>(m_parser, GameboyTestFile);
    }

    ~DebuggerOperationsTests() override {}

    void SetUp() override {}

    void TearDown() override {}


    std::vector<unsigned int> m_mockMemory;
    ReadMemoryFunc MockReadMemory = [&](unsigned int address) -> unsigned int {
        if (address >= m_mockMemory.size()) { return 0u; }
        return m_mockMemory[address];
    };

    std::shared_ptr<DebuggerXmlParser> m_parser;
    std::unique_ptr<DebuggerOperations> m_operations;
};

TEST_F(DebuggerOperationsTests, GameboyOperations_ParseFile) {
    const auto gbExtOpcode = 0xCBu;
    const auto expectedOpcodeLength = 8;
    const auto expectedOperationsSize = 244u;
    const auto expectedExtendedOperationsSize = 256u;

    ASSERT_TRUE(m_operations->IsValid()) << m_operations->GetErrorMessage();

    const auto operationData = m_operations->GetOperations();
    EXPECT_EQ(operationData.opcodeLength, expectedOpcodeLength);
    EXPECT_EQ(operationData.operations.size(), expectedOperationsSize);
    ASSERT_EQ(operationData.extendedOperations.size(), 1u);
    ASSERT_TRUE(operationData.extendedOperations.find(gbExtOpcode) != operationData.extendedOperations.end());
    EXPECT_EQ(operationData.extendedOperations.at(gbExtOpcode).size(), expectedExtendedOperationsSize);
}

TEST_F(DebuggerOperationsTests, GameboyOperations_NormalOperationArgumentsMatchExpected) {
    const std::array<unsigned int, 2> expectedNop_OpcodeNumArgs = { 0x00, 0 };
    const std::array<unsigned int, 2> expectedIncBC_OpcodeNumArgs = { 0x03, 1 };
    const std::array<unsigned int, 2> expectedld16_OpcodeNumArgs = { 0x56, 2 };
    const std::array<unsigned int, 2> expectedCallFlags_OpcodeNumArgs = { 0xC4, 2 };

    ASSERT_TRUE(m_operations->IsValid()) << m_operations->GetErrorMessage();

    const auto operationData = m_operations->GetOperations();

    EXPECT_EQ(operationData.operations.at(expectedNop_OpcodeNumArgs[0]).arguments.size(), expectedNop_OpcodeNumArgs[1]);
    EXPECT_EQ(operationData.operations.at(expectedIncBC_OpcodeNumArgs[0]).arguments.size(), expectedIncBC_OpcodeNumArgs[1]);
    EXPECT_EQ(operationData.operations.at(expectedld16_OpcodeNumArgs[0]).arguments.size(), expectedld16_OpcodeNumArgs[1]);
    EXPECT_EQ(operationData.operations.at(expectedCallFlags_OpcodeNumArgs[0]).arguments.size(), expectedCallFlags_OpcodeNumArgs[1]);
}

TEST_F(DebuggerOperationsTests, GameboyOperations_ExtendedOperationArgumentsMatchExpected) {
    const auto gbExtOpcode = 0xCBu;
    const std::array<unsigned int, 2> expectedRlcB_OpcodeNumArgs = { 0x00, 1 };
    const std::array<unsigned int, 2> expectedIncSwapHlInder_OpcodeNumArgs = { 0x36, 1 };
    const std::array<unsigned int, 2> expectedSet7A_OpcodeNumArgs = { 0xFF, 2 };

    ASSERT_TRUE(m_operations->IsValid()) << m_operations->GetErrorMessage();

    const auto operationData = m_operations->GetOperations();

    ASSERT_EQ(operationData.extendedOperations.size(), 1u);
    ASSERT_TRUE(operationData.extendedOperations.find(gbExtOpcode) != operationData.extendedOperations.end());
    EXPECT_EQ(operationData.extendedOperations.at(gbExtOpcode).at(expectedRlcB_OpcodeNumArgs[0]).arguments.size(), expectedRlcB_OpcodeNumArgs[1]);
    EXPECT_EQ(operationData.extendedOperations.at(gbExtOpcode).at(expectedIncSwapHlInder_OpcodeNumArgs[0]).arguments.size(), expectedIncSwapHlInder_OpcodeNumArgs[1]);
    EXPECT_EQ(operationData.extendedOperations.at(gbExtOpcode).at(expectedSet7A_OpcodeNumArgs[0]).arguments.size(), expectedSet7A_OpcodeNumArgs[1]);
}

TEST_F(DebuggerOperationsTests, GameboyOperations_GetOperation) {

    ASSERT_TRUE(m_operations->IsValid()) << m_operations->GetErrorMessage();

    m_mockMemory.push_back(0xD0);
    DebuggerCallback::SetReadMemoryCallback(MockReadMemory);
    Operation actualOperation;
    const auto sizeOfOperation = m_operations->GetOperation(0, actualOperation);
    EXPECT_EQ(sizeOfOperation, 1u);
    EXPECT_EQ(actualOperation.info->name, "RET");
    EXPECT_EQ(actualOperation.arguments[0]->indirectArg, false);
    EXPECT_EQ(actualOperation.arguments[0]->name, "NC");
    EXPECT_TRUE(actualOperation.arguments[0]->reg.empty());
    EXPECT_EQ(actualOperation.arguments[0]->operation, RegOperationType::NONE);
    EXPECT_EQ(actualOperation.arguments[0]->type, ArgumentType::CONDITIONAL);
    EXPECT_EQ(actualOperation.arguments[0]->operationValue, 0u);
}

TEST_F(DebuggerOperationsTests, GameboyOperations_GetExtendedOperation) {
    ASSERT_TRUE(m_operations->IsValid()) << m_operations->GetErrorMessage();

    m_mockMemory.push_back(0xCB);
    m_mockMemory.push_back(0x2E);
    DebuggerCallback::SetReadMemoryCallback(MockReadMemory);
    Operation actualOperation;
    const auto sizeOfOperation = m_operations->GetOperation(0, actualOperation);
    EXPECT_EQ(sizeOfOperation, 2u);
    EXPECT_EQ(actualOperation.info->name, "SRA");
    EXPECT_EQ(actualOperation.arguments[0]->indirectArg, true);
    EXPECT_EQ(actualOperation.arguments[0]->name, "(HL)");
    EXPECT_EQ(actualOperation.arguments[0]->reg, "HL");
    EXPECT_EQ(actualOperation.arguments[0]->operation, RegOperationType::NONE);
    EXPECT_EQ(actualOperation.arguments[0]->type, ArgumentType::REG);
    EXPECT_EQ(actualOperation.arguments[0]->operationValue, 0u);
}

}