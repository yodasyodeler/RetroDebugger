#include "XmlParserException.h"

#include <fmt/core.h>
#include <tinyxml2.h>

#define RDB_DEBUG_FILE_LOCATION (true)

std::string FormatFileLocation(const std::source_location& location) {
#if RDB_DEBUG_FILE_LOCATION
    return fmt::format("XML Error {}({}): ", location.file_name(), location.column());
#else
    using namespace std::string_literals;
    return "XML Error: "s;
#endif
}

XmlParserException XmlParserException::CreateError(const std::string& errorMsg, std::source_location location) {
    return XmlParserException(fmt::format("{}{}", FormatFileLocation(location), errorMsg));
}

XmlParserException XmlParserException::CreateError(std::string_view elementName, const std::string& errorMsg, std::source_location location) {

    return XmlParserException(fmt::format("{}\"{}\", {}", FormatFileLocation(location), elementName, errorMsg));
}

XmlParserException XmlParserException::CreateError(const tinyxml2::XMLElement* element, const std::string& errorMsg, std::source_location location) {
    const auto* name = (element->Name() != nullptr) ? element->Name() : "UnkownElement";
    return XmlParserException(fmt::format("{}\"{}\" at line {}, {}", FormatFileLocation(location), name, element->GetLineNum(), errorMsg));
}

XmlParserException XmlParserException::CreateError(std::string_view elementName, std::string_view errorMsg, std::source_location location) {
    return XmlParserException(fmt::format("{}\"{}\", {}", FormatFileLocation(location), elementName, errorMsg));
}

XmlParserException XmlParserException::CreateError(const tinyxml2::XMLElement* element, std::string_view errorMsg, std::source_location location) {
    const auto* name = (element->Name() != nullptr) ? element->Name() : "UnkownElement";
    return XmlParserException(fmt::format("{}\"{}\" at line {}, {}", FormatFileLocation(location), name, element->GetLineNum(), errorMsg));
}

XmlParserException::XmlParserException(const std::string& str) :
    std::runtime_error(str) {}