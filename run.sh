#!/bin/bash

set -e

num_cpus=$(nproc)
run=""
format=""

while getopts "rf" opt
do
    case $opt in
    (r) run="1" ;;
    (f) format="1" ;;
    (*) printf "Illegal option '-%s'\n" "$opt" && exit 1 ;;
    esac
done

format_cpp () {
    echo "Formatting $1 ..."
    clang-format --style="{BasedOnStyle: webkit, IndentWidth: 4}" -i "$1"
}

export -f format_cpp

if [[ $format ]]; then
    find . \( -path ./build -prune \) -o \( -iname "*.hpp" -o -iname "*.cpp" \) -exec bash -c 'format_cpp "$1"' _ {} \;
fi

mkdir -p build
cd build
cmake ..
make -j $num_cpus
cd ..
ln -sf build/compile_commands.json .

if [[ $run ]]; then 
    echo "Launching..."
    build/rachel_example
fi