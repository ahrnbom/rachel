#!/bin/bash

set -e

mkdir -p build
cd build 
cmake ..
make
ln -sf build/compile_commands.json ..