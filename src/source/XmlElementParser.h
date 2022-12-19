#pragma once

#include "tinyxml2.h"

#include "DebuggerCommon.h"

class XmlElementParser {
public:
    std::string GetLastError();

    bool ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperations& operations);
    bool ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperation& operation);
    bool ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerArgument& argument);
    // bool ParseXmlElement(const tinyxml2::XMLElement* element, std::string& outValue, bool allowDefault = false);
    // bool ParseXmlElement(const tinyxml2::XMLElement* element, unsigned int& outValue, bool allowDefault = false);
    // bool ParseXmlElement(const tinyxml2::XMLElement* element, FlagType& outValue, bool allowDefault = false);

    /*template <class T>
    bool ParseXmlElement(const tinyxml2::XMLElement* element, std::set<T>& outValue, bool allowDefault = false) {
        outValue.clear();
        if (!element) { return (allowDefault) ? true : SetLastError("Container element for Set of data", "encountered unexpected nullptr"); }
        auto arrayElement = element->FirstChildElement();

        const auto arrayName = (!arrayElement) ? "" : arrayElement->Name();
        while (arrayElement != nullptr) {
            T value;
            if (!ParseXmlElement(arrayElement, value, allowDefault)) { return false; }
            outValue.emplace(value);
            arrayElement = arrayElement->NextSiblingElement(arrayName);
        }

        return true;
    }*/
private:
    bool SetLastError(const std::string& elementName, std::string_view errorMsg);
    bool SetLastError(const tinyxml2::XMLElement* element, std::string_view errorMsg);

    std::string m_lastError;
};
