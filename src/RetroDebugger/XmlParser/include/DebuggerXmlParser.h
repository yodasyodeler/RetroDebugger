#pragma once

#include "DebuggerCommon.h"

#include "XmlElementParser.h"

class DebuggerXmlParser {
public:
    DebuggerXmlParser() = default;
    explicit DebuggerXmlParser(const std::string& filename); // TODO: should this be a PATH type instead?

    void Reset();

    bool ParseFile(const std::string& filename);
    bool ParseXmlDocument(const tinyxml2::XMLDocument& xmlDocument);

    XmlOperationsMap GetOperations() const;
    // DebuggerCommandMap GetCommands();

    std::string GetLastError() const;

private:
    bool ParseOperations(const tinyxml2::XMLElement* operationElements, XmlDebuggerOperations& operations);
    bool ParseOperation(const tinyxml2::XMLElement* operationElement, XmlDebuggerOperation& operation);

    bool SetLastError(std::string_view elementName, const std::string& errorMsg);
    bool SetLastError(const tinyxml2::XMLElement* element, const std::string& errorMsg);
    bool SetLastError(std::string_view elementName, std::string_view errorMsg);
    bool SetLastError(const tinyxml2::XMLElement* element, std::string_view errorMsg);

    // bool ParseCommands();
    // bool ParseCommand(const tinyxml2::XMLElement& command);

    tinyxml2::XMLDocument m_xmlDocument;
    XmlOperationsMap m_operationMap = {};
    std::string m_lastError = {};
    XmlElementParser m_parser;
};