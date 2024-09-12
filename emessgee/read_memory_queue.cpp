#include <algorithm>

#include <read_memory_queue.h>

namespace emessgee
{

ReadMemoryQueue::ReadMemoryQueue(std::string name)
{
    _read_block = std::make_unique<ReadMemoryBlock>(name);

    if(_read_block->is_initialized())
    {
        read_metadata();
    }
}

ReadMemoryQueue::~ReadMemoryQueue()
{
    close();
}

bool ReadMemoryQueue::is_initialized()
{
    return _read_block->is_initialized();
}

ReadResult ReadMemoryQueue::read()
{
    ReadResult read_result;

    if(!initialized)
    {
        if(!try_initialize())
        {
            return read_result;
        }
    }

    if(_metadata->writing)
    {
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
    _queue_index = (_queue_index + 1) % _metadata->queue_size;

    return read_result;
}
void ReadMemoryQueue::close()
{
    _read_block->destroy();
}

MessageHeader* ReadMemoryQueue::read_header(uint queue_index)
{
    uint header_index = METADATA_SIZE + (queue_index * MESSAGE_HEADER_SIZE);
    return MessageHeader::from_bytes(_read_block->read(header_index));
}

void ReadMemoryQueue::read_metadata()
{
    byte* data = _read_block->read(0);
    _metadata = reinterpret_cast<Metadata*>(data);
}

bool ReadMemoryQueue::try_initialize()
{
    if(!_read_block->is_initialized())
    {
        if(_read_block->initialize())
        {
            read_metadata();
        }
        else
        {
            return false;
        }
    }

    if(!_metadata->block_ready)
    {
        return false;
    }

    _read_message_ids = std::deque<uint>(_metadata->queue_size);   
    initialized = true;
    return true;
}

}