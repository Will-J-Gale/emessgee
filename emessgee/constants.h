#pragma once

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <string>

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
constexpr char PARAMS_ENV_KEY[] = "EMESSGEE_PARAMS_DIR";
const std::string PARAMS_PATH = std::getenv(PARAMS_ENV_KEY) != nullptr ? std::getenv(PARAMS_ENV_KEY) : "/tmp/emessgee/params";
constexpr char PARAMS_COUNT_KEY[] = "num_params_instances";
constexpr char LOCK_EXT[] = ".lock";

}