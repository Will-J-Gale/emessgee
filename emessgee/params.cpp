#include <csignal>
#include "emessgee/params.h"
#include "emessgee/error_messages.h"
#include <fstream>

namespace emessgee
{

Params::Params()
{
    std::atexit(params_exit_handler);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGKILL, signal_handler);

    if(!std::filesystem::exists(PARAMS_PATH))
    {
        std::filesystem::create_directory(PARAMS_PATH);
    }

    if(check_key(PARAMS_COUNT_KEY))
    {
        int count = read_int(PARAMS_COUNT_KEY);
        write_int(PARAMS_COUNT_KEY, count + 1);
    }
    else
    {
        utils::empty_dir(PARAMS_PATH);
        write_int(PARAMS_COUNT_KEY, 1);
    }
}

Params::~Params()
{
    close();
}

void Params::close()
{
    if(_closed)
    {
        return;
    }
    if(check_key(PARAMS_COUNT_KEY))
    {
        int count = read_int(PARAMS_COUNT_KEY);
        if(count == 1)
        {
            utils::empty_dir(PARAMS_PATH);
        }
        else
        {
            write_int(PARAMS_COUNT_KEY, count - 1);
        }
    }
    else
    {
        std::runtime_error("Expected count key to exist when destroying params instance!");
    }

    _closed = true;
}

bool Params::check_key(const std::string& key)
{
    Path key_path = get_key_path(key);
    return std::filesystem::exists(key_path);
}

std::string Params::read_string(const std::string& key)
{
    Path key_path = get_key_path(key);
    FileLock lock(key_path);

    if(not std::filesystem::exists(key_path))
    {
        std::string message = "Key does not exist: " + key;
        throw std::runtime_error(message);
    }

    std::ifstream file(key_path);
    std::string result;

    if(!file.is_open())
    {
        lock.close();
        return std::string();
    }

    std::getline(file, result);
    lock.close();

    return result;
}

BufferWriteCode Params::write_string(const std::string& key, const std::string& data)
{
    Path key_path = get_key_path(key);

    FileLock lock(key_path);
    std::ofstream file(key_path);
    
    if(!file.is_open())
    {
        lock.close();
        return BufferWriteCode::FAILED;
    }

    file << data;
    lock.close();
    file.close();

    return BufferWriteCode::SUCCESS;
}

void Params::read_bytes(const std::string& key, char* dst, size_t size)
{
    assert(dst != nullptr);
    Path key_path = get_key_path(key);
    FileLock lock(key_path);

    if(not std::filesystem::exists(key_path))
    {
        std::string message = "Key does not exist: " + key;
        throw std::runtime_error(message);
    }

    std::ifstream file(key_path);
    std::string result;

    if(!file.is_open())
    {
        lock.close();
    }

    file.read((char*)dst, size);
    lock.close();
}

BufferWriteCode Params::write_bytes(const std::string& key, const char* data, size_t size)
{
    Path key_path = get_key_path(key);

    FileLock lock(key_path);
    std::ofstream file(key_path, std::ios::binary);
    
    if(!file.is_open())
    {
        lock.close();
        return BufferWriteCode::FAILED;
    }

    file.write((const char *)data, size);
    lock.close();
    file.close();

    return BufferWriteCode::SUCCESS;
}

bool Params::read_bool(const std::string& key)
{
    std::string value = read_string(key);
    return (bool)std::stoi(value);
}

int Params::read_int(const std::string& key)
{
    std::string value = read_string(key);
    return std::stoi(value);
}

float Params::read_float(const std::string& key)
{
    std::string value = read_string(key);
    return std::stof(value);
}

double Params::read_double(const std::string& key)
{
    std::string value = read_string(key);
    return std::stod(value);
}

Path Params::get_key_path(const std::string& key)
{
    return Path(PARAMS_PATH) / key;
}

}