# Build instructions
1. Install emscripten from [here](https://emscripten.org/docs/getting_started/downloads.html)
2. Create build folder and copy appropriate files
```
mkdir build && cd build
cp -r ../src/prefix.js ../src/postfix.js ../src/jack-os/ .
```
3. Run cmake with emscripten
```
emcmake cmake ..
emmake cmake --build . -- -j24
```
4. Run server
```
cd ../src
./run_server.sh
```
