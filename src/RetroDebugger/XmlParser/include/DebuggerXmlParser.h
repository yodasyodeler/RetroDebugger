#pragma once

#include "DebuggerCommon.h"

#include "XmlElementParser.h"

namespace tinyxml2 {
class XMLElement;
class XMLDocument;
}

class DebuggerXmlParser {
public:
    DebuggerXmlParser() = default;
    explicit DebuggerXmlParser(const std::string& filename); // TODO: should this be a PATH type instead?

    void Reset();

    bool IsValid();

    void ParseFile(const std::string& filename);
    void ParseXmlDocument(const tinyxml2::XMLDocument& xmlDocument);

    XmlOperationsMap GetOperations() const;
    // DebuggerCommandMap GetCommands();

private:
    void ParseOperations(const tinyxml2::XMLElement* operationElements, XmlDebuggerOperations& operations);
    void ParseOperation(const tinyxml2::XMLElement* operationElement, XmlDebuggerOperation& operation);

    // bool ParseCommands();
    // bool ParseCommand(const tinyxml2::XMLElement& command);

    bool m_isValid = false;
    XmlOperationsMap m_operationMap = {};
    std::string m_lastError = {};
    XmlElementParser m_parser;
};