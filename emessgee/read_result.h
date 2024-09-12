#pragma once

#include <typedefs.h>

namespace emessgee
{

struct ReadResult
{
    byte* data = nullptr;
    uint size = 0;
    bool valid = false;
};

}