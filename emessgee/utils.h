#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <filesystem>
#include <initializer_list>

#include <constants.h>

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

    inline void clean_temp_folder()
    {
        for (const auto& entry : std::filesystem::directory_iterator(TMP_FOLDER))
        {
            std::remove(entry.path().c_str());
        } 
    }

    inline void create_tmp_folder()
    {
        if(!std::filesystem::exists(TMP_FOLDER))
        {
            std::filesystem::create_directory(TMP_FOLDER);
        }
    }
}

}