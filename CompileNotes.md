Compiling with CMake

Steps to do before Compiling
1. Make sure the EPANET repository is cloned alongside MSX (common root directory), as dependencies are listed as relative paths.

Compiling
1. Create a directory to store the build inside the root directory.
2. Navigate into the directory.
3. Run `cmake ..`
4. Run `cmake --build . --config Release`

In Windows:

Navigate into the bin folder of your build and put `epanet2.dll` inside.
Now the executable will work successfully.
Run examples using the format: `runepanetmsx example.inp example.msx example.rpt`
