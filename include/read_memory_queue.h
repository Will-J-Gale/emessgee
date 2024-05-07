#pragma once

#include <memory>
#include <deque>
#include <string>

#include <typedefs.h>
#include <message_header.h>
#include <metadata.h>

#include <read_memory_block.h>

struct ReadResult
{
    byte* data = nullptr;
    uint size = 0;
    bool valid = false;
};

class ReadMemoryQueue
{
public:
    using Ptr = std::unique_ptr<ReadMemoryQueue>;

    ReadMemoryQueue(std::string name);
    ~ReadMemoryQueue();

    ReadResult read();
    void close();

private:
    bool is_writing();
    void initialize();
    MessageHeader* read_header(uint queue_index);
    Metadata* read_metadata();

private:
    uint _queue_index = 0;
    Metadata _metadata;
    std::deque<uint> _read_message_ids;
    ReadMemoryBlock::Ptr _read_block;
};