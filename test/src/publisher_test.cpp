#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <filesystem>
#include <vector>

#include <emessgee.h>
#include <test_data.h>


TEST(PubTest, constructor_successfullyCreatesTempFile)
{
    //Assemble
    std::string topic = "pub_test";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;

    //Act
    emessgee::Publisher publisher(topic);

    //Assert
    EXPECT_TRUE(std::filesystem::exists(tmp_file));
}

TEST(PubTest, constructor_multipleTopics_filesCreatedForEachTopic)
{
    //Assemble
    std::vector<std::string> topics = {"topic_1", "topic_2", "topic_3", "topic_4", "topic_5"};
    uint buffer_size = 2345;

    //Act
    emessgee::Publisher publisher(topics);

    //Assert
    for(std::string& topic : topics)
    {
        EXPECT_TRUE(std::filesystem::exists(emessgee::TMP_FOLDER + topic));
    }
}

TEST(PubTest, close_successfullyClosesFile)
{
    //Assemble
    std::string topic = "pub_test";
    uint buffer_size = 2345;
    std::filesystem::path tmp_file = emessgee::TMP_FOLDER + topic;
    emessgee::Publisher publisher(topic);

    //Act
    publisher.close();

    //Assert
    EXPECT_FALSE(std::filesystem::exists(tmp_file));
}

TEST(PubTest, close_multipleTopics_closesAllFiles)
{
    //Assemble
    std::vector<std::string> topics = {"topic_1", "topic_2", "topic_3", "topic_4", "topic_5"};
    uint buffer_size = 2345;
    emessgee::Publisher publisher(topics);


    //Act
    publisher.close();

    //Assert
    for(std::string& topic : topics)
    {
        EXPECT_FALSE(std::filesystem::exists(emessgee::TMP_FOLDER + topic));
    }
}

