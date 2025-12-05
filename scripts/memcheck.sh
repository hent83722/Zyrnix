#!/usr/bin/env bash
set -e
echo "██╗  ██╗██╗      ██████╗  ██████╗ "
echo "╚██╗██╔╝██║     ██╔═══██╗██╔════╝ "
echo " ╚███╔╝ ██║     ██║   ██║██║      "
echo " ██╔██╗ ██║     ██║   ██║██║   ██║"
echo "██╔╝ ██╗███████╗╚██████╔╝╚██████╔╝"
echo "╚═╝  ╚═╝╚══════╝ ╚═════╝  ╚═════╝ "
cd ~/xlog
echo "Running memory checks with Valgrind..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel

if [ -f ./test ]; then
    valgrind --leak-check=full --show-leak-kinds=all \
             --suppressions=../scripts/valgrind_suppressions.supp \
             ./test
else
    echo "No executable 'test' found."
fi
