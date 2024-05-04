#pragma once

#include <string>
#include <memory>

#include <write_memory_block.h>
#include <constants.h>
#include <metadata.h>
#include <message_header.h>


class WriteMemoryQueue
{
public:
    WriteMemoryQueue(std::string name, uint buffer_size=DEFAULT_BUFFER_SIZE, uint queue_size=DEFAULT_QUEUE_SIZE);
    ~WriteMemoryQueue();
    BufferWriteCode write(byte* data, uint size);
    void close();

private:
    void begin_write();
    void end_write();
    void write_metadata(Metadata metadata);
    void write_header(uint queue_index, MessageHeader header);

private:
    uint _buffer_size = 0;
    uint _allocated_buffer_size = 0;
    uint _queue_size = 0;
    uint _write_index = 0;
    uint _queue_index = 0;
    uint _data_start = 0;
    uint _header_length = 0;
    std::unique_ptr<WriteMemoryBlock> _write_block;
    Metadata _metadata;
};