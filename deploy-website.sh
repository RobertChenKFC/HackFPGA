#!/bin/bash
# Build emulator
cd emulator/
mkdir -p build
cd build/
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
emmake make -j
# Remove existing files
cd ..
FILES=("index.html" "sketch.js" "HackEmulator.js" "HackEmulator.wasm" "os/")
rm -rf "${FILES[@]}"
# Copy new files
cd src/
cp -r "${FILES[@]}" ../..
