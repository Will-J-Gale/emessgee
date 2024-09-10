#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <filesystem>
#include <vector>

#include <emessgee.h>
#include <test_data.h>

TEST(ReadMemoryQueueTest, constructor_readMemoryQueue_tempFileDoesNotExist_isNotInitialized)
{
    //Assemble/Act
    emessgee::ReadMemoryQueue read_queue("read_block_test");

    //Assert
    EXPECT_FALSE(read_queue.is_initialized());
}

TEST(ReadMemoryQueueTest, constructor_readMemoryQueue_tempFileExists_isInitialized)
{
    //Assemble
    std::string topic = "read_block_test";
    uint buffer_size = 2345;
    uint queue_size = 3;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);

    //Act
    emessgee::ReadMemoryQueue read_queue(topic);

    //Assert
    EXPECT_TRUE(read_queue.is_initialized());
}

TEST(ReadMemoryQueueTest, read_isNotInitialized_resultIsNullptr)
{
    //Assemble
    emessgee::ReadMemoryQueue read_queue("read_block_test");

    //Act
    emessgee::ReadResult result = read_queue.read();

    //Assert
    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.data, nullptr);
}

TEST(ReadMemoryQueueTest, read_isInitlaized_noDataInQueue_resultIsNullptr)
{
    //Assemble
    std::string topic = "read_block_test";
    uint buffer_size = 2345;
    uint queue_size = 3;

    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);
    emessgee::ReadMemoryQueue read_queue(topic);

    //Act
    emessgee::ReadResult result = read_queue.read();

    //Assert
    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.data, nullptr);
}

TEST(ReadMemoryQueueTest, read_dataIsWrittenToQueue_dataReturnedInReadFunction)
{
    //Assemble
    std::string topic = "read_block_test";
    uint buffer_size = 2345;
    uint queue_size = 3;
    std::string data = "some byte data";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);
    write_queue.write((char*)data.c_str(), sizeof(data));

    emessgee::ReadMemoryQueue read_queue(topic);

    //Act
    emessgee::ReadResult result = read_queue.read();

    //Assert
    EXPECT_TRUE(result.valid);
    EXPECT_EQ(result.size, sizeof(data));
    EXPECT_EQ(result.data, data);
}

TEST(ReadMemoryQueueTest, read_dataIsWrittenToQueue_dataAlreadyRead_nextReadIsNotValid)
{
    //Assemble
    std::string topic = "read_block_test";
    uint buffer_size = 2345;
    uint queue_size = 3;
    std::string data = "some byte data";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);
    write_queue.write((char*)data.c_str(), sizeof(data));

    emessgee::ReadMemoryQueue read_queue(topic);

    //Act
    emessgee::ReadResult result_1 = read_queue.read();
    emessgee::ReadResult result_2 = read_queue.read();

    //Assert
    EXPECT_TRUE(result_1.valid);
    EXPECT_EQ(result_1.size, sizeof(data));
    EXPECT_EQ(result_1.data, data);

    EXPECT_FALSE(result_2.valid);
    EXPECT_EQ(result_2.size, 0);
}

TEST(ReadMemoryQueueTest, read_multipleMessagesInQueue_correctlyReadsAllMessages)
{
    //Assemble
    std::string topic = "read_block_test";
    uint buffer_size = 2345;
    uint queue_size = 10;
    std::string data = "some byte data";
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::WriteMemoryQueue write_queue(topic, buffer_size, queue_size);

    uint num_messages = 7;
    std::vector<std::string> messages;

    for(uint i = 0; i < num_messages; i++)
    {
        std::string message = std::to_string(emessgee::RNG::generate());
        write_queue.write((char*)message.c_str(), message.size());
        messages.push_back(message);
    }

    emessgee::ReadMemoryQueue read_queue(topic);

    //Act
    for(uint i = 0; i < num_messages; i++)
    {
        emessgee::ReadResult result = read_queue.read();
        std::string result_message = std::string(result.data, result.size);
        std::string expected_message = messages[i];

        EXPECT_TRUE(result.valid);
        EXPECT_EQ(result.size, expected_message.size());
        EXPECT_EQ(result_message, expected_message);
    }

}