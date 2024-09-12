#include <iostream>

#include <emessgee.h>

int main()
{
    std::string topic = "pub_sub_topic";
    size_t buffer_size = 1000;
    size_t queue_size = 2;
    emessgee::Publisher publisher(topic, buffer_size, queue_size);
    emessgee::Subscriber subscriber(topic);

    std::string data = "hello there";
    std::cout << "Published: " << data << std::endl;
    publisher.send(topic, (emessgee::byte*)data.c_str(), data.size());

    emessgee::ReadResult result = subscriber.recv(topic);

    if(result.valid)
    {
        std::string result_message((char*)result.data, result.size);
        std::cout << "Received: " << result_message << std::endl;
    }

    return 0;
}
