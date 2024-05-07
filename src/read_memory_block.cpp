#include <read_memory_block.h>

ReadMemoryBlock::ReadMemoryBlock(std::string name)
{
        _filepath = utils::string_concat({TMP_FOLDER, name});
    assert(std::filesystem::exists(_filepath) == true);

    _file_descriptor = open(_filepath.c_str(), O_RDONLY);
    _buffer_size = std::filesystem::file_size(_filepath);
    _buffer = (byte*)mmap(NULL, _buffer_size, PROT_READ, MAP_SHARED, _file_descriptor, 0);
}

ReadMemoryBlock::~ReadMemoryBlock()
{
    destroy();
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
    if(index >= _buffer_size)
    {
        return nullptr;
    }

    return _buffer + index;
}