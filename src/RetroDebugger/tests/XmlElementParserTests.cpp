#include "XmlElementParser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <tinyxml2.h>


/******************************************************************************
 * TODOs
 *   ParseXmlElement_UnkownName      -   make sure element with unknown name sets the last error appropriately
 *
 ******************************************************************************/

namespace XmlElementParserTests {

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

static constexpr auto* XmlOperations = R"(<operations opcodeLength="8"></operations>)";
static constexpr auto* XmlOperationsExtended = R"(<operations extended="0xCB" opcodeLength="8"></operations>)";
static constexpr auto* XmlOperationsInvalidOpcodeLength = R"(<operations extended="0xCB" opcodeLength="9"></operations>)";
static constexpr auto* XmlOperationsInvalidNoOpcodeLength = R"(<operations></operations>)";
static constexpr auto* XmlOperationsInvalidExtendedOpcode = R"(<operations extended="0xGG" opcodeLength="8"></operations>)";

static constexpr auto* XmlOperation = R"(<operation opcode="0x40" command="BIT">    <arg value="0"/>    <arg value="B"/>     </operation>)";
static constexpr auto* XmlOperationMinimal = R"(<operation opcode="0x40" command="BIT"/>)";
static constexpr auto* XmlOperationInvalidNoOpcode = R"(<operation command="BIT"/>)";
static constexpr auto* XmlOperationInvalidNoCommand = R"(<operation opcode="0x40"/>)";

class XmlElementParserTests : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override { m_xmlDocument.Clear(); }

    XmlElementParser m_xmlParser;
    tinyxml2::XMLDocument m_xmlDocument;
};

TEST_F(XmlElementParserTests, Operations_ParseOperations) {
    const auto expectedOcodeLength = 8;
    const auto expectedOperationsSize = 0U;

    m_xmlDocument.Parse(XmlOperations);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperations operations;
    m_xmlParser.ParseXmlElement(element, operations);
    EXPECT_EQ(operations.extendedOpcode, NormalOperationsKey);
    EXPECT_EQ(operations.opcodeLength, expectedOcodeLength);
    EXPECT_EQ(operations.operations.size(), expectedOperationsSize);
}

TEST_F(XmlElementParserTests, Operations_ParseExtendedOperations) {
    const auto expectedExtendedOpcode = 0xCBU;
    const auto expectedOcodeLength = 8;
    const auto expectedOperationsSize = 0U;

    m_xmlDocument.Parse(XmlOperationsExtended);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperations operations;
    m_xmlParser.ParseXmlElement(element, operations);
    EXPECT_EQ(operations.extendedOpcode, expectedExtendedOpcode);
    EXPECT_EQ(operations.opcodeLength, expectedOcodeLength);
    EXPECT_EQ(operations.operations.size(), expectedOperationsSize);
}

TEST_F(XmlElementParserTests, Operations_InvalidOpcodeLength) {
    const auto expectedDefaultOpcodeLength = 0U;
    const auto expectedOperationsSize = 0U;

    m_xmlDocument.Parse(XmlOperationsInvalidOpcodeLength);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperations operations;
    ASSERT_THROW_STRING(m_xmlParser.ParseXmlElement(element, operations), "Failed to obtain a valid OperationLength for");
    EXPECT_EQ(operations.extendedOpcode, NormalOperationsKey);
    EXPECT_EQ(operations.opcodeLength, expectedDefaultOpcodeLength);
    EXPECT_EQ(operations.operations.size(), expectedOperationsSize);
}

TEST_F(XmlElementParserTests, Operations_InvalidNoOpcodeLength) {
    m_xmlDocument.Parse(XmlOperationsInvalidNoOpcodeLength);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperations operations;
    ASSERT_THROW_STRING(m_xmlParser.ParseXmlElement(element, operations), "Failed to obtain a valid OperationLength for ");
}

TEST_F(XmlElementParserTests, Operations_InvalidExtendedOpcode) {
    m_xmlDocument.Parse(XmlOperationsInvalidExtendedOpcode);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperations operations;
    ASSERT_THROW_STRING(m_xmlParser.ParseXmlElement(element, operations), "Failed to obtain a valid unsigned int for ");
}

TEST_F(XmlElementParserTests, Operation_ParseOperation) {
    const auto expectedOpcode = 0x40;
    const auto* expectedCommand = "BIT";
    m_xmlDocument.Parse(XmlOperation);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperation operation;
    m_xmlParser.ParseXmlElement(element, operation);

    EXPECT_EQ(operation.opcode, expectedOpcode);
    EXPECT_EQ(operation.command, expectedCommand);
}

TEST_F(XmlElementParserTests, Operation_ParseOperationMinimal) {
    const auto expectedOpcode = 0x40;
    const auto* expectedCommand = "BIT";
    m_xmlDocument.Parse(XmlOperationMinimal);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperation operation;
    m_xmlParser.ParseXmlElement(element, operation);

    EXPECT_EQ(operation.opcode, expectedOpcode);
    EXPECT_EQ(operation.command, expectedCommand);
}

TEST_F(XmlElementParserTests, Operation_InvalidNoOpcode) {
    m_xmlDocument.Parse(XmlOperationInvalidNoOpcode);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperation operation;
    ASSERT_THROW_STRING(m_xmlParser.ParseXmlElement(element, operation), "Failed to obtain a valid unsigned int for ");
}

TEST_F(XmlElementParserTests, Operation_InvalidNoCommand) {

    m_xmlDocument.Parse(XmlOperationInvalidNoCommand);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerOperation operation;
    ASSERT_THROW_STRING(m_xmlParser.ParseXmlElement(element, operation), "Failed to obtain a valid string for ");
}

const auto ArgA = R"(<arg value="A"/>)";
const auto ArgAExplicit = R"(<arg type="Reg" indirect="false" operation="none" reg="A" value="A"/>)";
const auto ArgBcIndir = R"x(<arg value="(BC)"/>)x";
const auto ArgBcIndirExplicit = R"x(<arg type="Reg" indirect="true" operation="none" reg="BC" value="(BC)"/>)x";
const auto ArgDec16 = R"(<arg value="d16"/>)";
const auto ArgDec16Explicit = R"(<arg type="u16Bit" indirect="false" operation="none" value="d16"/>)";
const auto ArgDec8 = R"(<arg value="d8"/>)";
const auto ArgDec8Explicit = R"(<arg type="u8Bit" indirect="false" operation="none" value="d8"/>)";
const auto ArgCondNz = R"(<arg type="cond" value="NZ"/>)";
const auto ArgHlPostIncIndir = R"x(<arg value="(HL+)"/>)x";
const auto ArgHlPostIncIndirExplicit = R"x(<arg type="reg" indirect="true" operation="POSTINC" reg="HL" value="(HL+)"/>)x";
const auto ArgHlPostDec = R"x(<arg value="HL-"/>)x";
const auto ArgHlPreInc = R"x(<arg value="+HL"/>)x";
const auto ArgAddr16 = R"(<arg value="a16"/>)";
const auto ArgConstDecHex = R"(<arg value="0x08"/>)";
const auto ArgConstDecHexExplicit = R"(<arg type="constant" indirect="false" operation="none" offset="0x08" value="0x08"/>)";
const auto ArgSpOffset = R"(<arg value="SP+r8"/>)";
const auto ArgSpOffsetExplicit = R"(<arg type="reg" indirect="false" operation="REG_OFFSET_S8BIT" reg="SP" value="SP+r8"/>)";

TEST_F(XmlElementParserTests, Argument_ArgA) {
    m_xmlDocument.Parse(ArgA);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "A");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgAExplicit) {
    m_xmlDocument.Parse(ArgAExplicit);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "A");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgBcIndir) {
    m_xmlDocument.Parse(ArgBcIndir);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, true);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "(BC)");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgBcIndirExplicit) {
    m_xmlDocument.Parse(ArgBcIndirExplicit);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, true);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "(BC)");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgDec16) {
    m_xmlDocument.Parse(ArgDec16);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::U16BIT);
    EXPECT_EQ(argument.value.name, "d16");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgDec16Explicit) {
    m_xmlDocument.Parse(ArgDec16Explicit);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);


    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::U16BIT);
    EXPECT_EQ(argument.value.name, "d16");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgDec8) {
    m_xmlDocument.Parse(ArgDec8);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);


    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::U8BIT);
    EXPECT_EQ(argument.value.name, "d8");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgDec8Explicit) {
    m_xmlDocument.Parse(ArgDec8Explicit);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);


    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::U8BIT);
    EXPECT_EQ(argument.value.name, "d8");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgCondNz) {
    m_xmlDocument.Parse(ArgCondNz);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::CONDITIONAL);
    EXPECT_EQ(argument.value.name, "NZ");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgPostIncIndir) {
    m_xmlDocument.Parse(ArgHlPostIncIndir);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, true);
    EXPECT_EQ(argument.operation, RegOperationType::POSTINC);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "(HL+)");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgPostIncIndirExplicit) {
    m_xmlDocument.Parse(ArgHlPostIncIndirExplicit);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, true);
    EXPECT_EQ(argument.operation, RegOperationType::POSTINC);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "(HL+)");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgPostDec) {
    m_xmlDocument.Parse(ArgHlPostDec);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::POSTDEC);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "HL-");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgPreInc) {
    m_xmlDocument.Parse(ArgHlPreInc);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::PREINC);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "+HL");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgAddr16) {
    m_xmlDocument.Parse(ArgAddr16);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::U16BIT);
    EXPECT_EQ(argument.value.name, "a16");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgConstDecHex) {
    m_xmlDocument.Parse(ArgConstDecHex);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::CONSTANT);
    EXPECT_EQ(argument.value.name, "0x08");
    EXPECT_TRUE(argument.value.reg.empty());
    static constexpr auto expectedValue = 8U;
    EXPECT_EQ(argument.value.offset, expectedValue);
}

TEST_F(XmlElementParserTests, Argument_ArgConstDecHexExplicit) {
    m_xmlDocument.Parse(ArgConstDecHexExplicit);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::NONE);
    EXPECT_EQ(argument.type, ArgumentType::CONSTANT);
    EXPECT_EQ(argument.value.name, "0x08");
    EXPECT_TRUE(argument.value.reg.empty());
    static constexpr auto expectedValue = 8U;
    EXPECT_EQ(argument.value.offset, expectedValue);
}

TEST_F(XmlElementParserTests, Argument_ArgSpOffset) {
    m_xmlDocument.Parse(ArgSpOffset);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::REG_OFFSET_S8BIT);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "SP+r8");
    EXPECT_EQ(argument.value.reg, "SP");
    EXPECT_EQ(argument.value.offset, 0U);
}

TEST_F(XmlElementParserTests, Argument_ArgSpOffsetExplicit) {
    m_xmlDocument.Parse(ArgSpOffsetExplicit);
    const auto* element = m_xmlDocument.FirstChildElement();

    XmlDebuggerArgument argument;
    m_xmlParser.ParseXmlElement(element, argument);

    EXPECT_EQ(argument.indirectArg, false);
    EXPECT_EQ(argument.operation, RegOperationType::REG_OFFSET_S8BIT);
    EXPECT_EQ(argument.type, ArgumentType::REG);
    EXPECT_EQ(argument.value.name, "SP+r8");
    EXPECT_EQ(argument.value.reg, "SP");
    EXPECT_EQ(argument.value.offset, 0U);
}

}