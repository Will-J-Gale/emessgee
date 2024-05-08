#include <publisher.h>

namespace emessgee
{

Publisher::Publisher(std::vector<std::string> topics, uint buffer_size, uint queue_size)
{
    for(std::string& topic : topics)
    {
        _topic_queues.insert({
            topic,
            std::make_unique<WriteMemoryQueue>(topic, buffer_size, queue_size)
        });
    }
}

Publisher::Publisher(std::string topic, uint buffer_size, uint queue_size)
{
    _topic_queues.insert({
        topic,
        std::make_unique<WriteMemoryQueue>(topic, buffer_size, queue_size)
    });
}

Publisher::~Publisher()
{
    close();
}
void Publisher::send(std::string topic, byte* data, uint size)
{
    if(_topic_queues.count(topic) > 0)
    {
        _topic_queues[topic]->write(data, size);
    }
}

void Publisher::close()
{
    std::map<std::string, std::unique_ptr<WriteMemoryQueue>>::iterator it;

    for(it = _topic_queues.begin(); it != _topic_queues.end(); ++it)
    {
        it->second->close();
    }
}

}