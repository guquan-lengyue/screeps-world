mkdir -p build
cd build
cmake  -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/home/kang/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake ..
cmake  --build . -- -j 4