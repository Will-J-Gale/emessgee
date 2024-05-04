#pragma once

#include <typedefs.h>
#include <constants.h>

struct MessageHeader
{
    uint message_index = INVALID_INDEX;
    uint message_size = INVALID_SIZE;
    uint message_id = INVALID_ID;

    byte* to_bytes()
    {
        return reinterpret_cast<byte*>(this);
    }
};

constexpr int MESSAGE_HEADER_SIZE = sizeof(MessageHeader);