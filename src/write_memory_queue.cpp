#include <write_memory_queue.h>

#include <rng.h>
#include <algorithm>

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

    _metadata.queue_size = queue_size;
    _metadata.block_ready = true;

    write_metadata(_metadata);
}

WriteMemoryQueue::~WriteMemoryQueue()
{
    close();
}

BufferWriteCode WriteMemoryQueue::write(byte* data, uint size)
{
    BufferWriteCode write_status = BufferWriteCode::SUCCESS;

    begin_write();
    if(_write_index + size >= _allocated_buffer_size)
    {
        _write_index = _data_start;
    }

    uint message_index = _write_index;

    //@TODO figure out how to handle non successful messages, 
    write_status = _write_block->write_bytes(message_index, data, size);
    _write_index += size;

    write_header(_queue_index, {
        .message_index=message_index,
        .message_size=size,
        .message_id=get_unique_id()
    });

    _queue_index = (_queue_index + 1) % _queue_size;

    end_write();

    return write_status;
}

void WriteMemoryQueue::close()
{
    _write_block->destroy();
}

void WriteMemoryQueue::begin_write()
{
    _metadata.writing = true;
    write_metadata(_metadata);
}
void WriteMemoryQueue::end_write()
{
    _metadata.writing = false;
    write_metadata(_metadata);
}

void WriteMemoryQueue::write_metadata(Metadata metadata)
{
    _write_block->write_bytes(0, _metadata.to_bytes(), METADATA_SIZE);
}

void WriteMemoryQueue::write_header(uint queue_index, MessageHeader header)
{
    uint header_index = METADATA_SIZE + (MESSAGE_HEADER_SIZE * queue_index);
    _write_block->write_bytes(header_index, header.to_bytes(), MESSAGE_HEADER_SIZE);
}

uint WriteMemoryQueue::get_unique_id()
{
    uint id = 0;
    bool valid = false;

    for(int i = 0; i < MAX_SANITY_LOOPS; i++)
    {
        id = RNG::generate();
        valid = std::find(_used_ids.begin(), _used_ids.end(), id) == _used_ids.end();

        if(valid)
        {
            break;
        }
    }

    assert(valid);

    return id;
}

}