#include <source_location>
#include <stdexcept>

namespace tinyxml2 {
class XMLElement;
}

class XmlParserException : public std::runtime_error {
public:
    static XmlParserException CreateError(const std::string& errorMsg, std::source_location location = std::source_location::current());
    static XmlParserException CreateError(std::string_view elementName, const std::string& errorMsg, std::source_location location = std::source_location::current());
    static XmlParserException CreateError(const tinyxml2::XMLElement* element, const std::string& errorMsg, std::source_location location = std::source_location::current());
    static XmlParserException CreateError(std::string_view elementName, std::string_view errorMsg, std::source_location location = std::source_location::current());
    static XmlParserException CreateError(const tinyxml2::XMLElement* element, std::string_view errorMsg, std::source_location location = std::source_location::current());

    XmlParserException(const std::string& str);
};