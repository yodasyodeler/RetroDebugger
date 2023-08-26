#include "XmlElementParser.h"

#include <fmt/core.h>

#include <algorithm>
#include <string_view>

static constexpr std::string_view ErrorNullptr = "encountered unexpected nullptr";
static constexpr std::string_view ErrorUnexpectedElement = "Found an unexpected element. Looking for ";
static constexpr std::string_view ErrorObtainingUnsignedInt = "Failed to obtain a valid unsigned int for ";
static constexpr std::string_view ErrorObtainingOpcodeLength = "Failed to obtain a valid OperationLength for ";
static constexpr std::string_view ErrorObtainingString = "Failed to obtain a valid string for ";
static constexpr std::string_view ErrorObtainingBool = "Failed to obtain a valid bool for ";
static constexpr std::string_view ErrorObtainingOperation = "Failed to obtain a valid reg operation for ";
static constexpr std::string_view ErrorDeterminingArgumentType = "Failed to determine argument type for ";

static constexpr auto ByteOpcodeLength = 8;
static constexpr auto WordOpcodeLength = 16;
static constexpr auto DoubleWordOpcodeLength = 32;

bool ParseArgOperations(std::string& name, XmlDebuggerArgument& arg);

/// @brief Checks the name string is a valid Register offset operation.
/// @param name         String containing the register offset operation.
/// @param operationPos Position of the operation character in the 'name' string.
/// @return  Was reg offset valid. False: (Error, True: Success)
bool CheckRegOffset(std::string& name, XmlDebuggerArgument& arg, size_t operationPos);

std::string ToUpperString(std::string_view str) {
    std::string upperString(str);
    std::ranges::transform(upperString, upperString.begin(), [](unsigned char character) { return static_cast<char>(std::toupper(character)); });
    return upperString;
};

// local helper methods
// TODO: could move to a common file, probably keep here unless code is needed else where. Should check the text formatting code
static bool StringNCompare(const std::string_view& string1, const std::string_view& string2) {
    if (string1.size() != string2.size()) { return false; }

    return ToUpperString(string1) == ToUpperString(string2);
}


/// @brief Converts a string holding a numeric value to an unsigned int.
/// Converts a string holding a numeric value to an unsigned int.
/// Supports hex(0x prefix), octal(0 prefix), and decimal. TODO: should this support binary?
/// If 'str' holds a character that is not numeric or a prefix for a numeric value it is considered not numeric.
/// @param       str    String holding a numeric value.
/// @param [out] value  Numeric value held by the string.
/// @return Boolean of whether the string was a numeric value(true) or not(false).
static bool StringToUnsigned(const std::string& str, unsigned int& value) {
    try {
        size_t pos{};
        value = static_cast<unsigned int>(std::stoul(str, &pos, 0));
        return pos == str.size(); // If numeric, stoul would have gone over every character.
    }
    catch (...) {
        return false;
    }
}

static bool StringToBool(const std::string_view& str, bool& value) {
    if (StringNCompare(str, "TRUE")) {
        value = true;
        return true;
    }
    if (StringNCompare(str, "FALSE")) {
        value = false;
        return true;
    }
    return false;
}

static bool StringToRegOperation(const std::string_view& str, RegOperationType& operation) {
    if (StringNCompare(str, "NONE")) {
        operation = RegOperationType::NONE;
        return true;
    }
    if (StringNCompare(str, "POSTINC")) {
        operation = RegOperationType::POSTINC;
        return true;
    }
    if (StringNCompare(str, "POSTDEC")) {
        operation = RegOperationType::POSTDEC;
        return true;
    }
    if (StringNCompare(str, "PREINC")) {
        operation = RegOperationType::PREINC;
        return true;
    }
    if (StringNCompare(str, "PREDEC")) {
        operation = RegOperationType::PREDEC;
        return true;
    }
    if (StringNCompare(str, "REG_OFFSET_ADD")) {
        operation = RegOperationType::REG_OFFSET_ADD;
        return true;
    }
    if (StringNCompare(str, "REG_OFFSET_SUB")) {
        operation = RegOperationType::REG_OFFSET_SUB;
        return true;
    }
    if (StringNCompare(str, "REG_OFFSET_U8BIT")) {
        operation = RegOperationType::REG_OFFSET_U8BIT;
        return true;
    }
    if (StringNCompare(str, "REG_OFFSET_U16BIT")) {
        operation = RegOperationType::REG_OFFSET_U16BIT;
        return true;
    }
    if (StringNCompare(str, "REG_OFFSET_S8BIT")) {
        operation = RegOperationType::REG_OFFSET_S8BIT;
        return true;
    }
    if (StringNCompare(str, "REG_OFFSET_S16BIT")) {
        operation = RegOperationType::REG_OFFSET_S16BIT;
        return true;
    }
    return false;
}

static bool IsAlphaString(const std::string_view& string) {
    return std::ranges::all_of(string, [](char character) { return std::isalpha(character); });
}

static bool StringToImmediateArgType(const std::string_view& str, ArgumentType& type) {
    if (StringNCompare(str, "R8")) {
        type = ArgumentType::S8BIT;
        return true;
    }
    if (StringNCompare(str, "S8BIT")) {
        type = ArgumentType::S8BIT;
        return true;
    }
    if (StringNCompare(str, "SIGNED8BIT")) {
        type = ArgumentType::S8BIT;
        return true;
    }
    if (StringNCompare(str, "R16")) {
        type = ArgumentType::S16BIT;
        return true;
    }
    if (StringNCompare(str, "S16BIT")) {
        type = ArgumentType::S16BIT;
        return true;
    }
    if (StringNCompare(str, "SIGNED16BIT")) {
        type = ArgumentType::S16BIT;
        return true;
    }
    if (StringNCompare(str, "D8")) {
        type = ArgumentType::U8BIT;
        return true;
    }
    if (StringNCompare(str, "U8")) {
        type = ArgumentType::U8BIT;
        return true;
    }
    if (StringNCompare(str, "A8")) {
        type = ArgumentType::U8BIT;
        return true;
    }
    if (StringNCompare(str, "U8BIT")) {
        type = ArgumentType::U8BIT;
        return true;
    }
    if (StringNCompare(str, "UNSIGNED8BIT")) {
        type = ArgumentType::U8BIT;
        return true;
    }
    if (StringNCompare(str, "D16")) {
        type = ArgumentType::U16BIT;
        return true;
    }
    if (StringNCompare(str, "U16")) {
        type = ArgumentType::U16BIT;
        return true;
    }
    if (StringNCompare(str, "A16")) {
        type = ArgumentType::U16BIT;
        return true;
    }
    if (StringNCompare(str, "U16BIT")) {
        type = ArgumentType::U16BIT;
        return true;
    }
    if (StringNCompare(str, "UNSIGNED16BIT")) {
        type = ArgumentType::U16BIT;
        return true;
    }
    return false;
}

static bool StringToArgType(const std::string_view& str, ArgumentType& type) {
    if (StringNCompare(str, "REG")) {
        type = ArgumentType::REG;
        return true;
    }
    if (StringNCompare(str, "CONST")) {
        type = ArgumentType::CONSTANT;
        return true;
    }
    if (StringNCompare(str, "CONSTANT")) {
        type = ArgumentType::CONSTANT;
        return true;
    }
    if (StringNCompare(str, "COND")) {
        type = ArgumentType::CONDITIONAL;
        return true;
    }
    if (StringNCompare(str, "CONDITIONAL")) {
        type = ArgumentType::CONDITIONAL;
        return true;
    }

    return StringToImmediateArgType(str, type);
}

static bool StringToOpcodeLength(const std::string_view& str, OpcodeLength& opcodeLength) {
    if (StringNCompare("8", str) || StringNCompare("BYTE", str)) {
        opcodeLength = ByteOpcodeLength;
        return true;
    }
    if (StringNCompare("16", str)) {
        opcodeLength = WordOpcodeLength;
        return true;
    }
    if (StringNCompare("32", str)) {
        opcodeLength = DoubleWordOpcodeLength;
        return true;
    }
    return false;
}

static bool ParseAutoArgType(XmlDebuggerArgument& argument) {
    argument.operation = RegOperationType::NONE;
    argument.type = ArgumentType::UNKOWN;
    argument.indirectArg = false;
    auto name = argument.value.name;

    // Paren check
    if (name.front() == '(' && name.back() == ')') {
        if (name.size() <= 2) { return false; }
        name.erase(0, 1);
        name.erase(name.size() - 1);
        argument.indirectArg = true;
    }
    if (name.find('(') != std::string::npos || name.find(')') != std::string::npos) { return false; }


    // Reg is the most common case, check directly after immediate check
    if (IsAlphaString(name)) {
        argument.type = ArgumentType::REG;
        argument.value.reg = name;
        return true;
    }

    // Constant value
    if (StringToUnsigned(name, argument.value.offset)) {
        argument.type = ArgumentType::CONSTANT;
        return true;
    }

    // Check if an immediate type
    if (StringToImmediateArgType(name, argument.type)) { return true; }

    // Check for an operation
    if (ParseArgOperations(name, argument)) {
        argument.type = ArgumentType::REG;
        argument.value.reg = name;
        return true;
    }

    return false;
}

bool ParseArgOperations(std::string& name, XmlDebuggerArgument& arg) {
    static constexpr std::string_view operationChars = "+-";
    auto operationPos = name.find_first_of(operationChars);
    if (operationPos == std::string::npos) {
        arg.operation = RegOperationType::NONE;
        return false;
    }

    if (operationPos == 0) { // PreOp
        if (name[operationPos] == '+' && IsAlphaString(name.substr(1))) { arg.operation = RegOperationType::PREINC; }
        else if (name[operationPos] == '-') {
            arg.operation = RegOperationType::PREDEC;
        }
        else {
            arg.operation = RegOperationType::NONE;
            return false;
        }
        name.erase(0, 1);
    }
    else if (operationPos == name.size() - 1) { // PostOp
        if (name[operationPos] == '+') { arg.operation = RegOperationType::POSTINC; }
        else if (name[operationPos] == '-') {
            arg.operation = RegOperationType::POSTDEC;
        }
        else {
            arg.operation = RegOperationType::NONE;
            return false;
        }
        name.erase(name.size() - 1);
    }
    else { // Check if reg offset
        if (!CheckRegOffset(name, arg, operationPos)) {
            return false;
        }
    }

    const auto reservedCharacters = std::string(operationChars) + "()";
    if (name.find(reservedCharacters) != std::string::npos) {
        arg.operation = RegOperationType::NONE;
        return false;
    }

    return true;
}

bool CheckRegOffset(std::string& name, XmlDebuggerArgument& arg, size_t operationPos) {
    const auto firstParam = name.substr(0, operationPos);
    const auto secondParam = name.substr(operationPos + 1);
    if (IsAlphaString(firstParam) && StringToUnsigned(secondParam, arg.value.offset)) { // TODO: is there a unit test for this?
        name = firstParam;
        arg.operation = (name[operationPos] == '+') ? RegOperationType::REG_OFFSET_ADD : RegOperationType::REG_OFFSET_SUB; // TODO: add error checking for operation
    }
    else if (auto immediateType = ArgumentType::UNKOWN; IsAlphaString(firstParam) && StringToImmediateArgType(secondParam, immediateType)) { // TODO: Look at adding support for REG+REG, REG+S8BIT, REG+S16BIT,
        if (immediateType == ArgumentType::U8BIT) { arg.operation = RegOperationType::REG_OFFSET_U8BIT; }
        else if (immediateType == ArgumentType::U16BIT) {
            arg.operation = RegOperationType::REG_OFFSET_U16BIT;
        }
        else if (immediateType == ArgumentType::S8BIT) {
            arg.operation = RegOperationType::REG_OFFSET_S8BIT;
        }
        else if (immediateType == ArgumentType::S16BIT) {
            arg.operation = RegOperationType::REG_OFFSET_S16BIT;
        }
        else {
            return false;
        }
        name = firstParam;
    }
    // else if (StringToUnsigned(firstParam, arg.value.offset) && IsAlphaString(secondParam)) {
    //     //arg.value.name = secondParam;
    //     arg.operation = (value[operationFind] == '+') ? RegOperationType::REG_OFFSET_ADD : RegOperationType::REG_OFFSET_SUB;
    // }
    else {
        arg.operation = RegOperationType::NONE;
        return false;
    }
    return true;
}

std::string GetTextAsString(const tinyxml2::XMLAttribute* attribute) {
    const auto* textPtr = attribute->Value();
    return (textPtr == nullptr) ? "" : std::string(textPtr);
}

// FlagType StringToFlagType(const std::string_view& flagType) {
//     if (StringNCompare("", flagType))           return FlagType::NONE;
//     if (StringNCompare("NONE", flagType))       return FlagType::NONE;
//     if (StringNCompare("ZERO", flagType))       return FlagType::ZERO;
//     if (StringNCompare("SUBTRACT", flagType))   return FlagType::SUBTRACT;
//     if (StringNCompare("HALF_CARRY", flagType)) return FlagType::HALF_CARRY;
//     if (StringNCompare("CARRY", flagType))      return FlagType::CARRY;
//
//     return FlagType::UNKOWN;
// }

// Exposed Methods
std::string XmlElementParser::GetLastError() const {
    return m_lastError;
}

bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperations& operations) {
    operations.Reset();

    static constexpr auto OperationsStr = "operations";
    if (element == nullptr) { return SetLastError(OperationsStr, ErrorNullptr); }
    if (!StringNCompare(element->Name(), "operations")) { return SetLastError(element, std::string(ErrorUnexpectedElement) + OperationsStr); }

    const char* opcodeLengthStr = "opcodeLength";
    const char* str = element->Attribute(opcodeLengthStr);
    if ((str == nullptr) || !StringToOpcodeLength(str, operations.opcodeLength)) { return SetLastError(element, std::string(ErrorObtainingOpcodeLength) + opcodeLengthStr); }

    const char* extenededOpcodeStr = "extended";
    str = element->Attribute(extenededOpcodeStr);
    if (str == nullptr) { operations.extendedOpcode = NormalOperationsKey; }
    else if (!StringToUnsigned(str, operations.extendedOpcode)) {
        return SetLastError(element, std::string(ErrorObtainingUnsignedInt) + extenededOpcodeStr);
    }

    return true;
}

bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperation& operation) {
    operation.Reset();

    if (static constexpr auto OperationStr = "operation";
        element == nullptr) { return SetLastError(std::string(OperationStr), std::string(ErrorNullptr)); }
    else if (!StringNCompare(element->Name(), OperationStr)) { return SetLastError(element, std::string(ErrorUnexpectedElement) + OperationStr); }

    if (const char *opcodeStr = "opcode", *str = element->Attribute(opcodeStr);
        str == nullptr || !StringToUnsigned(str, operation.opcode)) { return SetLastError(element, std::string(ErrorObtainingUnsignedInt) + opcodeStr); }

    if (const char *commandStr = "command", *str = element->Attribute(commandStr);
        str == nullptr) { return SetLastError(element, std::string(ErrorObtainingString) + commandStr); }
    else {
        operation.command = str;
    }

    if (const auto* str = element->Attribute("isJump");
        str != nullptr) {
        constexpr auto trueStr = "true";
        operation.isJump = ToUpperString(str) == ToUpperString(trueStr);
    }

    return true;
}

// R"(<arg type="Reg" indirect="false" operation="none" reg="A" value="A"/>)";
bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerArgument& argument) {
    bool isCandidateForAuto = true;
    argument.Reset();
    if (constexpr auto argStr = "arg";
        element == nullptr) { return SetLastError(std::string(argStr), std::string(ErrorNullptr)); }
    else if (!StringNCompare(element->Name(), argStr)) { return SetLastError(element, std::string(ErrorUnexpectedElement) + argStr); }

    if (const char *typeStr = "type", *str = element->Attribute(typeStr);
        str != nullptr) {
        if (!StringToArgType(str, argument.type)) { return SetLastError(element, fmt::format("{}{}: {}", ErrorObtainingString, typeStr, str)); }
        else {
            isCandidateForAuto = false;
        }
    }

    if (const char *indirectStr = "indirect", *str = element->Attribute(indirectStr);
        str != nullptr) {
        if (!StringToBool(str, argument.indirectArg)) { return SetLastError(element, std::string(ErrorObtainingBool) + indirectStr); }
        else {
            isCandidateForAuto = false;
        }
    }

    if (const char *operationStr = "operation", *str = element->Attribute(operationStr);
        str != nullptr) {
        if (!StringToRegOperation(str, argument.operation)) { return SetLastError(element, std::string(ErrorObtainingOperation) + operationStr); }
        else {
            isCandidateForAuto = false;
        }
    }

    if (const char *offsetStr = "offset", *str = element->Attribute(offsetStr);
        str != nullptr) {
        if (!StringToUnsigned(str, argument.value.offset)) { return SetLastError(element, std::string(ErrorObtainingUnsignedInt) + offsetStr); }
        else {
            isCandidateForAuto = false;
        }
    }

    // TODO: need to figure out what to do with reg when immediate
    if (const char* str = element->Attribute("reg");
        str != nullptr) {
        argument.value.reg = str;
        isCandidateForAuto = false;
    }

    if (const char *valueStr = "value", *str = element->Attribute(valueStr);
        str == nullptr) { return SetLastError(element, std::string(ErrorObtainingString) + valueStr); }
    else {
        argument.value.name = str;
    }

    // Parse Auto last
    if (isCandidateForAuto) {
        if (!ParseAutoArgType(argument)) {
            return SetLastError(element, std::string(ErrorDeterminingArgumentType) + argument.value.name);
        }
    }

    return true;
}

// bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, std::string& string, bool allowDefault) {
//     string.clear();
//     if (!element) { return SetLastError("string", ErrorNullptr); }
//     string = GetTextAsString(element);
//     if (!allowDefault && string.empty()) { return SetLastError(element, ErrorObtainingString + element->Name()); }
//
//     return true;
// }
//
// bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, unsigned int& outValue, bool allowDefault) {
//     outValue = 0u;
//     std::string temp;
//     if (!element) { return SetLastError("unsigned int", ErrorNullptr); }
//     try {
//         temp = GetTextAsString(element);
//         outValue = std::stoul(temp, nullptr, 0);
//     }
//     catch (...) {
//         if (!allowDefault) { return SetLastError(element, ErrorObtainingUnsignedInt + element->Name()); }
//     }
//
//     return true;
// }
//
// bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, FlagType& outValue, bool allowDefault) {
//     const std::string errorString = "Failed to find FlagType for XML element ";
//     outValue = FlagType::NONE;
//     if (!element) { return (allowDefault) ? true :/* SetLastError(errorString)*/ false; }
//     const auto attribute = element->FindAttribute("name");
//     if (!attribute) { return (allowDefault) ? true :/* SetLastError(errorString)*/ false; }
//     outValue = StringToFlagType(GetTextAsString(attribute));
//     if (!allowDefault && outValue == FlagType::NONE) { return/* SetLastError(errorString + element->Name())*/ false; }
//
//     return true;
// }

// Error handling helper
bool XmlElementParser::SetLastError(const std::string& elementName, const std::string& errorMsg) {
    m_lastError = fmt::format("Error XmlElementParser: failed to parse: \"{}\", {}", elementName, errorMsg);
    return false;
}
bool XmlElementParser::SetLastError(const tinyxml2::XMLElement* element, const std::string& errorMsg) {
    const auto* name = element->Name();
    const auto lineNum = element->GetLineNum();
    m_lastError = fmt::format("Error XmlElementParser: \"{}\" at line {}, {}", name == nullptr ? "UnknownElement" : name, lineNum, errorMsg);
    return false;
}
bool XmlElementParser::SetLastError(const std::string& elementName, std::string_view errorMsg) {
    m_lastError = fmt::format("Error XmlElementParser: failed to parse: \"{}\", {}", elementName, errorMsg);
    return false;
}

bool XmlElementParser::SetLastError(const tinyxml2::XMLElement* element, std::string_view errorMsg) {
    const auto* name = element->Name();
    const auto lineNum = element->GetLineNum();
    m_lastError = fmt::format("Error XmlElementParser: \"{}\" at line {}, {}", name == nullptr ? "UnknownElement" : name, lineNum, errorMsg);
    return false;
}