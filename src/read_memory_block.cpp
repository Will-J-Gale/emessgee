#include <read_memory_block.h>

namespace emessgee
{

ReadMemoryBlock::ReadMemoryBlock(std::string name)
{
    _filepath = utils::string_concat({TMP_FOLDER, name});
    initialize();
}

ReadMemoryBlock::~ReadMemoryBlock()
{
    destroy();
}

bool ReadMemoryBlock::initialize()
{
    if(!std::filesystem::exists(_filepath))
    {
        return false;
    }

    _file_descriptor = open(_filepath.c_str(), O_RDONLY);
    _buffer_size = std::filesystem::file_size(_filepath);
    _buffer = (byte*)mmap(NULL, _buffer_size, PROT_READ, MAP_SHARED, _file_descriptor, 0);

    return true;
}

bool ReadMemoryBlock::is_initialized()
{
    return _buffer != nullptr;
}

void ReadMemoryBlock::destroy()
{
    if(_buffer == nullptr)
    {
        return;
    }

    int ret = close(_file_descriptor);
    assert(ret == 0);

    _buffer = nullptr;
}

byte* ReadMemoryBlock::read(uint index)
{
    if(_buffer == nullptr)
    {
        if(!initialize())
        {
            return nullptr;
        }
    }

    if(index >= _buffer_size)
    {
        return nullptr;
    }

    return _buffer + index;
}

}