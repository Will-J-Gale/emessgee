#pragma once

#include <cstdlib>
#include <iostream>
#include <filesystem>

enum ReservedIndexes
{
    BLOCK_READY=0,
    WRITING=1,
    QUEUE_SIZE=2,
    RESERVED_SIZE
};

constexpr char TMP_FOLDER[] = "/tmp/emessgee/";
constexpr uint DEFAULT_BUFFER_SIZE = uint(1e3);
constexpr uint DEFAULT_QUEUE_SIZE = 1;
constexpr uint ID_LEN = 16;
// HEADER_FORMAT = f">II{ID_LEN}s"
// HEADER_LENGTH = calcsize(HEADER_FORMAT)
constexpr int HEADER_START = ReservedIndexes::RESERVED_SIZE;
// constexpr char ID_BYTES_ENDIEN[] = "big";
constexpr uint INVALID_ID = 0;
constexpr uint INVALID_INDEX = 0;
constexpr uint INVALID_SIZE = 0;
constexpr uint MAX_SANITY_LOOPS = 10000;

