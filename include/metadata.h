#pragma once

#include <typedefs.h>

struct Metadata
{
    uint queue_size = 0;
    bool block_ready = false;
    bool writing = false;

    byte* to_bytes()
    {
        return reinterpret_cast<byte*>(this);
    }
};

constexpr int METADATA_SIZE = sizeof(Metadata);