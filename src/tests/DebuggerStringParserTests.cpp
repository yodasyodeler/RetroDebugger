#include "gtest/gtest.h"
#include "DebuggerStringParser.h"

TEST(DebuggerStringParser, DebugStringParser_ParseBuffersOfDiffrentSizes) {
    const std::string EmptyString(""); //empty
    const std::string OneWord("OneWord");
    const std::string TwoWord("Two Words");

    EXPECT_EQ(DebuggerStringParser::ParseBuffer(EmptyString).size(), 1);
    EXPECT_EQ(DebuggerStringParser::ParseBuffer(OneWord).size(), 1);
    EXPECT_EQ(DebuggerStringParser::ParseBuffer(TwoWord).size(), 2);
}

TEST(DebuggerStringParser, DebugStringParser_ParseBuffersWithExtraWhiteSpace) {
    const std::string LeadingSpaces("    4LeadingSpaces");
    const std::string TrailingSpaces("4TrailingSpaces    ");
    const std::string ExtraSpaces("Four  Word   Extra    Space");

    const auto ParsedLeadingSpacesOut = DebuggerStringParser::ParseBuffer(LeadingSpaces);
    EXPECT_EQ(ParsedLeadingSpacesOut.size(), 1);
    EXPECT_EQ(ParsedLeadingSpacesOut[0], std::string("4LeadingSpaces"));

    const auto ParsedTrailingSpacesOut = DebuggerStringParser::ParseBuffer(TrailingSpaces);
    EXPECT_EQ(ParsedTrailingSpacesOut.size(), 1);
    EXPECT_EQ(ParsedTrailingSpacesOut[0], std::string("4TrailingSpaces"));

    const auto ParsedExtraSpacesOut = DebuggerStringParser::ParseBuffer(ExtraSpaces);
    EXPECT_EQ(ParsedExtraSpacesOut.size(), 4);
    EXPECT_EQ(ParsedExtraSpacesOut[0], std::string("Four"));
    EXPECT_EQ(ParsedExtraSpacesOut[1], std::string("Word"));
    EXPECT_EQ(ParsedExtraSpacesOut[2], std::string("Extra"));
    EXPECT_EQ(ParsedExtraSpacesOut[3], std::string("Space"));
}

TEST(DebuggerStringParser, DebugStringParser_ParseNumberSingle) {
    const std::string number_1("1");
    const std::string number_9567("9567");
    unsigned int numberOut;

    EXPECT_TRUE(DebuggerStringParser::ParseNumber(number_1, numberOut));
    EXPECT_EQ(numberOut, 1);

    EXPECT_TRUE(DebuggerStringParser::ParseNumber(number_9567, numberOut));
    EXPECT_EQ(numberOut, 9567);
}

TEST(DebuggerStringParser, DebugStringParser_ParseNumberSingleInvalidNum) {
    const std::string number_float("1.0");
    const std::string number_negative("-5");
    const std::string number_notNum("abc");
    const std::string number_startAsNum("1abc");
    const std::string number_endAsNum("abc1");
    const std::string number_empty("");
    const std::string number_numSpaceNum("1 2");

    unsigned int numberOut;

    EXPECT_TRUE(!DebuggerStringParser::ParseNumber(number_float, numberOut));
    EXPECT_TRUE(!DebuggerStringParser::ParseNumber(number_negative, numberOut));
    EXPECT_TRUE(!DebuggerStringParser::ParseNumber(number_notNum, numberOut));
    EXPECT_TRUE(!DebuggerStringParser::ParseNumber(number_startAsNum, numberOut));
    EXPECT_TRUE(!DebuggerStringParser::ParseNumber(number_endAsNum, numberOut));
    EXPECT_TRUE(!DebuggerStringParser::ParseNumber(number_empty, numberOut));
    EXPECT_TRUE(!DebuggerStringParser::ParseNumber(number_numSpaceNum, numberOut));
}

TEST(DebuggerStringParser, DebugStringParser_ParseNumberRange) {
    const std::string number_range("1-2");
    const std::string number_rangeLeadingZeros("001-2");
    const std::string number_rangeLeadingZerossecond("1-002");
    const std::string number_rangeWrongDirection("6-5");


    std::vector<unsigned int> numbers;

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_range, numbers));
    EXPECT_EQ(numbers[0], 1);
    EXPECT_EQ(numbers[1], 2);

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_rangeLeadingZeros, numbers));
    EXPECT_EQ(numbers[0], 1);
    EXPECT_EQ(numbers[1], 2);

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_rangeLeadingZerossecond, numbers));
    EXPECT_EQ(numbers[0], 1);
    EXPECT_EQ(numbers[1], 2);

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_rangeWrongDirection, numbers));
    EXPECT_EQ(numbers.size(), 0);
}

TEST(DebuggerStringParser, DebugStringParser_ParseListInvalidRanges) {
    const std::string number_rangeCharFirst("a-2");
    const std::string number_rangeCharSecond("1-b");
    const std::string number_rangeNegativeRange("1--2");
    const std::string number_rangeOneNum("1-");
    const std::string number_rangeEmptyRange("-");


    std::vector<unsigned int> number;

    EXPECT_TRUE(!DebuggerStringParser::ParseList(number_rangeCharFirst, number));
    EXPECT_TRUE(!DebuggerStringParser::ParseList(number_rangeCharSecond, number));
    EXPECT_TRUE(!DebuggerStringParser::ParseList(number_rangeNegativeRange, number));
    EXPECT_TRUE(!DebuggerStringParser::ParseList(number_rangeOneNum, number));
    EXPECT_TRUE(!DebuggerStringParser::ParseList(number_rangeEmptyRange, number));
}

TEST(DebuggerStringParser, DebugStringParser_ParseNumberDifferentNotations) {
    const std::string number_hex("0x5A");
    const std::string number_binary("0b1010");
    const std::string number_rangeMismatch("0x5-0b111");

    unsigned int number;
    std::vector<unsigned int> numbers;

    EXPECT_TRUE(DebuggerStringParser::ParseNumber(number_hex, number));
    EXPECT_EQ(number, 90);
    EXPECT_TRUE(DebuggerStringParser::ParseNumber(number_binary, number));
    EXPECT_EQ(number, 10);
    EXPECT_TRUE(DebuggerStringParser::ParseList(number_rangeMismatch, numbers));
    EXPECT_EQ(numbers.size(), 3);
    EXPECT_EQ(numbers[0], 5);
    EXPECT_EQ(numbers[1], 6);
    EXPECT_EQ(numbers[2], 7);
}

TEST(DebuggerStringParser, DebugStringParser_ParseComplexLists) {
    const std::string number_list1("0,1,2");
    const std::string number_list2("7,2-4");
    const std::string number_list3("0-1,19,1-2");

    std::vector<unsigned int> numbers;

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_list1, numbers));
    EXPECT_EQ(numbers.size(), 3);
    EXPECT_EQ(numbers[0], 0);
    EXPECT_EQ(numbers[1], 1);
    EXPECT_EQ(numbers[2], 2);

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_list2, numbers));
    EXPECT_EQ(numbers.size(), 4);
    EXPECT_EQ(numbers[0], 7);
    EXPECT_EQ(numbers[1], 2);
    EXPECT_EQ(numbers[2], 3);
    EXPECT_EQ(numbers[3], 4);

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_list3, numbers));
    EXPECT_EQ(numbers.size(), 5);
    EXPECT_EQ(numbers[0], 0);
    EXPECT_EQ(numbers[1], 1);
    EXPECT_EQ(numbers[2], 19);
    EXPECT_EQ(numbers[3], 1);
    EXPECT_EQ(numbers[4], 2);
}

TEST(DebuggerStringParser, DISABLED_DebugStringParser_ParseComplexSubLists) { //TODO: enable if negative numbers to remove items from lists becomes supported
    const std::string number_list0("1,2,-2");
    const std::string number_list1("0-3,-2");
    const std::string number_list2("0-4,-1-3");

    std::vector<unsigned int> numbers;

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_list0, numbers));
    EXPECT_EQ(numbers.size(), 1);
    EXPECT_EQ(numbers[0], 1);

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_list1, numbers));
    EXPECT_EQ(numbers.size(), 3);
    EXPECT_EQ(numbers[0], 0);
    EXPECT_EQ(numbers[1], 1);
    EXPECT_EQ(numbers[2], 3);

    EXPECT_TRUE(DebuggerStringParser::ParseList(number_list2, numbers));
    EXPECT_EQ(numbers.size(), 2);
    EXPECT_EQ(numbers[0], 0);
    EXPECT_EQ(numbers[0], 4);
}