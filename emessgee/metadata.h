#pragma once

#include <typedefs.h>

namespace emessgee
{

struct Metadata
{
    uint queue_size = 0;
    bool block_ready = false;
    bool writing = false;

    byte* to_bytes()
    {
        return reinterpret_cast<byte*>(this);
    }

    static Metadata* from_bytes(byte* data)
    {
        return reinterpret_cast<Metadata*>(data);
    }
};

constexpr int METADATA_SIZE = sizeof(Metadata);

}