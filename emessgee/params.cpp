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

bool Params::check_key(const std::string& key)
{
    std::string key_pad = utils::pad_string(key, PARAMS_KEY_LEN);

    if(_key_addrs.count(key_pad) == 0)
    {
        return false;
    }

    return true;
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

BufferWriteCode Params::write_string(const std::string& key, std::string data)
{
    return write(key, const_cast<char*>(data.c_str()), data.size());
}

std::string Params::read_string(const std::string& key)
{
    std::string key_pad = utils::pad_string(key, PARAMS_KEY_LEN);
    auto [value_addr, length] = read_addr(key_pad);
    std::string value(reinterpret_cast<const char*>(value_addr), length);

    return value;
}

std::tuple<byte*, uint> Params::read_addr(std::string& key)
{
    std::string key_pad = key;

    if(key.size() != PARAMS_KEY_LEN)
    {
        key_pad = utils::pad_string(key, PARAMS_KEY_LEN);
    }

    byte* key_addr = _key_addrs[key_pad];
    byte* length_addr = key_addr + key_pad.size();
    byte* value_addr = length_addr + sizeof(uint);
    
    uint length;
    memcpy(&length, length_addr, sizeof(uint));

    return {value_addr, length};
}

byte* Params::get_end()
{
    byte* end_addr = _buffer + _data_start;

    for(const auto& kv : _key_addrs)
    {
        std::string& key = const_cast<std::string&>(kv.first);
        auto [value_addr, length] = read_addr(key);

        if(value_addr + length > end_addr)
        {
            end_addr = value_addr + length;
        }
    }

    return end_addr;
}

BufferWriteCode Params::write(const std::string& key, void* data, uint size)
{
    assert(key.size() < PARAMS_KEY_LEN);

    if(_buffer == nullptr)
    {
        return BufferWriteCode::BUFFER_NULLPTR;
    }

    _metadata->writing = true;

    std::string key_pad = utils::pad_string(key, PARAMS_KEY_LEN);

    if(_key_addrs.count(key_pad) == 0)
    {
        byte* end_addr = get_end();
        _key_addrs.insert({key_pad, end_addr});
        _metadata->params_count += 1;
    }

    byte* key_addr = _key_addrs[key_pad];
    byte* length_addr = key_addr + key_pad.size();
    byte* value_addr = length_addr + sizeof(uint);

    memcpy(key_addr, key_pad.c_str(), key_pad.size());
    memcpy(length_addr, &size, sizeof(uint));
    memcpy(value_addr, data, size);

    _metadata->writing = false;
    return BufferWriteCode::SUCCESS;
}

}