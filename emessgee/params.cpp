#include <csignal>
#include "emessgee/params.h"
#include "emessgee/error_messages.h"

namespace emessgee
{

Params::Params(size_t buffer_size) : 
    _buffer_size(buffer_size)
{
    std::atexit(params_exit_handler);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGKILL, signal_handler);

    if(!std::filesystem::exists(TMP_FOLDER))
    {
        std::filesystem::create_directory(TMP_FOLDER);
    }

    bool populate_params = false;

    if(!std::filesystem::exists(PARAMS_PATH))
    {
        _file_descriptor = open(PARAMS_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        ftruncate(_file_descriptor, _buffer_size);
    }
    else
    {
        _file_descriptor = open(PARAMS_PATH, O_RDWR, S_IRUSR | S_IWUSR);
        populate_params = true;
    }

    _buffer = (byte*)mmap(NULL, _buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, _file_descriptor, 0);
    if(_buffer == MAP_FAILED)
    {
        throw std::runtime_error(FAILED_TO_CREATE_MMAP);
    }

    _metadata = reinterpret_cast<ParamsMetadata*>(_buffer);
    _data_start = sizeof(ParamsMetadata);

    if(populate_params)
    {
        byte* current_addr = _buffer + _data_start;

        for(size_t i = 0; i < _metadata->params_count; i++)
        {
            byte* key_addr = current_addr;
            byte* length_addr = key_addr + PARAMS_KEY_LEN;
            byte* value_index = length_addr + sizeof(uint);

            std::string key = std::string(key_addr, key_addr + PARAMS_KEY_LEN);
            uint length;

            memcpy(&length, length_addr, sizeof(uint));

            _key_addrs.insert({key, key_addr});
            current_addr = value_index + length;
        }
    }

    _metadata->param_writer_count += 1;
}

Params::~Params()
{
    destroy();
}

void Params::destroy()
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

    _metadata->param_writer_count -= 1;

    if(_metadata->param_writer_count == 0)
    {

        ret = std::remove(PARAMS_PATH);
        if(ret != 0)
        {
            throw std::runtime_error(FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK);
        }
    }

    _buffer = nullptr;
}

}