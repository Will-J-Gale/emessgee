#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include <constants.h>
#include <typedefs.h>
#include <write_memory_queue.h>

class Publisher
{
public:
    Publisher(std::string topic, uint buffer_size=DEFAULT_BUFFER_SIZE, uint queue_size=DEFAULT_QUEUE_SIZE);
    Publisher(std::vector<std::string> topics, uint buffer_size=DEFAULT_BUFFER_SIZE, uint queue_size=DEFAULT_QUEUE_SIZE);
    ~Publisher();
    void send(std::string topic, byte* data, uint size);
    void close();

private:
    std::map<std::string, std::unique_ptr<WriteMemoryQueue>> _topic_queues;
};