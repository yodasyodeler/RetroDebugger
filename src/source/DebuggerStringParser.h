#pragma once

#include <string>
#include <vector>

namespace DebuggerStringParser {
std::vector<std::string> ParseBuffer(const std::string& buffer);
bool ParseList(const std::string& word, std::vector<unsigned int>& num);
bool ParseNumber(const std::string& word, unsigned int& num);
bool ParseNumberPair(const std::string& word, unsigned int& num1, unsigned int& num2, const char* separatorStr);
}