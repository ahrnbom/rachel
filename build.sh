#!/bin/bash

set -e

num_cpus=$(nproc)

mkdir -p build
cd build 
cmake ..
make -j $num_cpus
ln -sf build/compile_commands.json ..