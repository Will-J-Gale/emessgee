#include "gtest/gtest.h"
#include <map>
#include <filesystem>

#include "emessgee/emessgee.h"
#include "emessgee/constants.h"
#include "emessgee/params.h"

TEST(ParamsTest, successfully_writes_and_reads_parameter)
{
    //Assemble
    std::string key = "key";
    int value = 13567;
    emessgee::Params params;

    //Act
    emessgee::BufferWriteCode write_result = params.write_int(key, value);
    int read_result = params.read_int(key);
    
    //Assert
    EXPECT_EQ(write_result, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(read_result, value);
}

TEST(ParamsTest, successfully_writes_and_reads_multiple_parameter)
{
    //Assemble
    std::string key_1 = "key_1";
    std::string key_2 = "key_2";
    std::string key_3 = "key_3";
    std::string key_4 = "key_4";
    std::string key_5 = "key_5";

    int value_1 = 13567;
    bool value_2 = true;
    float value_3 = 3.141592;
    std::string value_4 = "hello-there";

    emessgee::Params params;

    //Act
    emessgee::BufferWriteCode write_result_1 = params.write_int(key_1, value_1); //Int
    emessgee::BufferWriteCode write_result_2 = params.write_bool(key_2, value_2); //bool
    emessgee::BufferWriteCode write_result_3 = params.write_float(key_3, value_3); //float
    emessgee::BufferWriteCode write_result_4 = params.write_string(key_4, value_4); //string
    emessgee::BufferWriteCode write_result_5 = params.write_int(key_5, 9999); //rvalue

    int read_result_1 = params.read_int(key_1);
    bool read_result_2 = params.read_bool(key_2);
    float read_result_3 = params.read_float(key_3);
    std::string read_result_4 = params.read_string(key_4);
    int read_result_5 = params.read_int(key_5);
    
    //Assert
    EXPECT_EQ(write_result_1, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_2, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_3, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_4, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_5, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(read_result_1, value_1);
    EXPECT_EQ(read_result_2, value_2);
    EXPECT_EQ(read_result_3, value_3);
    EXPECT_EQ(read_result_4, value_4);
    EXPECT_EQ(read_result_5, 9999);
}

TEST(ParamsTest, multiple_params_second_successfully_reads_keys_from_buffer)
{
    //Assemble
    std::string key_1 = "key_1";
    std::string key_2 = "key_2";
    std::string key_3 = "key_3";
    std::string key_4 = "key_4";

    int value_1 = 13567;
    bool value_2 = true;
    float value_3 = 3.141592;
    std::string value_4 = "hello-there";

    emessgee::Params params;
    params.write_int(key_1, value_1);
    params.write_bool(key_2, value_2);
    params.write_float(key_3, value_3);
    params.write_string(key_4, value_4);

    emessgee::Params params_2;

    //Act
    int read_result_1 = params_2.read_int(key_1);
    bool read_result_2 = params_2.read_bool(key_2);
    float read_result_3 = params_2.read_float(key_3);
    std::string read_result_4 = params_2.read_string(key_4);
    
    //Assert
    EXPECT_EQ(read_result_1, value_1);
    EXPECT_EQ(read_result_2, value_2);
    EXPECT_EQ(read_result_3, value_3);
    EXPECT_EQ(read_result_4, value_4);
}

TEST(ParamsTest, check_key_key_exists_returns_true)
{
    //Assemble
    std::string key = "key_that_does_exist";
    int value = 13567;
    emessgee::Params params;

    //Act
    emessgee::BufferWriteCode write_result = params.write_int(key, value);
    bool result = params.check_key(key);
    
    //Assert
    EXPECT_EQ(write_result, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_TRUE(result);
}

TEST(ParamsTest, check_key_key_does_not_exist_returns_false)
{
    //Assemble
    std::string key = "key_that_does_not_exist";
    emessgee::Params params;

    //Act
    bool result = params.check_key(key);
    
    //Assert
    EXPECT_FALSE(result);
}

TEST(ParamsTest, try_read_before_write)
{
    //Assemble
    std::string key = "key_read_before_writing";
    emessgee::Params params;

    //Act/Assert
    EXPECT_THROW(params.read_int(key), std::runtime_error);
}

TEST(ParamsTest, multiple_params_counter_incremeneted_and_removed_once_all_closed)
{
    //Assemble
    emessgee::Params params_1;
    emessgee::Params params_2;
    emessgee::Params params_3;
    emessgee::Params params_4;

    //Act
    int result = params_1.read_int(emessgee::PARAMS_COUNT_KEY);
    params_1.close();
    params_2.close();
    params_3.close();
    params_4.close();
    
    //Assert
    EXPECT_EQ(result, 4);
    EXPECT_FALSE(std::filesystem::exists(emessgee::Path(emessgee::PARAMS_PATH) / emessgee::PARAMS_COUNT_KEY));
}

TEST(ParamsTest, successfully_writes_and_reads_byte_data)
{
    //Assemble
    std::string key = "key_bytes";
    std::string data = "ijrnbvikjrewnbvc";
    char* read_data = (char*)malloc(data.size());
    emessgee::Params params;

    //Act
    emessgee::BufferWriteCode write_result = params.write_bytes(key, data.c_str(), data.size());
    params.read_bytes(key, read_data, data.size());;
    std::string read_result(read_data, data.size());
    
    //Assert
    EXPECT_EQ(write_result, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(read_result, data);
}

TEST(ParamsTest, successfully_writes_and_reads_struct_as_byte_data)
{
    struct Data
    {
        int x = 0;
        float y = 0.0f;
        bool test = false;
    };

    //Assemble
    std::string key = "key_struct_bytes";
    Data data = {
        .x = 100,
        .y = 3.14,
        .test = true
    };
    Data read_result;

    emessgee::Params params;

    //Act
    emessgee::BufferWriteCode write_result = params.write_bytes(key, reinterpret_cast<char*>(&data), sizeof(Data));
    params.read_bytes(key, reinterpret_cast<char*>(&read_result), sizeof(Data));
    
    //Assert
    EXPECT_EQ(write_result, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(read_result.x, data.x);
    EXPECT_EQ(read_result.y, data.y);
    EXPECT_EQ(read_result.test, data.test);
}

TEST(ParamsTest, successfully_writes_and_reads_struct_list)
{
    struct Data
    {
        int x = 0;
        float y = 0.0f;
        bool test = false;
    };

    //Assemble
    std::string key = "key_struct_list";
    std::vector<Data> data_list;
    std::vector<std::string> data_bytes_write_list;
    size_t data_count = 5;

    for(size_t i = 0; i < data_count; i++)
    {
        Data data = {
            .x = 100,
            .y = 3.14,
            .test = true
        };

        std::string data_string = std::string(reinterpret_cast<char*>(&data), sizeof(Data));
        data_bytes_write_list.push_back(data_string);
        data_list.push_back(std::move(data));
    }

    emessgee::Params params;

    //Act
    emessgee::BufferWriteCode write_result = params.write_string_list(key, data_bytes_write_list);
    std::vector<std::string> read_result = params.read_string_list(key);

    
    //Assert
    EXPECT_EQ(write_result, emessgee::BufferWriteCode::SUCCESS);

    for(size_t i = 0; i < read_result.size(); i++)
    {
        std::string data_str = read_result[i];
        Data* read_data = reinterpret_cast<Data*>(const_cast<char*>(data_str.c_str()));
        EXPECT_EQ(read_data->x, data_list[i].x);
        EXPECT_EQ(read_data->y, data_list[i].y);
        EXPECT_EQ(read_data->test, data_list[i].test);
    }
}