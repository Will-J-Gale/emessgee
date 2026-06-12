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
    emessgee::BufferWriteCode write_result = params.write(key, value);
    emessgee::Param<int> read_result = params.read<int>(key);
    
    //Assert
    EXPECT_EQ(write_result, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(read_result.code, emessgee::ReadResultCode::SUCCESS);
    EXPECT_EQ(read_result.value, value);

    params.destroy();
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
    emessgee::BufferWriteCode write_result_1 = params.write(key_1, value_1); //Int
    emessgee::BufferWriteCode write_result_2 = params.write(key_2, value_2); //bool
    emessgee::BufferWriteCode write_result_3 = params.write(key_3, value_3); //float
    emessgee::BufferWriteCode write_result_4 = params.write(key_4, value_4); //string
    emessgee::BufferWriteCode write_result_5 = params.write(key_5, 9999); //rvalue

    emessgee::Param<int> read_result_1 = params.read<int>(key_1);
    emessgee::Param<bool> read_result_2 = params.read<bool>(key_2);
    emessgee::Param<float> read_result_3 = params.read<float>(key_3);
    emessgee::Param<std::string> read_result_4 = params.read_string(key_4);
    emessgee::Param<int> read_result_5 = params.read<int>(key_5);
    
    //Assert
    EXPECT_EQ(write_result_1, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_2, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_3, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_4, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(write_result_5, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(read_result_1.value, value_1);
    EXPECT_EQ(read_result_2.value, value_2);
    EXPECT_EQ(read_result_3.value, value_3);
    EXPECT_EQ(read_result_4.value, value_4);
    EXPECT_EQ(read_result_5.value, 9999);

    params.destroy();
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
    params.write(key_1, value_1);
    params.write(key_2, value_2);
    params.write(key_3, value_3);
    params.write(key_4, value_4);

    emessgee::Params params_2;

    //Act
    emessgee::Param<int> read_result_1 = params_2.read<int>(key_1);
    emessgee::Param<bool> read_result_2 = params_2.read<bool>(key_2);
    emessgee::Param<float> read_result_3 = params_2.read<float>(key_3);
    emessgee::Param<std::string> read_result_4 = params_2.read_string(key_4);
    
    //Assert
    EXPECT_EQ(read_result_1.code, emessgee::ReadResultCode::SUCCESS);
    EXPECT_EQ(read_result_2.code, emessgee::ReadResultCode::SUCCESS);
    EXPECT_EQ(read_result_3.code, emessgee::ReadResultCode::SUCCESS);
    EXPECT_EQ(read_result_4.code, emessgee::ReadResultCode::SUCCESS);
    EXPECT_EQ(read_result_1.value, value_1);
    EXPECT_EQ(read_result_2.value, value_2);
    EXPECT_EQ(read_result_3.value, value_3);
    EXPECT_EQ(read_result_4.value, value_4);

    params.destroy();
    params_2.destroy();
}

TEST(ParamsTest, multiple_params_file_only_deleted_when_last_param_is_destroyed)
{
    //Assemble
    emessgee::Params params_1;
    emessgee::Params params_2;
    emessgee::Params params_3;

    //Act
    params_1.destroy();
    bool exists_1 = std::filesystem::exists(emessgee::PARAMS_PATH);
    params_2.destroy();
    bool exists_2 = std::filesystem::exists(emessgee::PARAMS_PATH);
    params_3.destroy();
    bool exists_3 = std::filesystem::exists(emessgee::PARAMS_PATH);
    
    //Assert
    EXPECT_TRUE(exists_1);
    EXPECT_TRUE(exists_2);
    EXPECT_FALSE(exists_3);
}