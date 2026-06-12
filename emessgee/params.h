#pragma once

#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdio>
#include <cassert>
#include <map>
#include <filesystem>
#include <cstdlib>
#include <functional>
#include <cstring>
#include <memory>
#include <tuple>

#include "emessgee/utils.h"
#include "emessgee/typedefs.h"
#include "emessgee/constants.h"
#include "emessgee/error_messages.h"
#include "emessgee/buffer_write_code.h"

namespace emessgee
{

inline void params_exit_handler()
{
    if(std::filesystem::exists(PARAMS_PATH))
    {
        std::filesystem::remove(PARAMS_PATH);
    }
}

inline void signal_handler(int signal)
{
    params_exit_handler();
}

template<typename T>
struct Param
{
    bool valid = false;
    ReadResultCode code;
    T value = T();
};

struct ParamsMetadata
{
    uint params_count = 0;
    uint param_writer_count = 0;
    bool writing = false;
}; 

class Params
{
public:
    using Ptr = std::unique_ptr<Params>;

    Params(size_t buffer_size=PARAMS_BUFFER_SIZE);
    ~Params();
    void destroy();

    template<typename T>
    BufferWriteCode write(std::string& key, T& data)
    {
        return write(key, (void*)&data, sizeof(T));
    }

    template<typename T>
    BufferWriteCode write(std::string& key, T&& data)
    {
        return write(key, (void*)&data, sizeof(T));
    }

    BufferWriteCode write(std::string& key, std::string& data)
    {
        return write(key, const_cast<char*>(data.c_str()), data.size());
    }

    BufferWriteCode write(std::string& key, void* data, uint size)
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
            byte* end_addr = end();
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

    template<typename T>
    Param<T> read(std::string& key)
    {
        Param<T> result;
        std::string key_pad = utils::pad_string(key, PARAMS_KEY_LEN);

        if(_buffer == nullptr)
        {
            result.code = ReadResultCode::BUFFER_NULLPTR;
            return result;
        }

        if(_key_addrs.count(key_pad) == 0)
        {
            result.code = ReadResultCode::KEY_DOES_NOT_EXIST;
            return result;
        }

        auto [value_addr, length] = read_raw(key_pad);

        memcpy(&result.value, value_addr, length);
        result.valid = true;
        result.code = ReadResultCode::SUCCESS;

        return result;
    }

    Param<std::string> read(std::string& key) = delete;

    Param<std::string> read_string(std::string& key)
    {
        Param<std::string> result;
        std::string key_pad = utils::pad_string(key, PARAMS_KEY_LEN);

        if(_buffer == nullptr)
        {
            result.code = ReadResultCode::BUFFER_NULLPTR;
            return result;
        }

        if(_key_addrs.count(key_pad) == 0)
        {
            result.code = ReadResultCode::KEY_DOES_NOT_EXIST;
            return result;
        }

        auto [value_addr, length] = read_raw(key_pad);

        std::string value(reinterpret_cast<const char*>(value_addr), length);

        result.value = std::string(reinterpret_cast<const char*>(value_addr), length);
        result.valid = true;
        result.code = ReadResultCode::SUCCESS;

        return result;
    }
    
    std::tuple<byte*, uint> read_raw(std::string& key)
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

private:
    byte* end()
    {
        byte* end_addr = _buffer + _data_start;

        for(const auto& kv : _key_addrs)
        {
            std::string& key = const_cast<std::string&>(kv.first);
            auto [value_addr, length] = read_raw(key);

            if(value_addr + length > end_addr)
            {
                end_addr = value_addr + length;
            }
        }

        return end_addr;
    }

private:
    byte* _buffer = nullptr;
    int _file_descriptor = -1;
    uint _buffer_size = 0;
    std::map<std::string, byte*> _key_addrs;
    ParamsMetadata* _metadata = nullptr;
    uint _data_start = 0;
};

}