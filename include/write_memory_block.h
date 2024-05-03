#pragma once

#include <constants.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdio>
#include <cassert>
#include <filesystem>
#include <cstdlib>
#include <functional>
#include <cstring>

#include <utils.h>
#include <typedefs.h>

enum BufferWriteCode
{
    SUCCESS=0,
    BUFFER_NULLPTR=1,
    INDEX_TO_LARGE=2
};

class WriteMemoryBlock
{
public:
    WriteMemoryBlock(std::string name, int buffer_size);
    ~WriteMemoryBlock();
    void destroy();
    BufferWriteCode write(uint index, byte data);
    BufferWriteCode write_bytes(uint index, byte* data, uint size);

private:
    byte* _buffer = nullptr;
    std::filesystem::path _filepath = "";
    int _file_descriptor = -1;
    uint _buffer_size = 0;
};