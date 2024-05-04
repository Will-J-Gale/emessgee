#pragma once

#include <typedefs.h>

class RNG
{
public:
    static uint generate();
private:
    static bool _initialized;
};