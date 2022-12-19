#include "DebuggerXmlParser.h"

#include <fmt/core.h>

static constexpr std::string_view ErrorNullptr = "Encountered unexpected nullptr";
static constexpr std::string_view ErrorEmptyFile = "couldn't find the first XML element in the file";
static constexpr std::string_view ErrorPreMsg = "Error XmlParser: ";

static constexpr std::string_view OperationsStr = "operations";
static constexpr std::string_view OperationStr = "operation";
static constexpr std::string_view ArgStr = "arg";

bool DebuggerXmlParser::SetLastError(std::string_view elementName, std::string_view errorMsg) {
    m_lastError = fmt::format("{}\"{}\", {}", ErrorPreMsg, elementName, errorMsg);
    return false;
}

bool DebuggerXmlParser::SetLastError(const tinyxml2::XMLElement* element, std::string_view errorMsg) {
    const auto* name = (element->Name() != nullptr) ? element->Name() : "UnkownElement";
    m_lastError = fmt::format("{}\"{}\" at line {}, {}", ErrorPreMsg, name, element->GetLineNum(), errorMsg);
    return false;
}

DebuggerXmlParser::DebuggerXmlParser(const std::string& filename) {
    ParseFile(filename);
}

void DebuggerXmlParser::Reset() {
    m_xmlDocument.Clear();
    m_operationMap.clear();
    m_lastError.clear();
}

bool DebuggerXmlParser::ParseFile(const std::string& filename) {
    if (m_xmlDocument.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        m_lastError = m_xmlDocument.ErrorStr();
        return false;
    }

    return ParseXmlDocument(m_xmlDocument);
}

XmlOperationsMap DebuggerXmlParser::GetOperations() {
    return m_operationMap;
}

bool DebuggerXmlParser::ParseXmlDocument(const tinyxml2::XMLDocument& xmlDocument) {
    m_operationMap.clear();

    const auto* element = xmlDocument.FirstChildElement();
    if (element == nullptr) { return SetLastError("XML file", ErrorEmptyFile); } // if file is empty report an error

    while (element != nullptr) {
        const std::string_view name = (element->Name() != nullptr) ? element->Name() : "";

        XmlDebuggerOperations operations;
        if (name == OperationsStr) { // TODO: functionalize this to make it easier to read/maintain
            if (!ParseOperations(element, operations)) { return SetLastError(OperationsStr, m_parser.GetLastError()); }
            else {
                if (m_operationMap.find(operations.extendedOpcode) != m_operationMap.end()) { return SetLastError(element, fmt::format("Reused extended operation opcode: {}", operations.extendedOpcode)); }
                m_operationMap.emplace(operations.extendedOpcode, operations);
            }
        }

        else {
            return SetLastError(element, "Unknown element");
        }
        element = element->NextSiblingElement();
    }
    return true;
}

bool DebuggerXmlParser::ParseOperations(const tinyxml2::XMLElement* operationElements, XmlDebuggerOperations& operations) {
    // Parse xmlOperation element
    if (operationElements == nullptr) { return SetLastError(OperationsStr, ErrorNullptr); }
    if (!m_parser.ParseXmlElement(operationElements, operations)) { return SetLastError(OperationsStr, m_parser.GetLastError()); }

    const auto* element = operationElements->FirstChildElement();
    XmlDebuggerOperation xmlOperation;
    while (element != nullptr) {
        const std::string_view name = (element->Name() != nullptr) ? element->Name() : "";

        if (name == OperationStr) {
            if (!ParseOperation(element, xmlOperation)) { return SetLastError(OperationStr, m_parser.GetLastError()); }
            else {
                if (operations.operations.find(xmlOperation.opcode) != operations.operations.end()) { return SetLastError(element, fmt::format("Reused normal operation opcode: {}", xmlOperation.opcode)); }
                auto opcode = xmlOperation.opcode;
                operations.operations.emplace(opcode, xmlOperation);
            }
        }
        else {
            return SetLastError(element, "Unknown element");
        }
        element = element->NextSiblingElement();
    }

    return true;
}

bool DebuggerXmlParser::ParseOperation(const tinyxml2::XMLElement* operationElement, XmlDebuggerOperation& operation) {
    // Parse xmlOperation element
    if (operationElement == nullptr) { return SetLastError(OperationStr, ErrorNullptr); }
    if (!m_parser.ParseXmlElement(operationElement, operation)) { return SetLastError(OperationStr, m_parser.GetLastError()); }

    // Parse children elements of xmlOperation
    XmlDebuggerArgument argument;
    const auto* element = operationElement->FirstChildElement();
    while (element != nullptr) {
        const std::string_view name = (element->Name() != nullptr) ? element->Name() : "";

        if (name == ArgStr) {
            if (!m_parser.ParseXmlElement(element, argument)) { return SetLastError(ArgStr, m_parser.GetLastError()); }
            else {
                operation.arguments.emplace_back(argument);
            }
        }

        else {
            return SetLastError(element, "Unknown element");
        }
        element = element->NextSiblingElement();
    }
    return true;
}

std::string DebuggerXmlParser::GetLastError() {
    return m_lastError;
}

// bool DebuggerXmlParser::ParseCommands() {
//     //Get to first tempElement
//     const auto commandsStr = "commands";
//     const auto commands = m_xmlDocument.FirstChildElement(commandsStr);
//     if (!commands) { return SetLastError(commandsStr, "Couldn't find xml element"); }
//
//     const auto commandStr = "command";
//     auto command = commands->FirstChildElement(commandStr);
//     if (!command) { return SetLastError(commandStr, "Couldn't find xml element"); }
//
//     //Parse all Commands
//     do {
//         if (!ParseCommand(*command)) {
//             m_commandMap.clear();
//             return false;
//         }
//         command = command->NextSiblingElement(commandStr);
//     } while (command);
//
//     return true;
// }
//
////TODO: could be a good idea to move tempElement null check before ParseXmlElement to log more info
////TODO: figure out what should error out here, probably should make it strict so mistakes are caught and not glossed over in the xml.
////TODO: maybe I should functionalize some of these, ie get string from tempElement, get enum from tempElement?
// bool DebuggerXmlParser::ParseCommand(const tinyxml2::XMLElement& element) {
//     DebuggerCommand dbgCommand = {};
//
//     auto tempElement = element.FirstChildElement("name");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_name), false) { return false; }
//     if (dbgCommand.m_name == "") { dbgCommand.m_name = "Unkown Command"; }
//
//     tempElement = element.FirstChildElement("mnemonic");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_mnemonicPrint), false) { return false; }
//
//     //prettyFormat
//     tempElement = element.FirstChildElement("prettyFormat");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_prettyPrint, true)) { return false; }
//
//     //numberArguments
//     tempElement = element.FirstChildElement("numberArguments");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_numberArguments, true)) { return false; }
//
//     //immediateType
//     tempElement = element.FirstChildElement("immediateType");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_immediateType, true)) { return false; }
//
//     //type
//     tempElement = element.FirstChildElement("operationType");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_operationType, true)) { return false; }
//
//     //description
//     tempElement = element.FirstChildElement("description");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_description, true)) { return false; }
//
//     //flags
//     tempElement = element.FirstChildElement("flags");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_flagsAffected, true)) { return false; }
//     dbgCommand.m_flagsAffected.erase(FlagType::NONE); // remove NONE from the set.
//
//     //opcodes
//     std::set<unsigned int> opcodes = {};
//     tempElement = element.FirstChildElement("opcodes");
//     if (!m_parser.ParseXmlElement(tempElement, opcodes, false)) { return false; }
//
//     //construct dbg element
//     const auto dbgCommandPtr = std::make_shared<const DebuggerCommand>(dbgCommand);
//     for (const auto& opcode : opcodes)
//     {
//         if (m_commandMap.find(opcode) != m_commandMap.end()) { return false /*ReportReusedOpcode(opcode, *m_commandMap.at(opcode), dbgCommand)*/; }
//         m_commandMap.emplace(opcode, dbgCommandPtr);
//     }
//
//     return true;
// }
