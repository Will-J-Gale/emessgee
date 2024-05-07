#include <algorithm>

#include <read_memory_queue.h>

ReadMemoryQueue::ReadMemoryQueue(std::string name)
{
    _read_block = std::make_unique<ReadMemoryBlock>(name);
    _metadata = *read_metadata();
    _read_message_ids = std::deque<uint>(_metadata.queue_size);
}

ReadMemoryQueue::~ReadMemoryQueue()
{
    close();
}

ReadResult ReadMemoryQueue::read()
{
    ReadResult read_result;

    if(is_writing())
    {
        std::cout << "writing" << std::endl;
        return read_result;
    }

    MessageHeader* header = read_header(_queue_index);

    bool id_exists = std::find(_read_message_ids.begin(), _read_message_ids.end(), header->message_id) != _read_message_ids.end();

    if(header->message_id == INVALID_ID || id_exists)
    {
        return read_result;
    }

    read_result.data = _read_block->read(header->message_index);
    read_result.size = header->message_size;
    read_result.valid = true;

    _read_message_ids.push_back(header->message_id);
    _queue_index = (_queue_index + 1) % _metadata.queue_size;

    return read_result;
}
void ReadMemoryQueue::close()
{
    _read_block->destroy();
}

bool ReadMemoryQueue::is_writing()
{
    Metadata* metadata = read_metadata();
    return metadata->writing;
}

MessageHeader* ReadMemoryQueue::read_header(uint queue_index)
{
    uint header_index = METADATA_SIZE + (queue_index * MESSAGE_HEADER_SIZE);
    return MessageHeader::from_bytes(_read_block->read(header_index));
}

Metadata* ReadMemoryQueue::read_metadata()
{
    return Metadata::from_bytes(_read_block->read(0));
}