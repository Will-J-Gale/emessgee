#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

examples="OFF"

if [ "$1" = "examples" ]; then
    examples="ON"
fi

cd build
cmake .. -DEXAMPLES=$examples -Wno-dev
make
cd ..