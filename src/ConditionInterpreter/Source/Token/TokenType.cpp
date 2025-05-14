#include "TokenType.h"

#include <magic_enum/magic_enum.hpp>

std::string_view to_string(const TokenType tokenType) {
    return magic_enum::enum_name(tokenType);
}