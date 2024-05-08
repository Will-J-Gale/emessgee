#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <initializer_list>

namespace emessgee
{

namespace utils
{
    inline std::string string_concat(std::initializer_list<std::string> values)
    {
        std::string output;
        for(auto value : values)
        {
            output += value;
        }

        return output;
    };
}

}