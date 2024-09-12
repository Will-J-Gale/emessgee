#include <iostream>

#include <emessgee.h>

int main(int argc, char const *argv[])
{
    std::string topic_1 = "topic_1";
    std::string topic_2 = "topic_2";
    std::string data_1 = "some bytes";
    std::string data_2 = "some more bytes";

    emessgee::Publisher publisher({topic_1, topic_2});
    emessgee::Subscriber subscriber1(topic_1);
    emessgee::Subscriber subscriber2(topic_2);

    publisher.send(topic_1, (emessgee::byte*)data_1.c_str(), data_1.size());
    std::cout << "Published to topic '" << topic_1 << "': " << data_1 << std::endl;

    publisher.send(topic_2, (emessgee::byte*)data_2.c_str(), data_2.size());
    std::cout << "Published to topic '" << topic_2 << "': " << data_2 << std::endl;

    emessgee::ReadResult result_1 = subscriber1.recv(topic_1);
    emessgee::ReadResult result_2 = subscriber2.recv(topic_2);

    if(result_1.valid)
    {
        std::string result_message((char*)result_1.data, result_1.size);
        std::cout << "Received on topic '" << topic_1 << "': " << result_message << std::endl;
    }

    if(result_2.valid)
    {
        std::string result_message((char*)result_2.data, result_2.size);
        std::cout << "Received on topic '" << topic_2 << "': " << result_message << std::endl;
    }

    return 0;
}
