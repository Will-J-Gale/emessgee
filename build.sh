#!/bin/bash

build_examples="OFF"
build_python=false

for arg in "$@"
do
    if [[ "$arg" == "--build_examples" ]]; then
        build_examples="ON"
    fi

    if [[ "$arg" == "--build_python" ]]; then
        build_python=true
    fi
done

if [ ! -d "build" ]; then
    mkdir build
fi


cd build
cmake .. -DEXAMPLES=$build_examples -Wno-dev
make
cd ..

if [[ "$build_python" == true ]]; then
    python3 setup.py build_ext --inplace -f 
fi