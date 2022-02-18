#include "gtest/gtest.h"

#include <regex>

#include "DebuggerXmlParser.h"

/******************************************************************************
* TODOs
*   Z80Operations_ParseFileTest         -   more than one extended operations
*   GameboyCommandInfo_ParseFileTest    -   coverage of any other types of xml elment archetypes.
*   EmptyFile_ParseFileTest             -   fails gracefully
*
******************************************************************************/


namespace DebuggerXmlParserTests {

const auto GameboyTestFile = "../GameboyOperationsDebugger.xml";
const auto InvalidEmptyTestFile = "../invalid_empty.xml";
const auto ValidBareTestFile = "../valid_bare_min.xml";


class DebuggerXmlParserTests : public ::testing::Test {
protected:
    DebuggerXmlParserTests() {}

    ~DebuggerXmlParserTests() override {}

    void SetUp() override {}

    void TearDown() override {}

    DebuggerXmlParser m_xmlParser;
};

TEST_F(DebuggerXmlParserTests, GameboyOperations_ParseFileTest) {
    const auto GAMEBOY_EXTENDED_OPCODE_KEY = 0xCB;
    const auto expectedGameboyOpcodeSize = 8;
    const auto expectedNumOfOperations = 2u;

    const auto expectedNormalOperationsSize = 244u;
    const auto expectedExtendedOperationsSize = 256u;

    const auto parseFileReturn = m_xmlParser.ParseFile(GameboyTestFile);
    ASSERT_TRUE(parseFileReturn) << m_xmlParser.GetLastError();

    auto operations = m_xmlParser.GetOperations();
    EXPECT_EQ(operations.size(), expectedNumOfOperations);
    EXPECT_NE(operations.find(NORMAL_OPERATIONS_KEY), operations.end());
    EXPECT_NE(operations.find(GAMEBOY_EXTENDED_OPCODE_KEY), operations.end());

    const auto actualNormalOperations = operations[NORMAL_OPERATIONS_KEY];
    EXPECT_EQ(actualNormalOperations.extendedOpcode, NORMAL_OPERATIONS_KEY);
    EXPECT_EQ(actualNormalOperations.opcodeLength, expectedGameboyOpcodeSize);
    EXPECT_EQ(actualNormalOperations.operations.size(), expectedNormalOperationsSize);

    const auto actualExtendedOperations = operations[GAMEBOY_EXTENDED_OPCODE_KEY];
    EXPECT_EQ(actualExtendedOperations.extendedOpcode, GAMEBOY_EXTENDED_OPCODE_KEY);
    EXPECT_EQ(actualExtendedOperations.opcodeLength, expectedGameboyOpcodeSize);
    EXPECT_EQ(actualExtendedOperations.operations.size(), expectedExtendedOperationsSize);
}

TEST_F(DebuggerXmlParserTests, Invalid_ParseFileTest) {
    const std::regex expectedErrorString(".*couldn't find the first XML element in the file");

    const auto parseFileReturn = m_xmlParser.ParseFile(InvalidEmptyTestFile);
    ASSERT_FALSE(parseFileReturn);

    const auto actualErrorMessage = m_xmlParser.GetLastError();
    EXPECT_TRUE(std::regex_match(actualErrorMessage, expectedErrorString)) << m_xmlParser.GetLastError();
}

}