#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdio>
#include <cassert>
#include <filesystem>
#include <cstdlib>
#include <functional>
#include <cstring>
#include <memory>

#include <utils.h>
#include <typedefs.h>
#include <constants.h>
#include <buffer_write_code.h>

namespace emessgee
{

class WriteMemoryBlock
{
public:
    using Ptr = std::unique_ptr<WriteMemoryBlock>;

    WriteMemoryBlock(std::string name, int buffer_size);
    ~WriteMemoryBlock();
    void destroy();
    BufferWriteCode write(uint index, byte data);
    BufferWriteCode write_bytes(uint index, byte* data, uint size);
    byte* read(uint index);

private:
    byte* _buffer = nullptr;
    std::filesystem::path _filepath = "";
    int _file_descriptor = -1;
    uint _buffer_size = 0;
};

}