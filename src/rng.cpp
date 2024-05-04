#include <rng.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

uint RNG::generate()
{
    if(!_initialized)
    {
        _initialized = true;
        auto current_time = std::chrono::system_clock::now().time_since_epoch();
        double duration = std::chrono::duration<double>(current_time).count() * 1000.0f;
        std::srand(duration);
    }

    return std::rand();
}

bool RNG::_initialized = false;