#pragma once

#include <cassert>
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
    //@TODO HANDLE??!
    // if(std::filesystem::exists(PARAMS_PATH))
    // {
    //     std::filesystem::remove_all(PARAMS_PATH);
    // }
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

    Params();
    ~Params();

    bool check_key(const std::string& key);
    BufferWriteCode write_bool(const std::string& key, bool data) {return write_string(key, std::to_string(data)); };
    BufferWriteCode write_int(const std::string& key, int data) {return write_string(key, std::to_string(data)); };
    BufferWriteCode write_float(const std::string& key, float data) {return write_string(key, std::to_string(data)); };
    BufferWriteCode write_double(const std::string& key, double data) {return write_string(key, std::to_string(data)); };
    BufferWriteCode write_string(const std::string& key, const std::string& data);
    bool read_bool(const std::string& key);
    int read_int(const std::string& key);
    float read_float(const std::string& key);
    double read_double(const std::string& key);
    std::string read_string(const std::string& key);
    void close();

private:
    Path get_key_path(const std::string& key);
    
private:
    bool _closed = false;
};

}