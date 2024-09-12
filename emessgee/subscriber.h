#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include <constants.h>
#include <typedefs.h>
#include <read_memory_queue.h>

namespace emessgee
{

class Subscriber
{
public:
    Subscriber(std::string topic);
    Subscriber(std::vector<std::string> topics);
    ~Subscriber();
    ReadResult recv(std::string topic);
    void close();

private:
    std::map<std::string, ReadMemoryQueue::Ptr> _topic_queues;
};

}