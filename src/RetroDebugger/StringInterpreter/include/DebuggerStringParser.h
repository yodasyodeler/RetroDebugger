#pragma once

#include <string>
#include <string_view>
#include <vector>

// TODO: Think about the types here. unsigned long may make more sense? Also maybe add length checks, check the string to integer method used.
// TODO: Document these methods.
namespace DebuggerStringParser {
std::vector<std::string> ParseBuffer(const std::string& buffer);
bool ParseList(const std::string& word, std::vector<unsigned int>& num);
bool ParseNumber(const std::string& word, unsigned int& num);
bool ParseNumberPair(const std::string& word, unsigned int& num1, unsigned int& num2, std::string_view separatorStr);
}