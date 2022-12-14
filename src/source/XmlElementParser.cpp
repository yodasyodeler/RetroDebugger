#include "XmlElementParser.h"

#include <algorithm>

static const auto ErrorNullptr = std::string("encountered unexpected nullptr");
static const auto ErrorUnexpectedElement = std::string("Found an unexpected element. Looking for ");
static const auto ErrorObtainingUnsignedInt = std::string("Failed to obtain a valid unsigned int for ");
static const auto ErrorObtainingOpcodeLength = std::string("Failed to obtain a valid OperationLength for ");
static const auto ErrorObtainingString = std::string("Failed to obtain a valid string for ");
static const auto ErrorObtainingBool = std::string("Failed to obtain a valid bool for ");
static const auto ErrorObtainingOperation = std::string("Failed to obtain a valid reg operation for ");
static const auto ErrorDeterminingArgumentType = std::string("Failed to determine argument type for ");

bool ParseArgOperations(std::string& name, XmlDebuggerArgument& arg);

// local helper methods
// TODO: could move to a common file, probably keep here unless code is needed else where. Should check the text formatting code
static bool StringNCompare(const std::string_view& string1, const std::string_view& string2) {
    if (string1.size() != string2.size()) return false;

    auto toUpperString = [](std::string_view str) {
        std::string re(str);
        std::ranges::transform(re, re.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        return re;
    };
    return !toUpperString(string1).compare(toUpperString(string2));
}

// TODO: I wonder if I should look into a method that doesn't use an exception.
static bool StringToUnsigned(const std::string& str, unsigned int& value) {
    try {
        value = std::stoul(str, nullptr, 0);
    }
    catch (...) {
        return false;
    }
    return true;
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
    for (const auto& c : string) {
        if (!std::isalpha(c)) { return false; }
    }
    return true;
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
    if (StringToImmediateArgType(str, type)) { return true; }
    return false;
}

static bool StringToOpcodeLength(const std::string_view& str, OpcodeLength& opcodeLength) {
    if (StringNCompare("8", str)) {
        opcodeLength = 8;
        return true;
    }
    if (StringNCompare("BYTE", str)) {
        opcodeLength = 8;
        return true;
    }
    if (StringNCompare("16", str)) {
        opcodeLength = 16;
        return true;
    }
    if (StringNCompare("32", str)) {
        opcodeLength = 32;
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
    const auto& value = name; // TODO: what am I doing here
    constexpr auto operationChars = "+-";
    auto operationFind = value.find_first_of(operationChars);
    if (operationFind == std::string::npos) {
        arg.operation = RegOperationType::NONE;
        return false;
    }

    if (operationFind == 0) { // PreOp
        if (value[operationFind] == '+' && IsAlphaString(value.substr(1))) { arg.operation = RegOperationType::PREINC; }
        else if (value[operationFind] == '-') {
            arg.operation = RegOperationType::PREDEC;
        }
        else {
            arg.operation = RegOperationType::NONE;
            return false;
        }
        name.erase(0, 1);
    }
    else if (operationFind == value.size() - 1) { // PostOp
        if (value[operationFind] == '+') { arg.operation = RegOperationType::POSTINC; }
        else if (value[operationFind] == '-') {
            arg.operation = RegOperationType::POSTDEC;
        }
        else {
            arg.operation = RegOperationType::NONE;
            return false;
        }
        name.erase(name.size() - 1);
    }
    else { // Check if regoffset
        const auto firstParam = value.substr(0, operationFind);
        const auto secondParam = value.substr(operationFind + 1);
        if (IsAlphaString(firstParam) && StringToUnsigned(secondParam, arg.value.offset)) { // TODO: is there a unit test for this?
            name = firstParam;
            arg.operation = (value[operationFind] == '+') ? RegOperationType::REG_OFFSET_ADD : RegOperationType::REG_OFFSET_SUB;
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
    }

    const auto reservedCharacters = std::string(operationChars) + "()";
    if (name.find(reservedCharacters) != std::string::npos) {
        arg.operation = RegOperationType::NONE;
        return false;
    }

    return true;
}

std::string GetTextAsString(const tinyxml2::XMLAttribute* attribute) {
    auto textPtr = attribute->Value();
    return (!textPtr) ? "" : std::string(textPtr);
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
std::string XmlElementParser::GetLastError() {
    return m_lastError;
}

bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperations& operations) {
    operations.Reset();

    auto OperationsStr = "operations";
    if (!element) { return SetLastError(OperationsStr, ErrorNullptr); }
    if (!StringNCompare(element->Name(), "operations")) { return SetLastError(element, ErrorUnexpectedElement + OperationsStr); }

    const char* opcodeLengthStr = "opcodeLength";
    const char* str = element->Attribute(opcodeLengthStr);
    if ((!str) || !StringToOpcodeLength(str, operations.opcodeLength)) { return SetLastError(element, ErrorObtainingOpcodeLength + opcodeLengthStr); }

    const char* extenededOpcodeStr = "extended";
    str = element->Attribute(extenededOpcodeStr);
    if (!str) { operations.extendedOpcode = NORMAL_OPERATIONS_KEY; }
    else if (!StringToUnsigned(str, operations.extendedOpcode)) {
        return SetLastError(element, ErrorObtainingUnsignedInt + extenededOpcodeStr);
    }

    return true;
}

bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperation& operation) {
    operation.Reset();

    constexpr auto OperationStr = "operation";
    if (!element) { return SetLastError(OperationStr, ErrorNullptr); }
    if (!StringNCompare(element->Name(), OperationStr)) { return SetLastError(element, ErrorUnexpectedElement + OperationStr); }

    constexpr auto opcodeStr = "opcode";
    const char* str = element->Attribute(opcodeStr);
    if (!str || !StringToUnsigned(str, operation.opcode)) { return SetLastError(element, ErrorObtainingUnsignedInt + opcodeStr); }

    constexpr auto commandStr = "command";
    str = element->Attribute(commandStr);
    if (!str) { return SetLastError(element, ErrorObtainingString + commandStr); }
    operation.command = str;

    constexpr auto isJumpStr = "isJump";
    constexpr auto trueStr = "true";
    str = element->Attribute(isJumpStr);
    if (str) {
        operation.isJump = (_stricmp(str, trueStr) == 0); // TODO: cross platform issue
    }

    return true;
}

// R"(<arg type="Reg" indirect="false" operation="none" reg="A" value="A"/>)";
bool XmlElementParser::ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerArgument& argument) {
    bool isCandidateForAuto = true;
    argument.Reset();
    constexpr auto argStr = "arg";
    if (!element) { return SetLastError(argStr, ErrorNullptr); }
    if (!StringNCompare(element->Name(), argStr)) { return SetLastError(element, ErrorUnexpectedElement + argStr); }

    constexpr auto typeStr = "type";
    const char* str = element->Attribute(typeStr);
    if (str) {
        if (!StringToArgType(str, argument.type)) { return SetLastError(element, ErrorObtainingString + typeStr); }
        else {
            isCandidateForAuto = false;
        }
    }

    constexpr auto indirectStr = "indirect";
    str = element->Attribute(indirectStr);
    if (str) {
        if (!StringToBool(str, argument.indirectArg)) { return SetLastError(element, ErrorObtainingBool + indirectStr); }
        else {
            isCandidateForAuto = false;
        }
    }

    constexpr auto operationStr = "operation";
    str = element->Attribute(operationStr);
    if (str) {
        if (!StringToRegOperation(str, argument.operation)) { return SetLastError(element, ErrorObtainingOperation + operationStr); }
        else {
            isCandidateForAuto = false;
        }
    }

    constexpr auto offsetStr = "offset";
    str = element->Attribute(offsetStr);
    if (str) {
        if (!StringToUnsigned(str, argument.value.offset)) { return SetLastError(element, ErrorObtainingUnsignedInt + offsetStr); }
        else {
            isCandidateForAuto = false;
        }
    }

    // TODO: need to figure out what to do with reg when immediate
    constexpr auto regStr = "reg";
    str = element->Attribute(regStr);
    if (str) {
        argument.value.reg = str;
        isCandidateForAuto = false;
    }

    constexpr auto valueStr = "value";
    str = element->Attribute(valueStr);
    if (!str) { return SetLastError(element, ErrorObtainingString + valueStr); }
    else {
        argument.value.name = str;
    }

    // Parse Auto last
    if (isCandidateForAuto) {
        if (!ParseAutoArgType(argument)) {
            return SetLastError(element, ErrorDeterminingArgumentType + argument.value.name);
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
//     const std::string errorString = "Failed to find FlagType for XML elment ";
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
const auto errorPreMsg = std::string("Error XmlElementParser: ");
bool XmlElementParser::SetLastError(const std::string& elementName, const std::string& errorMsg) {
    m_lastError = errorPreMsg + "failed to parse: \"" + elementName + "\", " + errorMsg;
    return false;
}

bool XmlElementParser::SetLastError(const tinyxml2::XMLElement* element, const std::string& errorMsg) {
    const auto name = element->Name();
    const auto lineNum = element->GetLineNum();
    const auto fullErrorMsg = errorPreMsg + "\"" + name + "\" at line " + std::to_string(lineNum) + ", " + errorMsg;
    m_lastError = fullErrorMsg;
    return false;
}