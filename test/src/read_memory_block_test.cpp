#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <filesystem>

#include <emessgee.h>
#include <test_data.h>

TEST(ReadMemoryBlockTest, constructor_fileDoesNotExist_readBlockNotInitialized)
{
    //Assemble
    std::string topic = "read_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;

    //Act
    emessgee::ReadMemoryBlock read_block(topic);

    //Assert
    EXPECT_FALSE(read_block.is_initialized());
}

TEST(ReadMemoryBlockTest, constructor_fileExists_readBlockIsInitialized)
{
    //Assemble
    std::string topic = "read_block_2";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, 1000);

    //Act
    emessgee::ReadMemoryBlock read_block(topic);

    //Assert
    EXPECT_TRUE(read_block.is_initialized());
}

TEST(ReadMemoryBlockTest, initialize_fileExists_returnsTrue)
{
    //Assemble
    std::string topic = "read_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, 1000);
    emessgee::ReadMemoryBlock read_block(topic);

    //Act
    bool result = read_block.initialize();

    //Assert
    EXPECT_TRUE(result);
}

TEST(ReadMemoryBlockTest, initialize_fileDoesNotExists_returnsFalse)
{
    //Assemble
    std::string topic = "read_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::ReadMemoryBlock read_block(topic);

    //Act
    bool result = read_block.initialize();

    //Assert
    EXPECT_FALSE(result);
}

TEST(ReadMemoryBlockTest, read_blockNotInitialized_readReturnsNullPtr)
{
    //Assemble
    std::string topic = "read_block_3";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::ReadMemoryBlock read_block(topic);

    //Act
    char* result = read_block.read(10);

    //Assert
    EXPECT_EQ(result, nullptr);
}

TEST(ReadMemoryBlockTest, read_blockIsInitialized_readReturnsValue)
{
    //Assemble
    char data = 32;
    uint index = 10;
    std::string topic = "read_block_4";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, 1000);
    emessgee::ReadMemoryBlock read_block(topic);

    write_block.write(index, &data, 1);

    //Act
    char* result = read_block.read(index);

    //Assert
    EXPECT_EQ(*result, data);
}

TEST(ReadMemoryBlockTest, read_structIsWritten_returnsStruct)
{
    //Assemble
    TestData data  = {
        .int_value=88878434,
        .char_value = 'c',
        .flag=true,
    };

    uint index = 10;
    std::string topic = "read_block_4";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, 1000);
    emessgee::ReadMemoryBlock read_block(topic);

    write_block.write(index, reinterpret_cast<char*>(&data), sizeof(data));

    //Act
    char* result_ptr = read_block.read(index);
    TestData* result = reinterpret_cast<TestData*>(result_ptr);

    //Assert
    EXPECT_EQ(result->char_value, data.char_value);
    EXPECT_EQ(result->flag, data.flag);
    EXPECT_EQ(result->int_value, data.int_value);
}

TEST(ReadMemoryBlockTest, read_tryReadIndexGreaterThanSize_returnsNullptr)
{
    //Assemble
    char data = 32;
    uint block_size = 1000;
    std::string topic = "read_block_5";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, block_size);
    emessgee::ReadMemoryBlock read_block(topic);

    //Act
    char* result = read_block.read(block_size + 10);

    //Assert
    EXPECT_EQ(result, nullptr);
}

TEST(ReadMemoryBlockTest, read_blockCreatedAfterConstructor_initializeCalledWhenRead)
{
    //Assemble
    char data = 32;
    uint index = 10;
    std::string topic = "read_block_6";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::ReadMemoryBlock read_block(topic);
    emessgee::WriteMemoryBlock write_block(topic, 1000);
    
    bool before_is_initialized = read_block.is_initialized();

    write_block.write(index, &data, 1);

    //Act
    char* result = read_block.read(index);

    //Assert
    EXPECT_EQ(*result, data);
    EXPECT_FALSE(before_is_initialized);
    EXPECT_TRUE(read_block.is_initialized());
}


TEST(ReadMemoryBlockTest, destroy_successfullyDereferencesBuffer)
{
    //Assemble
    uint block_size = 1000;
    std::string topic = "read_block_7";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, block_size);
    emessgee::ReadMemoryBlock read_block(topic);
    bool before_is_initialized = read_block.is_initialized();

    //Act
    read_block.destroy();

    //Assert
    EXPECT_TRUE(before_is_initialized);
    EXPECT_FALSE(read_block.is_initialized());
}

TEST(ReadMemoryBlockTest, destroy_calledTwice_nothingHappensSecondTime)
{
    //Assemble
    uint block_size = 1000;
    std::string topic = "read_block_8";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, block_size);
    emessgee::ReadMemoryBlock read_block(topic);
    bool before_is_initialized = read_block.is_initialized();

    //Act
    read_block.destroy();
    read_block.destroy();

    //Assert
    EXPECT_TRUE(before_is_initialized);
    EXPECT_FALSE(read_block.is_initialized());
}



