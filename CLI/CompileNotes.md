Compiling with CMake


Compiling
1. Create a directory named "build" inside the root directory.
2. Navigate into the directory.
3. Run `cmake ..`
4. Run `cmake --build . --config Release`


The runepanetmsx.exe will be built in the bin/Release directory.
In order to run the executable, both epanet2.dll and epanetmsx.dll will be required to be present in the bin/Release directory as well.

