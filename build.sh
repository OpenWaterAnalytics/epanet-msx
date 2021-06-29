#!/bin/bash

# Build Transport-Chemistry
cd Transport-Chemistry
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

cd ../..

# Build Legacy DLL
cd "Legacy DLL"
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

cd ../..

# Build CLI
cd CLI
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

cd ../..

# Copy epanetmsx.dll
cp "Legacy DLL"/build/bin/*/epanetmsx.dll CLI/build/bin/*/

# Copy transport.dll
cp Transport-Chemistry/build/bin/*/transport.dll CLI/build/bin/*/

