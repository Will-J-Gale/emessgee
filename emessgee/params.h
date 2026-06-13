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

    bool check_key(const std::string& key);
    BufferWriteCode write_bool(const std::string& key, bool data) {return write(key, data); };
    BufferWriteCode write_int(const std::string& key, int data) {return write(key, data); };
    BufferWriteCode write_float(const std::string& key, float data) {return write(key, data); };
    BufferWriteCode write_string(const std::string& key, std::string data);
    bool read_bool(const std::string& key) {return read<bool>(key); };
    int read_int(const std::string& key) {return read<int>(key); };
    float read_float(const std::string& key) {return read<float>(key); };
    std::string read_string(const std::string& key);
    std::tuple<byte*, uint> read_addr(std::string& key);

private:
    byte* get_end();   
    BufferWriteCode write(const std::string& key, void* data, uint size);
    
    template<typename T>
    BufferWriteCode write(const std::string& key, T data)
    {
        return write(key, (void*)&data, sizeof(T));
    }

    template<typename T>
    T read(const std::string& key)
    {
        std::string key_pad = utils::pad_string(key, PARAMS_KEY_LEN);
        auto [value_addr, length] = read_addr(key_pad);
        T value;
        memcpy(&value, value_addr, length);
        return value;
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