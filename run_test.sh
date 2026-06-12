#!/bin/bash

set -e

./build.sh
./build/test/emessgee_tests
pytest