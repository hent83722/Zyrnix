#!/usr/bin/env bash
set -e
echo "██╗  ██╗██╗      ██████╗  ██████╗ "
echo "╚██╗██╔╝██║     ██╔═══██╗██╔════╝ "
echo " ╚███╔╝ ██║     ██║   ██║██║      "
echo " ██╔██╗ ██║     ██║   ██║██║   ██║"
echo "██╔╝ ██╗███████╗╚██████╔╝╚██████╔╝"
echo "╚═╝  ╚═╝╚══════╝ ╚═════╝  ╚═════╝ "
cd ~/xlog
echo "Running static analysis with clang-tidy..."
mkdir -p build
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cd ..

FILES=$(find src include -name '*.cpp' -o -name '*.hpp')

for f in $FILES; do
    clang-tidy "$f" -- -std=c++17
done

echo "Static analysis complete!"
