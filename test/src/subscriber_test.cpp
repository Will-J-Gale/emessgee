#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>

#include <emessgee.h>
#include <test_data.h>

inline void publish_data(std::string topic, bool& stop_thread)
{
    emessgee::Publisher publisher(topic);

    for(uint i = 0; i < 1000; i++)
    {
        if(stop_thread)
        {
            break;
        }

        std::string data = std::to_string(emessgee::RNG::generate());
        publisher.send(topic, (unsigned char*)data.c_str(), sizeof(data));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
};

TEST(SubTest, constructor_successful)
{
    //Assemble
    std::string topic = "sub_test";

    //Act
    emessgee::Subscriber subscriber(topic);

    //Assert
    EXPECT_TRUE(true); //Subscriber was created no issues.
}

TEST(SubTest, constructor_multipleTopics_successful)
{
    //Assemble
    std::vector<std::string> topics = {"sub_test", "sub_test_2", "sub_test_3"};

    //Act
    emessgee::Subscriber subscriber(topics);

    //Assert
    EXPECT_TRUE(true); //Subscriber was created no issues.
}

TEST(SubTest, recv_noPublisherForTopic_readResultNotValid)
{
    //Assemble
    std::string topic = "sub_test";
    emessgee::Subscriber subscriber(topic);

    //Act
    emessgee::ReadResult result = subscriber.recv(topic);

    //Assert
    EXPECT_FALSE(result.valid); 
    EXPECT_EQ(result.size, 0); 
    EXPECT_EQ(result.data, nullptr); 
}

TEST(SubTest, recv_dataPublished_subscriberReturnsData)
{
    //Assemble
    std::string data = "some data";
    std::string topic = "sub_test";
    emessgee::Subscriber subscriber(topic);
    emessgee::Publisher publisher(topic);
    publisher.send(topic, (unsigned char*)data.c_str(), sizeof(data));

    //Act
    emessgee::ReadResult result = subscriber.recv(topic);

    //Assert
    EXPECT_TRUE(result.valid); 
    EXPECT_EQ(result.size, sizeof(data)); 
    EXPECT_EQ(std::string((char*)result.data), data); 
}

TEST(SubTest, recv_dataPublished_subscriberReturnsData_secondReadReturnsIsInvalid)
{
    //Assemble
    std::string data = "some data";
    std::string topic = "sub_test";
    emessgee::Subscriber subscriber(topic);
    emessgee::Publisher publisher(topic);
    publisher.send(topic, (unsigned char*)data.c_str(), sizeof(data));

    //Act
    emessgee::ReadResult result_1 = subscriber.recv(topic);
    emessgee::ReadResult result_2 = subscriber.recv(topic);

    //Assert
    EXPECT_TRUE(result_1.valid); 
    EXPECT_EQ(result_1.size, sizeof(data)); 
    EXPECT_EQ(std::string((char*)result_1.data), data); 

    EXPECT_FALSE(result_2.valid); 
}

TEST(SubTest, recv_dataPublishedOnDifferentTopic_subscriberDoesNotReceiveData)
{
    //Assemble
    std::string data = "some data";
    std::string topic = "sub_test";
    emessgee::Subscriber subscriber(topic);
    emessgee::Publisher publisher("another_topic");
    publisher.send(topic, (unsigned char*)data.c_str(), sizeof(data));

    //Act
    emessgee::ReadResult result = subscriber.recv(topic);

    //Assert
    EXPECT_FALSE(result.valid); 
    EXPECT_EQ(result.size, 0); 
    EXPECT_EQ(result.data, nullptr); 
}


TEST(SubTest, recv_multipleTopics_receivedDataFromBoth)
{
    //Assemble
    std::string data_1 = "some data";
    std::string data_2 = "some other data";
    std::string topic_1 = "topic_1";
    std::string topic_2 = "topic_2";
    emessgee::Publisher publisher_1(topic_1);
    emessgee::Publisher publisher_2(topic_2);
    publisher_1.send(topic_1, (unsigned char*)data_1.c_str(), sizeof(data_1));
    publisher_2.send(topic_2, (unsigned char*)data_2.c_str(), sizeof(data_2));

    emessgee::Subscriber subscriber({topic_1, topic_2});

    //Act
    emessgee::ReadResult result_1 = subscriber.recv(topic_1);
    emessgee::ReadResult result_2 = subscriber.recv(topic_2);

    //Assert
    EXPECT_TRUE(result_1.valid); 
    EXPECT_EQ(result_1.size, sizeof(data_1)); 
    EXPECT_EQ(std::string((char*)result_1.data), data_1); 

    EXPECT_TRUE(result_2.valid); 
    EXPECT_EQ(result_2.size, sizeof(data_2)); 
    EXPECT_EQ(std::string((char*)result_2.data), data_2); 
}

TEST(SubTest, multipleSubscribersForSameTopic_bothSuccessfullyReceiveData)
{
    //Assemble
    std::string data = "some data";
    std::string topic = "sub_test";
    emessgee::Publisher publisher(topic);
    publisher.send(topic, (unsigned char*)data.c_str(), sizeof(data));

    emessgee::Subscriber subscriber_1(topic);
    emessgee::Subscriber subscriber_2(topic);

    //Act
    emessgee::ReadResult result_1 = subscriber_1.recv(topic);
    emessgee::ReadResult result_2 = subscriber_2.recv(topic);

    //Assert
    EXPECT_TRUE(result_1.valid); 
    EXPECT_EQ(result_1.size, sizeof(data)); 
    EXPECT_EQ(std::string((char*)result_1.data), data); 

    EXPECT_TRUE(result_2.valid); 
    EXPECT_EQ(result_2.size, sizeof(data)); 
    EXPECT_EQ(std::string((char*)result_2.data), data); 
}

TEST(SubTest, recv_dataPublishedOnSeparateThread_subscriberReturnsData)
{
    //Assemble
    std::string topic = "sub_test";
    emessgee::Subscriber subscriber(topic);
    bool stop_thread = false;

    std::thread pub_thread = std::thread(publish_data, topic, std::ref(stop_thread));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    //Act
    emessgee::ReadResult result = subscriber.recv(topic);

    //Assert
    EXPECT_TRUE(result.valid); 
    EXPECT_GT(result.size, 0); 

    stop_thread = true;
    pub_thread.join();
}


TEST(SubTest, recv_dataFromMultipleTopicsSentFromDifferentThreads_successfullyReceivesBothTopics)
{
    //Assemble
    std::string topic_1 = "sub_test_1";
    std::string topic_2 = "sub_test_2";
    emessgee::Subscriber subscriber({topic_1, topic_2});
    bool stop_thread = false;

    std::thread pub_thread_1 = std::thread(publish_data, topic_1, std::ref(stop_thread));
    std::thread pub_thread_2 = std::thread(publish_data, topic_2, std::ref(stop_thread));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    //Act
    emessgee::ReadResult result_1 = subscriber.recv(topic_1);
    emessgee::ReadResult result_2 = subscriber.recv(topic_2);

    //Assert
    EXPECT_TRUE(result_1.valid); 
    EXPECT_GT(result_1.size, 0); 

    EXPECT_TRUE(result_2.valid); 
    EXPECT_GT(result_2.size, 0); 

    stop_thread = true;
    pub_thread_1.join();
    pub_thread_2.join();
}
