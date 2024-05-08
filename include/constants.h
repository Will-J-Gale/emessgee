#pragma once

#include <cstdlib>
#include <iostream>
#include <filesystem>

namespace emessgee
{

constexpr char TMP_FOLDER[] = "/tmp/emessgee/";
constexpr uint DEFAULT_BUFFER_SIZE = uint(1e3);
constexpr uint DEFAULT_QUEUE_SIZE = 1;
constexpr uint ID_LEN = 16;
constexpr uint INVALID_ID = 0;
constexpr uint INVALID_INDEX = 0;
constexpr uint INVALID_SIZE = 0;
constexpr uint MAX_SANITY_LOOPS = 10000;

}