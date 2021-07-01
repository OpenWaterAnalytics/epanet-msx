#!/bin/bash

printf "\n\n-------- Building MSX Core --------\n\n"

# Build MSX Core
cd MSX\ Core
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

cd ../..

printf "\n\n-------- Building Legacy DLL --------\n\n"

# Build Legacy DLL
cd "Legacy DLL"
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

cd ../..

printf "\n\n-------- Building CLI --------\n\n"

# Build CLI
cd CLI
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

cd ../..

# Copy epanetmsx.dll
cp "Legacy DLL"/build/bin/*/legacy.dll CLI/build/bin/*/

# Copy core.dll
cp MSX\ Core/build/bin/*/core.dll CLI/build/bin/*/

