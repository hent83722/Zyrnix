#!/usr/bin/env bash
set -e
echo "██╗  ██╗██╗      ██████╗  ██████╗ "
echo "╚██╗██╔╝██║     ██╔═══██╗██╔════╝ "
echo " ╚███╔╝ ██║     ██║   ██║██║      "
echo " ██╔██╗ ██║     ██║   ██║██║   ██║"
echo "██╔╝ ██╗███████╗╚██████╔╝╚██████╔╝"
echo "╚═╝  ╚═╝╚══════╝ ╚═════╝  ╚═════╝ "
cd ~/xlog
echo "Running tests..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel


if [ -f ./test ]; then
    ./test
else
    echo "No executable 'test' found. Make sure you have a test program."
fi
