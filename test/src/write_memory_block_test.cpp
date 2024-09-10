#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <filesystem>

#include <emessgee.h>

struct TestData
{
    int int_value = 0;
    char char_value = 0;
    bool flag = false;
};

TEST(WriteMemoryBlockTest, constructor_createsTmpFile)
{
    //Assemble
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;

    //Act
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    //Assert
    EXPECT_TRUE(std::filesystem::exists(tmp_file));
    EXPECT_EQ(std::filesystem::file_size(tmp_file), buffer_size);
}

TEST(WriteMemoryBlockTest, destroy_reomvesTempFile)
{
    //Assemble
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    //Act
    write_block.destroy();

    //Assert
    EXPECT_FALSE(std::filesystem::exists(tmp_file));
}

TEST(WriteMemoryBlockTest, callDestroyTwice_nothingHappensSecondTime)
{
    //Assemble
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    //Act
    write_block.destroy();
    write_block.destroy();

    //Assert
    EXPECT_FALSE(std::filesystem::exists(tmp_file));
}

TEST(WriteMemoryBlockTest, write_successfullyWritesData)
{
    //Assemble
    TestData test_data = {
        .int_value=887834,
        .char_value=56,
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    uint index = 645;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    //Act
    emessgee::BufferWriteCode result =  write_block.write(index, reinterpret_cast<char*>(&test_data), sizeof(test_data));

    //Assert
    EXPECT_EQ(result, emessgee::BufferWriteCode::SUCCESS);

    char* written_data_ptr = write_block.read(index);
    TestData* written_data = reinterpret_cast<TestData*>(written_data_ptr);

    EXPECT_EQ(test_data.int_value, written_data->int_value);
    EXPECT_EQ(test_data.char_value, written_data->char_value);
    EXPECT_EQ(test_data.flag, written_data->flag);
}

TEST(WriteMemoryBlockTest, write_indexGreaterThanBufferSize_returnsIndexToLargeCode)
{
    //Assemble
    TestData test_data = {
        .int_value=887834,
        .char_value=56,
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    uint index = buffer_size + 8;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    //Act
    emessgee::BufferWriteCode result =  write_block.write(index, reinterpret_cast<char*>(&test_data), sizeof(test_data));

    //Assert
    EXPECT_EQ(result, emessgee::BufferWriteCode::INDEX_TOO_LARGE);
}

TEST(WriteMemoryBlockTest, write_dataTooBigForBuffer_returnsDataTooLarge)
{
    //Assemble
    TestData test_data = {
        .int_value=887834,
        .char_value=56,
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = 2;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    uint index = 0;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    //Act
    emessgee::BufferWriteCode result =  write_block.write(index, reinterpret_cast<char*>(&test_data), sizeof(test_data));

    //Assert
    EXPECT_EQ(result, emessgee::BufferWriteCode::DATA_TOO_LARGE);
}

TEST(WriteMemoryBlockTest, write_bufferNotInitialised_bufferNullptrCodeReturned)
{
    //Assemble
    TestData test_data = {
        .int_value=887834,
        .char_value=56,
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    uint index = buffer_size + 8;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);
    write_block.destroy();

    //Act
    emessgee::BufferWriteCode result =  write_block.write(index, reinterpret_cast<char*>(&test_data), sizeof(test_data));

    //Assert
    EXPECT_EQ(result, emessgee::BufferWriteCode::BUFFER_NULLPTR);
}

TEST(WriteMemoryBlockTest, read_index_greater_than_buffer_size_returns_nullptr)
{
    //Assemble
    TestData test_data = {
        .int_value=887834,
        .char_value=56,
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    uint index = buffer_size + 8;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);
    write_block.destroy();

    //Act
    char* result =  write_block.read(index);

    //Assert
    EXPECT_EQ(result, nullptr);
}

TEST(WriteMemoryBlockTest, read_buffer_is_nullptr_returns_nullptr)
{
    //Assemble
    TestData test_data = {
        .int_value=887834,
        .char_value=56,
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);
    write_block.destroy();

    //Act
    char* result =  write_block.read(0);

    //Assert
    EXPECT_EQ(result, nullptr);
}

TEST(WriteMemoryBlockTest, read_successfully_read_data)
{
    //Assemble
    TestData test_data = {
        .int_value=887834,
        .char_value=56,
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    uint index = 8;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    //Act
    char* result =  write_block.read(index);

    //Assert
    EXPECT_NE(result, nullptr);
}

TEST(WriteMemoryBlockTest, create2WriteBlockWithSameName_assertRaised)
{
    //Assemble
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryBlock write_block(topic, buffer_size);

    try
    {
        //Act
        emessgee::WriteMemoryBlock write_block2(topic, buffer_size);
    }
    catch(const std::runtime_error& e)
    {
        //Assert
        EXPECT_EQ(e.what(), std::string(emessgee::FILE_ALREADY_EXISTS));
    }
}