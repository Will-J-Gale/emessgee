#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <filesystem>
#include <initializer_list>

#include "emessgee/constants.h"

namespace emessgee
{

namespace utils
{
    inline std::string string_concat(std::initializer_list<std::string> values)
    {
        std::stringstream output;
        for(auto value : values)
        {
            output << value;
        }

        return output.str();
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

    inline std::string pad_string(std::string str, uint size, std::string pad_value=PARAMS_KEY_PAD)
    {
        int remaining = size - str.size();
        std::string string_pad = str;

        for(int i = 0; i < remaining; i++)
        {
            string_pad.append(pad_value);
        }
        return string_pad;
    }
}

}