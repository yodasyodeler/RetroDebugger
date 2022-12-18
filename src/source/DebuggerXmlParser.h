#pragma once

#include "tinyxml2.h"
#include <string>

#include "DebuggerCommon.h"

#include "XmlElementParser.h"

class DebuggerXmlParser {
public:
    DebuggerXmlParser() = default;
    explicit DebuggerXmlParser(const std::string& filename);

    void Reset();

    bool ParseFile(const std::string& filename);
    bool ParseXmlDocument(const tinyxml2::XMLDocument& xmlDocument);

    XmlOperationsMap GetOperations();
    // DebuggerCommandMap GetCommands();

    std::string GetLastError();

private:
    bool ParseOperations(const tinyxml2::XMLElement* command, XmlDebuggerOperations& operations);
    bool ParseOperation(const tinyxml2::XMLElement* command, XmlDebuggerOperation& operation);


    bool SetLastError(const std::string& elementName, const std::string& error);
    bool SetLastError(const tinyxml2::XMLElement* element, const std::string& error);

    // bool ParseCommands();
    // bool ParseCommand(const tinyxml2::XMLElement& command);

    tinyxml2::XMLDocument m_xmlDocument;
    XmlOperationsMap m_operationMap = {};
    std::string m_lastError = {};
    XmlElementParser m_parser;
};