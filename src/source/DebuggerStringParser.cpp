#include "DebuggerStringParser.h"

namespace DebuggerStringParser {
std::vector<std::string> ParseBuffer(const std::string& buffer) {
    std::vector<size_t> wordPos = {};
    size_t pos = 0;
    while (pos != std::string::npos) {
        // Find position of each word
        pos = buffer.find_first_not_of(' ', pos); // Add first none whitespace character
        wordPos.emplace_back(pos);

        if (pos != std::string::npos) {
            pos = buffer.find(' ', pos);
            wordPos.emplace_back(pos);
        }
    }

    // Parse each word
    std::vector<std::string> words;
    size_t count = 0;
    const auto spaceCount = wordPos.size() - 1; // Don't count the starting pos of the first word
    while (count < spaceCount) {
        const auto startPos = wordPos[count++];
        const auto lastPos = wordPos[count++];
        words.emplace_back(buffer.substr(startPos, lastPos - startPos));
    }

    if (words.empty()) {
        words.emplace_back("");
    }

    return words;
}

// TODO: expand this, be nice to handle list such as "1,3-6,-4" = vector of <1,3,5,6>. Negatives are not supported at the moment, should they be?
bool ParseList(const std::string& word, std::vector<unsigned int>& num) {
    size_t start_pos = 0;
    size_t last_pos = 0;
    unsigned int number1{};
    unsigned int number2{};

    num.clear();
    do {
        last_pos = word.find_first_of("-,", start_pos);
        if (!ParseNumber(word.substr(start_pos, last_pos - start_pos), number1)) {
            return false;
        }

        start_pos = last_pos + 1;
        if (last_pos == std::string::npos || word[last_pos] == ',') {
            num.emplace_back(number1);
        }
        else if (word[last_pos] == '-') {
            last_pos = word.find_first_of("-,", start_pos);
            if (!ParseNumber(word.substr(start_pos, last_pos - start_pos), number2)) {
                return false;
            }
            start_pos = last_pos + 1;

            for (unsigned int i = number1; i <= number2; ++i) {
                num.emplace_back(i);
            }
        }
        else {
            return false; // should never reach this
        }
    } while (last_pos != std::string::npos);

    return true;
}

bool ParseNumber(const std::string& word, unsigned int& num) {
    static constexpr std::string_view binaryPrefix = "0b";

    try {
        size_t pos{};
        unsigned long temp{};
        if (word.size() < 1) { return false; }
        if (word.size() > 2 && word.starts_with(binaryPrefix)) { // if binary
            temp = std::stoul(word.substr(2), &pos, 2);
            pos += 2; // stoul is using a substring without the '0b'
        }
        else if (word.front() == '-') { // Currently not supporting negatives
            return false;
        }
        else {
            temp = std::stoul(word, &pos, 0); // else use stoul built in to tell if hex, octal, or decimal
        }

        if (word.size() != pos) {
            return false; // more than just a number
        }
        num = static_cast<unsigned int>(temp); // TODO: revisit.
    }
    catch (const std::exception& /*e*/) { // Invalid number. TODO: may be worth logging info from this exception
        return false;
    }
    return true;
}

bool ParseNumberPair(const std::string& word, unsigned int& num1, unsigned int& num2, std::string_view separatorStr) {
    if (const auto separatorPos = word.find_first_of(separatorStr);
        separatorPos == std::string::npos) {
        return false;
    }
    else {
        if (separatorPos == 0 || (separatorPos + separatorStr.size()) == word.size()) { return false; } // If no number is before or after the Separator
        if (!DebuggerStringParser::ParseNumber(word.substr(0, separatorPos), num1)) { return false; }
        if (!DebuggerStringParser::ParseNumber(word.substr(separatorPos + separatorStr.size()), num2)) { return false; }

        return true;
    }
}
}