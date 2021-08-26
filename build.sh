#!/bin/bash

printf "\n\n-------- Building MSX Core --------\n\n"

# Build MSX Core
cd MSX\ Core
rm -rf build
mkdir build && cd build
cmake ..
cmake --build . --config Release

cd ../..

printf "\n\n-------- Building Legacy DLL --------\n\n"

# Build Legacy DLL
cd "Legacy DLL"
rm -rf build
mkdir build && cd build
cmake ..
cmake --build . --config Release

cd ../..

printf "\n\n-------- Building Examples --------\n\n"

# Build CLI
cd example-api
rm -rf build
mkdir build && cd build
cmake ..
cmake --build . --config Release

cd ../..

printf "\n\n-------- Building CLI --------\n\n"

# Build CLI
cd CLI
rm -rf build
mkdir build && cd build
cmake ..
cmake --build . --config Release

cd ../..

# Copy legacymsx.dll and epanet2.dll
cp "Legacy DLL"/build/bin/*/legacymsx.dll CLI/build/bin/*/
cp ../EPANET/build/bin/*/epanet2.dll CLI/build/bin/*/

# Copy msxcore.dll
cp MSX\ Core/build/bin/*/msxcore.dll CLI/build/bin/*/

# Copy examples.dll
cp example-api/build/bin/*/examples.dll CLI/build/bin/*/

