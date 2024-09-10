#pragma once

namespace emessgee
{

enum BufferWriteCode
{
    SUCCESS=0,
    BUFFER_NULLPTR=1,
    INDEX_TOO_LARGE=2,
    BUFFER_CLOSED=3,
    DATA_TOO_LARGE=4
};

}