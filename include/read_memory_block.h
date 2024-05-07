#pragma once

#include <string>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#include <sys/mman.h>
#include <cassert>
#include <memory>

#include <typedefs.h>
#include <utils.h>
#include <constants.h>

class ReadMemoryBlock
{
public:
    using Ptr = std::unique_ptr<ReadMemoryBlock>;

    ReadMemoryBlock(std::string name);
    ~ReadMemoryBlock();
    void destroy();
    byte* read(uint index);

private:
    byte* _buffer = nullptr;
    std::filesystem::path _filepath = "";
    int _file_descriptor = -1;
    uint _buffer_size = 0;
};