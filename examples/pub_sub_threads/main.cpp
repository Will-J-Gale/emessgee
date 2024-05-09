#include <iostream>
#include <chrono>
#include <thread>

#include <emessgee.h>

using namespace std::chrono_literals;

constexpr char LETTERS[] = "abcdefghijklmnopqrstuvwxyz";
constexpr char TERMINATE[] = "terminate";

std::string random_string(uint length)
{
    std::string str;
    for(uint i = 0; i < length; i++)
    {
        str += LETTERS[rand() % 26];
    }

    return str;
}

void publish_thread(std::string topic, uint num_loops=10)
{
    emessgee::Publisher publisher(topic);

    for(int i = 0; i < num_loops; i++)
    {
        std::string data = random_string(50);
        publisher.send(topic, (char*)data.c_str(), data.size());

        std::cout << "Sent: " << data << std::endl;
        std::this_thread::sleep_for(1000ms);
    }

    std::string terminate_message = TERMINATE;
    publisher.send(topic, (char*)terminate_message.c_str(), terminate_message.size());
    std::cout << "Publisher finished" << std::endl;
}

void subscribe_thread(std::string topic)
{
    emessgee::Subscriber subscriber(topic);

    while(true)
    {
        emessgee::ReadResult result = subscriber.recv(topic);
        if(result.valid)
        {
            std::string result_message((char*)result.data, result.size);

            if(result_message == TERMINATE)
            {
                break;
            }

            std::cout << "Received: " << result_message << std::endl;
        }
    }

    std::cout << "Subscriber finished" << std::endl;
}

int main()
{
    auto current_time = std::chrono::system_clock::now().time_since_epoch();
    double duration = std::chrono::duration<double>(current_time).count() * 1000.0f;
    std::srand(duration);

    std::string topic = "pub_sub_topic";

    std::thread p_thread = std::thread(publish_thread, topic, 10);
    std::thread s_thread = std::thread(subscribe_thread, topic);

    p_thread.join();
    s_thread.join();

    return 0;
}
