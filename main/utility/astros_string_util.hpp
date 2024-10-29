#ifndef __ASTROS_STRING_UTIL_HPP__
#define __ASTROS_STRING_UTIL_HPP__

#include <string>
#include <vector>

class AstrOsStringUtils
{

public:
    static std::vector<std::string> splitString(std::string str, char delimiter)
    {
        std::vector<std::string> parts;
        auto start = 0U;
        auto end = str.find(delimiter);
        while (end != std::string::npos)
        {
            parts.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }

        parts.push_back(str.substr(start, end));

        return parts;
    }
};

#endif