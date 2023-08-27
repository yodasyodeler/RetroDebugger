#pragma once

#include "DebuggerCommon.h"

namespace tinyxml2 {
class XMLElement;
}

class XmlElementParser {
public:
    void ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperations& operations);
    void ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerOperation& operation);
    void ParseXmlElement(const tinyxml2::XMLElement* element, XmlDebuggerArgument& argument);
    // void ParseXmlElement(const tinyxml2::XMLElement* element, std::string& outValue, bool allowDefault = false);
    // void ParseXmlElement(const tinyxml2::XMLElement* element, unsigned int& outValue, bool allowDefault = false);
    // void ParseXmlElement(const tinyxml2::XMLElement* element, FlagType& outValue, bool allowDefault = false);

    /*template <class T>
    void ParseXmlElement(const tinyxml2::XMLElement* element, std::set<T>& outValue, bool allowDefault = false) {
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
};
