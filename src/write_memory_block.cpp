#include <write_memory_block.h>

WriteMemoryBlock::WriteMemoryBlock(std::string name, int buffer_size) : 
    _buffer_size(buffer_size)
{
    if(!std::filesystem::exists(TMP_FOLDER))
    {
        std::filesystem::create_directory(TMP_FOLDER);
    }

    _filepath = utils::string_concat({TMP_FOLDER, name});
    assert(std::filesystem::exists(_filepath) == false);

    _file_descriptor = open(_filepath.c_str(), O_CREAT | O_RDWR);
    ftruncate(_file_descriptor, _buffer_size);
    _buffer = (byte*)mmap(NULL, _buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, _file_descriptor, 0);
    assert(_buffer != MAP_FAILED);
}

WriteMemoryBlock::~WriteMemoryBlock()
{
    destroy();
}

void WriteMemoryBlock::destroy()
{
    int ret = 0;

    ret = close(_file_descriptor);
    assert(ret == 0);

    ret = std::remove(_filepath.c_str());
    assert(ret == 0);
}


BufferWriteCode WriteMemoryBlock::write(uint index, byte data)
{
    if(_buffer == nullptr)
    {
        return BufferWriteCode::BUFFER_NULLPTR;
    }

    if(index > _buffer_size)
    {
        return BufferWriteCode::INDEX_TO_LARGE;
    }

    _buffer[index] = data;

    return BufferWriteCode::SUCCESS;
}

BufferWriteCode WriteMemoryBlock::write_bytes(uint index, byte* data, uint size)
{
    if(_buffer == nullptr)
    {
        return BufferWriteCode::BUFFER_NULLPTR;
    }

    if(index + size > _buffer_size)
    {
        return BufferWriteCode::INDEX_TO_LARGE;
    }

    memcpy(_buffer+index, data, size);

    return BufferWriteCode::SUCCESS;
}
