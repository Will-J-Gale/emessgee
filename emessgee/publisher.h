#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include <typedefs.h>
#include <constants.h>
#include <buffer_write_code.h>
#include <write_memory_queue.h>

namespace emessgee
{

class Publisher
{
public:
    Publisher(std::string topic, uint buffer_size=DEFAULT_BUFFER_SIZE, uint queue_size=DEFAULT_QUEUE_SIZE);
    Publisher(std::vector<std::string> topics, uint buffer_size=DEFAULT_BUFFER_SIZE, uint queue_size=DEFAULT_QUEUE_SIZE);
    ~Publisher();
    BufferWriteCode send(std::string topic, byte* data, uint size);
    void close();

private:
    std::map<std::string, std::unique_ptr<WriteMemoryQueue>> _topic_queues;
};

}