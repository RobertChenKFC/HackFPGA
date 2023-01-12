#!/bin/bash
mkdir build
cd build
cp -r ../src/prefix.js ../src/postfix.js ../src/jack-os .
emcmake cmake .. -DCMAKE_BUILD_TYPE=DEBUG
emmake make -j24
