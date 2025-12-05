#!/usr/bin/env bash
set -e
cd ~/xlog
echo "Installing XLog..."
bash scripts/build.sh
cd build
sudo cmake --install .
echo "XLog installed to system directories!"
