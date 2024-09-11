#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

#include <emessgee.h>

int main(int argc, char *argv[])
{
    std::filesystem::path image_path = "resources/image.jpg";

    if(argc == 2)
    {
        image_path = argv[1];
    }

    if(!std::filesystem::exists(image_path))
    {
        std::cout << image_path << " does not exist" << std::endl;
        return 1;
    }

    std::string topic = "image_topic";
    emessgee::Subscriber subscriber(topic);
    emessgee::Publisher publisher(topic, 1e8);

    std::ifstream file(image_path.string(), std::ios::binary | std::ios::ate);
    std::streamsize image_size = file.tellg();
    file.seekg(0, std::ios::beg); 

    std::vector<char> buffer(image_size);
    
    if(!file.read(buffer.data(), image_size))
    {
        std::cout << "Failed to read image" << std::endl;
    }

    publisher.send(topic, (unsigned char*)buffer.data(), buffer.size());

    emessgee::ReadResult result = subscriber.recv(topic);

    if(result.valid)
    {
        std::string result_message((char*)result.data, result.size);
        std::cout << "Received size: " << result.size << std::endl;
    }

    return 0;
}
