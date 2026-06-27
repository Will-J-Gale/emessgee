#include <csignal>
#include "emessgee/params.h"
#include "emessgee/error_messages.h"
#include <fstream>

namespace emessgee
{

Params::Params()
{

    if(!std::filesystem::exists(TMP_FOLDER))
    {
        std::filesystem::create_directory(TMP_FOLDER);
    }

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

    if(not std::filesystem::exists(key_path))
    {
        std::string message = "Key does not exist: " + key;
        throw std::runtime_error(message);
    }

    FileLock lock(key_path);

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

    file.write(data, size);
    lock.close();
    file.close();

    return BufferWriteCode::SUCCESS;
}

BufferWriteCode Params::write_string_list(const std::string& key, std::vector<std::string>& data_list)
{
    Path key_path = get_key_path(key);

    FileLock lock(key_path);
    std::ofstream file(key_path);

    if(!file.is_open())
    {
        lock.close();
        return BufferWriteCode::FAILED;
    }

    std::stringstream write_data;
    size_t data_count = data_list.size();
    write_data.write(reinterpret_cast<char*>(&data_count), sizeof(size_t));

    for(const std::string& data : data_list)
    {
        size_t data_size = data.size();
        write_data.write(reinterpret_cast<char*>(&data_size), sizeof(size_t));
        write_data.write(data.c_str(), data.size());
    }

    file.write(write_data.str().c_str(), write_data.str().size());

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

std::vector<std::string> Params::read_string_list(const std::string& key)
{
    Path key_path = get_key_path(key);

    if(not std::filesystem::exists(key_path))
    {
        std::string message = "Key does not exist: " + key;
        throw std::runtime_error(message);
    }

    FileLock lock(key_path);
    std::ifstream file(key_path, std::ios::ate);

    if(!file.is_open())
    {
        lock.close();
    }

    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer;
    buffer.resize(file_size);
    file.read(buffer.data(), file_size);

    size_t data_count = 0;
    memcpy(&data_count, buffer.data(), sizeof(size_t));
    char* data_ptr = buffer.data() + sizeof(size_t);

    std::vector<std::string> result;

    for(size_t i = 0; i < data_count; i++)
    {
        size_t data_size = 0;
        memcpy(&data_size, data_ptr, sizeof(size_t));
        data_ptr += sizeof(size_t);

        std::string data(data_ptr, data_size);
        result.push_back(data);
        data_ptr += data_size;
    }

    lock.close();
    return result;
}

bool Params::delete_key(const std::string& key)
{
    Path key_path = get_key_path(key);
    
    if(not std::filesystem::exists(key_path))
    {
        return false;
    }

    FileLock lock(key_path);
    std::filesystem::remove(key_path);
    lock.close();

    return true;
}

Path Params::get_key_path(const std::string& key)
{
    return Path(PARAMS_PATH) / key;
}

}