#include <write_memory_block.h>
#include <error_messages.h>

namespace emessgee
{

WriteMemoryBlock::WriteMemoryBlock(std::string name, size_t buffer_size) : 
    _buffer_size(buffer_size)
{
    if(!std::filesystem::exists(TMP_FOLDER))
    {
        std::filesystem::create_directory(TMP_FOLDER);
    }

    _filepath = utils::string_concat({TMP_FOLDER, name});
    if(std::filesystem::exists(_filepath))
    {
        throw std::runtime_error(FILE_ALREADY_EXISTS);
    }

    _file_descriptor = open(_filepath.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(_file_descriptor, _buffer_size);
    _buffer = (byte*)mmap(NULL, _buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, _file_descriptor, 0);
    if(_buffer == MAP_FAILED)
    {
        throw std::runtime_error(FAILED_TO_CREATE_MMAP);
    }
}

WriteMemoryBlock::~WriteMemoryBlock()
{
    destroy();
}

void WriteMemoryBlock::destroy()
{
    if(_buffer == nullptr)
    {
        return;
    }

    int ret = 0;

    ret = close(_file_descriptor);
    if(ret != 0)
    {
        throw std::runtime_error(FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK);
    }

    ret = std::remove(_filepath.c_str());
    if(ret != 0)
    {
        throw std::runtime_error(FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK);
    }

    _buffer = nullptr;
}

BufferWriteCode WriteMemoryBlock::write(uint index, byte* data, uint size)
{
    if(_buffer == nullptr)
    {
        return BufferWriteCode::BUFFER_NULLPTR;
    }

    if(index >= _buffer_size)
    {
        return BufferWriteCode::INDEX_TOO_LARGE;
    }
    if(index + size > _buffer_size)
    {
        return BufferWriteCode::DATA_TOO_LARGE;
    }

    memmove(_buffer+index, data, size);

    return BufferWriteCode::SUCCESS;
}

byte* WriteMemoryBlock::read(uint index)
{
    if(_buffer == nullptr)
    {
        return nullptr;
    }
    else if(index >= _buffer_size)
    {
        return nullptr;
    }

    return _buffer + index;
}

}