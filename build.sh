#!/bin/bash

set -e

num_cpus=$(nproc)

mkdir -p build
cd build 
cmake ..
make -j $num_cpus
cd ..
ln -sf build/compile_commands.json .

if [[ "$1" == "-r" ]]; then 
    build/rachel_example
fi