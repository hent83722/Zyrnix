#!/usr/bin/env bash
set -e
echo "██╗  ██╗██╗      ██████╗  ██████╗ "
echo "╚██╗██╔╝██║     ██╔═══██╗██╔════╝ "
echo " ╚███╔╝ ██║     ██║   ██║██║      "
echo " ██╔██╗ ██║     ██║   ██║██║   ██║"
echo "██╔╝ ██╗███████╗╚██████╔╝╚██████╔╝"
echo "╚═╝  ╚═╝╚══════╝ ╚═════╝  ╚═════╝ "
cd ~/xlog
echo "Building XLog..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
echo "Build complete!"
