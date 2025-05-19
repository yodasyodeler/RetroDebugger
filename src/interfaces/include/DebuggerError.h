#pragma once

#include <stdexcept>

namespace Rdb {

class DebuggerError : public std::runtime_error {
public:
    DebuggerError(const std::string& errorMessage) :
        std::runtime_error(errorMessage) {}
};

}