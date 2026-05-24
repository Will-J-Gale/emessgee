#pragma once

#include "emessgee/typedefs.h"

namespace emessgee
{

class RNG
{
public:
    static uint generate();
private:
    static bool _initialized;
};

}