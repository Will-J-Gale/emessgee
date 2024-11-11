#include <read_memory_block.h>
#include <error_messages.h>

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

    if(_buffer_size == 0)
    {
        return false;
    }

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

    if(ret != 0)
    {
        throw std::runtime_error(FAILED_TO_DESTROY_READ_MEMORY_BLOCK);
    }

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