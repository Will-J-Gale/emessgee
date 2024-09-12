#include <write_memory_queue.h>

#include <algorithm>
#include <error_messages.h>
#include <rng.h>

namespace emessgee
{

WriteMemoryQueue::WriteMemoryQueue(std::string name, uint buffer_size, uint queue_size) :
    _buffer_size(buffer_size),
    _queue_size(queue_size)
{
    _header_length = MESSAGE_HEADER_SIZE * queue_size;
    _data_start = METADATA_SIZE + _header_length;
    _write_index = _data_start;
    _allocated_buffer_size = _data_start + buffer_size;

    _write_block = std::make_unique<WriteMemoryBlock>(name, _allocated_buffer_size);

    for(uint i = 0; i < queue_size; i++)
    {
        write_header(i, MessageHeader());
    }

    _used_ids = std::deque<uint>(queue_size);

    _metadata =  reinterpret_cast<Metadata*>(_write_block->read(0));
    _metadata->queue_size = queue_size;
    _metadata->block_ready = true;
}

WriteMemoryQueue::~WriteMemoryQueue()
{
    close();
}

BufferWriteCode WriteMemoryQueue::write(byte* data, uint size)
{

    if(_write_block == nullptr)
    {
        return BufferWriteCode::BUFFER_NULLPTR;
    }
    
    if(size >= _buffer_size)
    {
        return BufferWriteCode::DATA_TOO_LARGE;
    }

    _metadata->writing = true;

    if(_write_index + size >= _allocated_buffer_size)
    {
        _write_index = _data_start;
    }

    uint message_index = _write_index;
    BufferWriteCode write_status = _write_block->write(message_index, data, size);

    if(write_status != BufferWriteCode::SUCCESS)
    {
        _metadata->writing = false;
        return write_status;
    }

    _write_index += size;

    write_header(_queue_index, {
        .message_index=message_index,
        .message_size=size,
        .message_id=get_unique_id()
    });


    _queue_index = (_queue_index + 1) % _queue_size;
    _metadata->writing = false;

    return write_status;
}

void WriteMemoryQueue::close()
{
    if(_write_block == nullptr)
    {
        return;
    }

    _write_block->destroy();
    _write_block.reset(nullptr);
}

void WriteMemoryQueue::write_header(uint queue_index, MessageHeader header)
{
    uint header_index = METADATA_SIZE + (MESSAGE_HEADER_SIZE * queue_index);
    _write_block->write(header_index, header.to_bytes(), MESSAGE_HEADER_SIZE);
}

uint WriteMemoryQueue::get_unique_id()
{
    uint id = 0;
    bool valid = false;

    for(size_t i = 0; i < MAX_SANITY_LOOPS; i++)
    {
        id = RNG::generate();
        valid = std::find(_used_ids.begin(), _used_ids.end(), id) == _used_ids.end();

        if(valid)
        {
            break;
        }
    }

    if(!valid)
    {
        throw std::runtime_error(FAILED_TO_GENERATE_UNIQUE_ID);
    }

    return id;
}

WriteMemoryBlock* WriteMemoryQueue::get_write_block()
{
    return _write_block.get();
}


}