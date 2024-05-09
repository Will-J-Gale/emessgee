#pragma once

#include <memory>
#include <deque>
#include <string>

#include <typedefs.h>
#include <message_header.h>
#include <metadata.h>
#include <read_memory_block.h>
#include <read_result.h>

namespace emessgee
{

class ReadMemoryQueue
{
public:
    using Ptr = std::unique_ptr<ReadMemoryQueue>;

    ReadMemoryQueue(std::string name);
    ~ReadMemoryQueue();

    ReadResult read();
    void close();

private:
    void try_initialize();
    MessageHeader* read_header(uint queue_index);
    void read_metadata();

private:
    uint _queue_index = 0;
    Metadata* _metadata = nullptr;
    std::deque<uint> _read_message_ids;
    ReadMemoryBlock::Ptr _read_block;
    bool initialized = false;
};

}