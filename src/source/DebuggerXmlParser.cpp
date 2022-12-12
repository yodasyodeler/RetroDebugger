#include "DebuggerXmlParser.h"

const auto ErrorNullptr = std::string("Encountered unexpected nullptr");
const auto ErrorReusedOperation = std::string("Reused normal operation opcode: ");
const auto ErrorReusedExtendedOperation = std::string("Reused extended operation opcode: ");
const auto ErrorEmptyFile = std::string("couldn't find the first XML element in the file");

const auto ErrorPreMsg = std::string("Error XmlParser: ");

const auto OperationsStr = "operations";
const auto OperationStr = "operation";
const auto ArgStr = "arg";

bool DebuggerXmlParser::SetLastError(const std::string& elementName, const std::string& errorMsg) {
    const auto fullErrorMsg = ErrorPreMsg + "\"" + elementName + "\", " + errorMsg;
    m_lastError = fullErrorMsg;
    return false;
}

bool DebuggerXmlParser::SetLastError(const tinyxml2::XMLElement* element, const std::string& errorMsg) {
    const auto name = element->Name();
    const auto lineNum = element->GetLineNum();
    const auto fullErrorMsg = ErrorPreMsg + "\"" + name + "\" at line " + std::to_string(lineNum) + ", " + errorMsg;
    m_lastError = fullErrorMsg;
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

    auto element = xmlDocument.FirstChildElement();
    if (!element) { return SetLastError("XML file", ErrorEmptyFile); } //if file is empty report an error

    while (element) {
        const auto name = element->Name() ? std::string(element->Name()) : std::string("");

        XmlDebuggerOperations operations;
        if (name == OperationsStr) { //TODO: functionalize this to make it easier to read/maintain
            if (!ParseOperations(element, operations)) { return SetLastError(OperationsStr, m_parser.GetLastError()); }
            else {
                if (m_operationMap.find(operations.extendedOpcode) != m_operationMap.end()) { return SetLastError(element, ErrorReusedExtendedOperation + std::to_string(operations.extendedOpcode)); }
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


bool DebuggerXmlParser::ParseOperations(const tinyxml2::XMLElement* operationsElement, XmlDebuggerOperations& xmlOperations) {
    //Parse xmlOperation element
    if (!operationsElement) { return SetLastError(OperationsStr, ErrorNullptr); }
    if (!m_parser.ParseXmlElement(operationsElement, xmlOperations)) { return SetLastError(OperationsStr, m_parser.GetLastError()); }

    auto element = operationsElement->FirstChildElement();

    XmlDebuggerOperation xmlOperation;
    while (element) {
        const auto name = element->Name() ? std::string(element->Name()) : std::string("");

        if (name == OperationStr) {
            if (!ParseOperation(element, xmlOperation)) { return SetLastError(OperationStr, m_parser.GetLastError()); }
            else {
                if (xmlOperations.operations.find(xmlOperation.opcode) != xmlOperations.operations.end()) { return SetLastError(element, ErrorReusedOperation + std::to_string(xmlOperation.opcode)); }
                auto opcode = xmlOperation.opcode;
                xmlOperations.operations.emplace(opcode, xmlOperation);
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
    //Parse xmlOperation element
    if (!operationElement) { return SetLastError(OperationStr, ErrorNullptr); }
    if (!m_parser.ParseXmlElement(operationElement, operation)) { return SetLastError(OperationStr, m_parser.GetLastError()); }

    //Parse children elements of xmlOperation
    XmlDebuggerArgument argument;
    auto element = operationElement->FirstChildElement();
    while (element) {
        const auto name = element->Name() ? std::string(element->Name()) : std::string("");

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

//bool DebuggerXmlParser::ParseCommands() {
//    //Get to first tempElement
//    const auto commandsStr = "commands";
//    const auto commands = m_xmlDocument.FirstChildElement(commandsStr);
//    if (!commands) { return SetLastError(commandsStr, "Couldn't find xml element"); }
//
//    const auto commandStr = "command";
//    auto command = commands->FirstChildElement(commandStr);
//    if (!command) { return SetLastError(commandStr, "Couldn't find xml element"); }
//
//    //Parse all Commands
//    do {
//        if (!ParseCommand(*command)) {
//            m_commandMap.clear();
//            return false;
//        }
//        command = command->NextSiblingElement(commandStr);
//    } while (command);
//
//    return true;
//}
//
////TODO: could be a good idea to move tempElement null check before ParseXmlElement to log more info
////TODO: figure out what should error out here, probably should make it strict so mistakes are caught and not glossed over in the xml.
////TODO: maybe I should functionalize some of these, ie get string from tempElement, get enum from tempElement?
//bool DebuggerXmlParser::ParseCommand(const tinyxml2::XMLElement& element) {
//    DebuggerCommand dbgCommand = {};
//
//    auto tempElement = element.FirstChildElement("name");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_name), false) { return false; }
//    if (dbgCommand.m_name == "") { dbgCommand.m_name = "Unkown Command"; }
//
//    tempElement = element.FirstChildElement("mnemonic");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_mnemonicPrint), false) { return false; }
//
//    //prettyFormat
//    tempElement = element.FirstChildElement("prettyFormat");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_prettyPrint, true)) { return false; }
//
//    //numberArguments
//    tempElement = element.FirstChildElement("numberArguments");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_numberArguments, true)) { return false; }
//
//    //immediateType
//    tempElement = element.FirstChildElement("immediateType");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_immediateType, true)) { return false; }
//
//    //type
//    tempElement = element.FirstChildElement("operationType");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_operationType, true)) { return false; }
//
//    //description
//    tempElement = element.FirstChildElement("description");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_description, true)) { return false; }
//
//    //flags
//    tempElement = element.FirstChildElement("flags");
//    if (!m_parser.ParseXmlElement(tempElement, dbgCommand.m_flagsAffected, true)) { return false; }
//    dbgCommand.m_flagsAffected.erase(FlagType::NONE); // remove NONE from the set.
//
//    //opcodes
//    std::set<unsigned int> opcodes = {};
//    tempElement = element.FirstChildElement("opcodes");
//    if (!m_parser.ParseXmlElement(tempElement, opcodes, false)) { return false; }
//
//    //construct dbg element
//    const auto dbgCommandPtr = std::make_shared<const DebuggerCommand>(dbgCommand);
//    for (const auto& opcode : opcodes)
//    {
//        if (m_commandMap.find(opcode) != m_commandMap.end()) { return false /*ReportReusedOpcode(opcode, *m_commandMap.at(opcode), dbgCommand)*/; }
//        m_commandMap.emplace(opcode, dbgCommandPtr);
//    }
//
//    return true;
//}
