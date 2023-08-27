#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "DebuggerXmlParser.h"
#include "RetroDebuggerTests_assets.h"
#include "XmlParserException.h"

#include <tinyxml2.h>

#include <string>

/******************************************************************************
 * TODOs
 *   Z80Operations_ParseFileTest         -   more than one extended operations
 *   GameboyCommandInfo_ParseFileTest    -   coverage of any other types of xml element archetypes.
 *   EmptyFile_ParseFileTest             -   fails gracefully
 *
 ******************************************************************************/

#define ASSERT_THROW_STRING(test, str)                                              \
    [&](const std::string& errorSubstring) {                                        \
        try {                                                                       \
            test;                                                                   \
            FAIL() << "Expected exception";                                         \
        }                                                                           \
        catch (const std::exception& e) {                                           \
            EXPECT_THAT(std::string(e.what()), testing::HasSubstr(errorSubstring)); \
        }                                                                           \
    }(str);

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

    m_xmlParser.ParseFile(std::string(RetroDebuggerTests::Assets::GameboyOperationsDebuggerXml));

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
    m_xmlParser.ParseFile(std::string(RetroDebuggerTests::Assets::ExampleOperationsXml));

    const auto operations = m_xmlParser.GetOperations();
    ASSERT_FALSE(operations.empty());
}

TEST_F(DebuggerXmlParserTests, Invalid_ParseFileTest) {
    ASSERT_THROW_STRING(m_xmlParser.ParseFile(std::string(RetroDebuggerTests::Assets::InvalidEmptyXml)), "couldn't find the first XML element in the file");
}

TEST_F(DebuggerXmlParserTests, Invalid_MustStartWith_RetroDebuggerElement) {
    const auto* XmlOperations = R"(<operations opcodeLength="8"></operations>)";

    tinyxml2::XMLDocument doc;
    doc.Parse(XmlOperations);

    ASSERT_THROW_STRING(m_xmlParser.ParseXmlDocument(doc), "Root XML element must be 'RetroDebugger'");
}

}