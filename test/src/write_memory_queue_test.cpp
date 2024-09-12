#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <filesystem>

#include <emessgee.h>
#include <test_data.h>

TEST(WriteMemoryQueueTest, constructor_successfullyCreatesFile)
{
    //Assemble
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    uint queue_size = 3;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    uint expected_size = emessgee::METADATA_SIZE + (emessgee::MESSAGE_HEADER_SIZE * queue_size) + buffer_size;

    //Act
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);

    //Assert
    EXPECT_TRUE(std::filesystem::exists(tmp_file));
    EXPECT_EQ(std::filesystem::file_size(tmp_file), expected_size);

    emessgee::WriteMemoryBlock* write_block = write_queue.get_write_block();
    emessgee::Metadata* metadata = emessgee::Metadata::from_bytes(write_block->read(0));
    EXPECT_EQ(metadata->queue_size, queue_size);
}

TEST(WriteMemoryQueueTest, close_successfullyCleansUpClass)
{
    //Assemble
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    uint queue_size = 3;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);

    //Act
    write_queue.close();

    //Assert
    EXPECT_FALSE(std::filesystem::exists(tmp_file));
    EXPECT_EQ(write_queue.get_write_block(), nullptr);
}

TEST(WriteMemoryQueueTest, close_calledTwice_successfullyCleansUpClassOnFirstRun_secondTimeIgnores)
{
    //Assemble
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    uint queue_size = 3;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);

    //Act
    write_queue.close();
    write_queue.close();

    //Assert
    EXPECT_FALSE(std::filesystem::exists(tmp_file));
}


TEST(WriteMemoryQueueTest, write_successfullyWritesData)
{
    //Assemble
    TestData data = {
        .int_value = 47362,
        .char_value='q',
        .flag=true
    };
    std::string topic = "write_block_1";
    uint buffer_size = 2345;
    uint queue_size = 3;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);
    emessgee::WriteMemoryBlock* write_block = write_queue.get_write_block();

    //Act
    emessgee::BufferWriteCode result = write_queue.write(reinterpret_cast<unsigned char*>(&data), sizeof(data));

    //Assert
    EXPECT_EQ(result, emessgee::BufferWriteCode::SUCCESS);
}

TEST(WriteMemoryQueueTest, write_writeTwice_headersSuccessfullyUpdated)
{
    //Assemble
    TestData data = {
        .int_value = 47362,
        .char_value='q',
        .flag=true
    };

    unsigned char data_2 = 'p';

    std::string topic = "write_block_1";
    uint buffer_size = 1000;
    uint queue_size = 3;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);
    emessgee::WriteMemoryBlock* write_block = write_queue.get_write_block();

    //Act
    emessgee::BufferWriteCode result_1 = write_queue.write(reinterpret_cast<unsigned char*>(&data), sizeof(data));
    emessgee::BufferWriteCode result_2 = write_queue.write(&data_2, sizeof(data_2));

    //Assert
    EXPECT_EQ(result_1, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(result_2, emessgee::BufferWriteCode::SUCCESS);

    unsigned char* metadata_ptr = write_block->read(emessgee::METADATA_SIZE);
    emessgee::MessageHeader* message_header = emessgee::MessageHeader::from_bytes(metadata_ptr);
    EXPECT_EQ(message_header->message_size, sizeof(data));
    EXPECT_NE(message_header->message_id, emessgee::INVALID_ID);
    EXPECT_NE(message_header->message_index, emessgee::INVALID_INDEX);
    TestData* result_data = reinterpret_cast<TestData*>(write_block->read(message_header->message_index));
    EXPECT_EQ(result_data->int_value, data.int_value);
    EXPECT_EQ(result_data->char_value, data.char_value);
    EXPECT_EQ(result_data->flag, data.flag);

    emessgee::MessageHeader* message_header_2 = emessgee::MessageHeader::from_bytes(metadata_ptr + emessgee::MESSAGE_HEADER_SIZE);
    EXPECT_EQ(message_header_2->message_size, sizeof(data_2));
    EXPECT_NE(message_header_2->message_id, emessgee::INVALID_ID);
    EXPECT_NE(message_header_2->message_index, emessgee::INVALID_INDEX);
    unsigned char* result_data_2 = write_block->read(message_header_2->message_index);
    EXPECT_EQ(*result_data_2, data_2);

    emessgee::MessageHeader* message_header_3 = emessgee::MessageHeader::from_bytes(metadata_ptr + (emessgee::MESSAGE_HEADER_SIZE * 2));
    EXPECT_EQ(message_header_3->message_size, emessgee::INVALID_SIZE);
    EXPECT_EQ(message_header_3->message_id, emessgee::INVALID_ID);
    EXPECT_EQ(message_header_3->message_index, emessgee::INVALID_INDEX);
}

TEST(WriteMemoryQueueTest, write_queueCorrectlyWrapsAround_data3ShouldBeInQueue1Position)
{
    //Assemble
    TestData data_1 = {
        .int_value = 47362,
        .char_value='q',
        .flag=true
    };

    TestData data_2 = {
        .int_value = 47362,
        .char_value='q',
        .flag=true
    };

    TestData data_3 = {
        .int_value = 47362,
        .char_value='q',
        .flag=true
    };

    std::string topic = "write_block_1";
    uint buffer_size = (sizeof(TestData) * 3) - 1;
    uint queue_size = 2;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);
    emessgee::WriteMemoryBlock* write_block = write_queue.get_write_block();

    //Act
    emessgee::BufferWriteCode result_1 = write_queue.write(reinterpret_cast<unsigned char*>(&data_1), sizeof(TestData));
    emessgee::BufferWriteCode result_2 = write_queue.write(reinterpret_cast<unsigned char*>(&data_2), sizeof(TestData));
    emessgee::BufferWriteCode result_3 = write_queue.write(reinterpret_cast<unsigned char*>(&data_3), sizeof(TestData));

    //Assert
    EXPECT_EQ(result_1, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(result_2, emessgee::BufferWriteCode::SUCCESS);
    EXPECT_EQ(result_3, emessgee::BufferWriteCode::SUCCESS);

    unsigned char* metadata_ptr = write_block->read(emessgee::METADATA_SIZE);
    emessgee::MessageHeader* message_header = emessgee::MessageHeader::from_bytes(metadata_ptr);
    EXPECT_EQ(message_header->message_size, sizeof(TestData));
    EXPECT_NE(message_header->message_id, emessgee::INVALID_ID);
    EXPECT_NE(message_header->message_index, emessgee::INVALID_INDEX);
    TestData* result_data = reinterpret_cast<TestData*>(write_block->read(message_header->message_index));
    EXPECT_EQ(result_data->int_value, data_3.int_value);
    EXPECT_EQ(result_data->char_value, data_3.char_value);
    EXPECT_EQ(result_data->flag, data_3.flag);
}

TEST(WriteMemoryQueueTest, write_bufferHasBeenDestroyed_returnsBufferNullptrError)
{
    //Assemble
    TestData data = {
        .int_value = 47362,
        .char_value='q',
        .flag=true
    };
    std::string topic = "write_block_1";
    uint buffer_size = sizeof(data) - 2;
    uint queue_size = 3;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);
    write_queue.close();

    //Act
    emessgee::BufferWriteCode result = write_queue.write(reinterpret_cast<unsigned char*>(&data), sizeof(data));

    //Assert
    EXPECT_EQ(result, emessgee::BufferWriteCode::BUFFER_NULLPTR);
}