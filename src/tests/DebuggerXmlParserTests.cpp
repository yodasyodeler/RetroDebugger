#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RetroDebuggerTests_assets.h"

#include "DebuggerXmlParser.h"

/******************************************************************************
 * TODOs
 *   Z80Operations_ParseFileTest         -   more than one extended operations
 *   GameboyCommandInfo_ParseFileTest    -   coverage of any other types of xml element archetypes.
 *   EmptyFile_ParseFileTest             -   fails gracefully
 *
 ******************************************************************************/

namespace DebuggerXmlParserTests {

class DebuggerXmlParserTests : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

    DebuggerXmlParser m_xmlParser;
};

TEST_F(DebuggerXmlParserTests, ParseFile_GameboyOperations) {
    const auto GAMEBOY_EXTENDED_OPCODE_KEY = 0xCB;
    const auto expectedGameboyOpcodeSize = 8;
    const auto expectedNumOfOperations = 2U;

    const auto expectedNormalOperationsSize = 244U;
    const auto expectedExtendedOperationsSize = 256U;

    ASSERT_TRUE(m_xmlParser.ParseFile(std::string(RetroDebuggerTests::Assets::GameboyOperationsDebuggerXml))) << m_xmlParser.GetLastError();

    auto operations = m_xmlParser.GetOperations();
    EXPECT_EQ(operations.size(), expectedNumOfOperations);
    EXPECT_NE(operations.find(NormalOperationsKey), operations.end());
    EXPECT_NE(operations.find(GAMEBOY_EXTENDED_OPCODE_KEY), operations.end());

    const auto& actualNormalOperations = operations[NormalOperationsKey];
    EXPECT_EQ(actualNormalOperations.extendedOpcode, NormalOperationsKey);
    EXPECT_EQ(actualNormalOperations.opcodeLength, expectedGameboyOpcodeSize);
    EXPECT_EQ(actualNormalOperations.operations.size(), expectedNormalOperationsSize);

    const auto& actualExtendedOperations = operations[GAMEBOY_EXTENDED_OPCODE_KEY];
    EXPECT_EQ(actualExtendedOperations.extendedOpcode, GAMEBOY_EXTENDED_OPCODE_KEY);
    EXPECT_EQ(actualExtendedOperations.opcodeLength, expectedGameboyOpcodeSize);
    EXPECT_EQ(actualExtendedOperations.operations.size(), expectedExtendedOperationsSize);
}

TEST_F(DebuggerXmlParserTests, ParseFile_ExampleOperations) {
    ASSERT_TRUE(m_xmlParser.ParseFile(std::string(RetroDebuggerTests::Assets::ExampleOperationsXml))) << m_xmlParser.GetLastError(); // Just checking that example operations are valid

    const auto operations = m_xmlParser.GetOperations();
    ASSERT_FALSE(operations.empty());
}

TEST_F(DebuggerXmlParserTests, Invalid_ParseFileTest) {
    ASSERT_FALSE(m_xmlParser.ParseFile(std::string(RetroDebuggerTests::Assets::InvalidEmptyXml)));

    EXPECT_THAT(m_xmlParser.GetLastError(), testing::HasSubstr("couldn't find the first XML element in the file"));
}

TEST_F(DebuggerXmlParserTests, Invalid_MustStartWith_RetroDebuggerElement) {
    const auto* XmlOperations = R"(<operations opcodeLength="8"></operations>)";

    tinyxml2::XMLDocument doc;
    doc.Parse(XmlOperations);
    ASSERT_FALSE(m_xmlParser.ParseXmlDocument(doc));

    const auto errorMsg = m_xmlParser.GetLastError();
    EXPECT_THAT(errorMsg, testing::HasSubstr("Root XML element must be 'RetroDebugger'"));
}

}