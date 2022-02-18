#include "DebuggerStringParser.h"

namespace DebuggerStringParser {
std::vector<std::string> ParseBuffer(const std::string& buffer) {
    std::vector<size_t> wordPos = {};
    size_t pos = 0;
    while (pos != std::string::npos) {
        //Find pos of each word
        pos = buffer.find_first_not_of(" ", pos); //Add first none whitespace character
        wordPos.emplace_back(pos);

        if (pos != std::string::npos) {
            pos = buffer.find(" ", pos);
            wordPos.emplace_back(pos);
        }
    }

    //Parse each word
    std::vector<std::string> words;
    size_t count = 0;
    const auto spaceCount = wordPos.size() - 1; //Don't count the starting pos of the first word
    while (count < spaceCount) {
        const auto startPos = wordPos[count++];
        const auto lastPos = wordPos[count++];
        words.emplace_back(buffer.substr(startPos, lastPos - startPos));
    }

    if (words.empty())
        words.emplace_back("");

    return words;
}

//TODO: expand this, be nice to handle list such as "1,3-6,-4" = vector of <1,3,5,6>. Negatives are not supported at the moment, should they be?
bool ParseList(const std::string& word, std::vector<unsigned int>& num) {
    size_t start_pos = 0;
    size_t last_pos = 0;
    unsigned int number1;
    unsigned int number2;

    num.clear();
    while (last_pos != std::string::npos) {
        last_pos = word.find_first_of("-,", start_pos);
        if (!ParseNumber(word.substr(start_pos, last_pos - start_pos), number1))
            return false;
        start_pos = last_pos + 1;

        if (last_pos == std::string::npos) {
            num.emplace_back(number1);
        }
        else if (word[last_pos] == '-') {
            last_pos = word.find_first_of("-,", start_pos);
            if (!ParseNumber(word.substr(start_pos, last_pos - start_pos), number2))
                return false;
            start_pos = last_pos + 1;

            for (unsigned int i = number1; i <= number2; ++i)
                num.emplace_back(i);
        }
        else if (word[last_pos] == ',') {
            num.emplace_back(number1);
        }
        else {
            return false; //should never reach this
        }
    }

    return true;
}

bool ParseNumber(const std::string& word, unsigned int& num) {
    try {
        size_t pos;
        int temp;
        if (word.size() > 2 && word.substr(0, 2) == std::string("0b")) { //if binary
            temp = std::stoi(word.substr(2), &pos, 2);
            pos += 2; //substr starts 2 chars in
        }
        else
            temp = std::stoi(word, &pos, 0); //else use stoi built in to tell if hex, octal, or decimal
        if (word[pos] != '\0')
            return false; //more than just a number
        if (temp < 0)
            return false;
        num = temp;
    }
    catch (std::exception) { //Invalid num
        return false;
    }
    return true;
}

bool ParseNumberPair(const std::string& word, unsigned int& num1, unsigned int& num2, const char* const separatorStr) {
    if (const auto separator = word.find(separatorStr); separator != std::string::npos) {
        if (separator == 0 || separator == word.back()) { return false; }
        if (!DebuggerStringParser::ParseNumber(word.substr(0, separator), num1)) { return false; }
        if (!DebuggerStringParser::ParseNumber(word.substr(separator + 1), num2)) { return false; }

        return true;
    }
    return false;
}
}