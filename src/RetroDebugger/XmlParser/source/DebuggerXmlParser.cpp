#include "DebuggerXmlParser.h"

#include "XmlParserException.h"

#include <fmt/core.h>
#include <tinyxml2.h>

static constexpr std::string_view ErrorNullptr = "Encountered unexpected nullptr";
static constexpr std::string_view ErrorEmptyFile = "couldn't find the first XML element in the file";
static constexpr std::string_view ErrorPreMsg = "Error XmlParser: ";
static constexpr std::string_view ErrorUnknownElementStr = "Unknown element";

static constexpr std::string_view RetroDebuggerStr = "RetroDebugger";
static constexpr std::string_view OperationsStr = "operations";
static constexpr std::string_view OperationStr = "operation";
static constexpr std::string_view ArgStr = "arg";


DebuggerXmlParser::DebuggerXmlParser(const std::string& filename) {
    ParseFile(filename);
}

void DebuggerXmlParser::Reset() {
    m_isValid = false;
    m_operationMap.clear();
    m_lastError.clear();
}

bool DebuggerXmlParser::IsValid() {
    return m_isValid;
}

void DebuggerXmlParser::ParseFile(const std::string& filename) {
    Reset();
    tinyxml2::XMLDocument document;
    if (document.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        throw XmlParserException::CreateError(document.ErrorStr());
    }
    return ParseXmlDocument(document);
}

XmlOperationsMap DebuggerXmlParser::GetOperations() const {
    if (!m_isValid) {
        using namespace std::string_literals;
        throw XmlParserException::CreateError("Failed to get operations. XML file was not valid."s);
    }
    return m_operationMap;
}

void DebuggerXmlParser::ParseXmlDocument(const tinyxml2::XMLDocument& xmlDocument) {
    Reset();

    const auto* element = xmlDocument.FirstChildElement();
    if (element == nullptr) { throw XmlParserException::CreateError("XML file", ErrorEmptyFile); } // if file is empty report an error

    if (const auto* unexpectedSibling = element->NextSiblingElement();
        unexpectedSibling != nullptr) {
        throw XmlParserException::CreateError("XML file", fmt::format("'RetroDebugger' must be the only root element. Found sibling element: '{}'", (unexpectedSibling->Name() == nullptr ? "" : unexpectedSibling->Name())));
    }

    if (const auto* name = element->Name();
        name == nullptr || name != RetroDebuggerStr) {
        throw XmlParserException::CreateError("XML file", fmt::format("Root XML element must be 'RetroDebugger'. Root element found: '{}'", (name == nullptr ? "" : name)));
    }

    element = element->FirstChildElement(); // Go to first element within the 'RetroDebugger' element
    while (element != nullptr) {
        const std::string_view name = (element->Name() != nullptr) ? element->Name() : "";

        XmlDebuggerOperations operations;
        if (name == OperationsStr) { // TODO: functionalize this to make it easier to read/maintain
            try {
                ParseOperations(element, operations);
            }
            catch (const XmlParserException& e) {
                throw XmlParserException::CreateError(OperationsStr, std::string(e.what()));
            }

            if (m_operationMap.find(operations.extendedOpcode) != m_operationMap.end()) { throw XmlParserException::CreateError(element, fmt::format("Reused extended operation opcode: {}", operations.extendedOpcode)); }
            m_operationMap.emplace(operations.extendedOpcode, operations);
        }
        else {
            throw XmlParserException::CreateError(element, ErrorUnknownElementStr);
        }
        element = element->NextSiblingElement();
    }

    m_isValid = true;
}

void DebuggerXmlParser::ParseOperations(const tinyxml2::XMLElement* operationElements, XmlDebuggerOperations& operations) {
    // Parse xmlOperation element
    if (operationElements == nullptr) { throw XmlParserException::CreateError(OperationsStr, ErrorNullptr); }
    try {
        m_parser.ParseXmlElement(operationElements, operations);
    }
    catch (const XmlParserException& e) {
        throw XmlParserException::CreateError(OperationsStr, std::string(e.what()));
    }

    const auto* element = operationElements->FirstChildElement();
    XmlDebuggerOperation xmlOperation;
    while (element != nullptr) {
        const std::string_view name = (element->Name() != nullptr) ? element->Name() : "";

        if (name == OperationStr) {
            try {
                ParseOperation(element, xmlOperation);
            }
            catch (const XmlParserException& e) {
                throw XmlParserException::CreateError(OperationStr, std::string(e.what()));
            }

            if (operations.operations.find(xmlOperation.opcode) != operations.operations.end()) { throw XmlParserException::CreateError(element, fmt::format("Reused normal operation opcode: {}", xmlOperation.opcode)); }
            auto opcode = xmlOperation.opcode;
            operations.operations.emplace(opcode, xmlOperation);
        }
        else {
            throw XmlParserException::CreateError(element, ErrorUnknownElementStr);
        }
        element = element->NextSiblingElement();
    }
}

void DebuggerXmlParser::ParseOperation(const tinyxml2::XMLElement* operationElement, XmlDebuggerOperation& operation) {
    // Parse xmlOperation element
    if (operationElement == nullptr) { throw XmlParserException::CreateError(OperationStr, ErrorNullptr); }
    try {
        m_parser.ParseXmlElement(operationElement, operation);
    }
    catch (const XmlParserException& e) {
        throw XmlParserException::CreateError(OperationStr, std::string(e.what()));
    }

    // Parse children elements of xmlOperation
    XmlDebuggerArgument argument;
    const auto* element = operationElement->FirstChildElement();
    while (element != nullptr) {
        const std::string_view name = (element->Name() != nullptr) ? element->Name() : "";

        if (name == ArgStr) {
            try {
                m_parser.ParseXmlElement(element, argument);
            }
            catch (const XmlParserException& e) {
                throw XmlParserException::CreateError(ArgStr, std::string(e.what()));
            }
            operation.arguments.emplace_back(argument);
        }

        else {
            throw XmlParserException::CreateError(element, ErrorUnknownElementStr);
        }
        element = element->NextSiblingElement();
    }
}

// bool DebuggerXmlParser::ParseCommands() {
//     //Get to first tempElement
//     const auto commandsStr = "commands";
//     const auto commands = m_xmlDocument.FirstChildElement(commandsStr);
//     if (!commands) { return SetLastError(commandsStr, "Couldn't find XML element"); }
//
//     const auto commandStr = "command";
//     auto command = commands->FirstChildElement(commandStr);
//     if (!command) { return SetLastError(commandStr, "Couldn't find XML element"); }
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
////TODO: figure out what should error out here, probably should make it strict so mistakes are caught and not glossed over in the XML.
////TODO: maybe I should functionalize some of these, ie) get string from tempElement, get enum from tempElement?
// bool DebuggerXmlParser::ParseCommand(const tinyxml2::XMLElement& element) {
//     DebuggerCommand dbgCommand = {};
//
//     auto tempElement = element.FirstChildElement("name");
//     if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_name), false) { return false; }
//     if (dbgCommand.m_name == "") { dbgCommand.m_name = "Unknown Command"; }
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
//     //construct DBG element
//     const auto dbgCommandPtr = std::make_shared<const DebuggerCommand>(dbgCommand);
//     for (const auto& opcode : opcodes)
//     {
//         if (m_commandMap.find(opcode) != m_commandMap.end()) { return false /*ReportReusedOpcode(opcode, *m_commandMap.at(opcode), dbgCommand)*/; }
//         m_commandMap.emplace(opcode, dbgCommandPtr);
//     }
//
//     return true;
// }
