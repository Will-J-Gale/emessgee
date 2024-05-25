#include <subscriber.h>

namespace emessgee
{

Subscriber::Subscriber(std::string topic)
{
    _topic_queues.insert({
        topic,
        std::make_unique<ReadMemoryQueue>(topic)
    });
}

Subscriber::Subscriber(std::vector<std::string> topics)
{

    for(std::string& topic : topics)
    {
        _topic_queues.insert({
            topic,
            std::make_unique<ReadMemoryQueue>(topic)
        });
    }
}

Subscriber::~Subscriber()
{
    close();
}

ReadResult Subscriber::recv(std::string topic)
{
    if(_topic_queues.count(topic) > 0)
    {
        return _topic_queues[topic]->read();
    }

    return ReadResult();
}

void Subscriber::close()
{
    std::map<std::string, ReadMemoryQueue::Ptr>::iterator it;

    for(it = _topic_queues.begin(); it != _topic_queues.end(); ++it)
    {
        it->second->close();
    }
}

}