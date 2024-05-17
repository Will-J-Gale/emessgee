#include <iostream>
#include <publisher.h>
#include <subscriber.h>
#include <stdexcept> 

/*
    @TODO
    1. Create c++ examples
    2. Create unit tests
    3. Cython-ize
*/

int main()
{
    std::string topic = "test";
    emessgee::Subscriber subscriber(topic);
    emessgee::Publisher publisher(topic);

    std::string data = "hello there";
    publisher.send(topic, (byte*)data.c_str(), data.size());

    emessgee::ReadResult result = subscriber.recv(topic);

    if(result.valid)
    {
        std::string result_message((char*)result.data, result.size);
        std::cout << result_message << std::endl;
    }

    return 0;
}