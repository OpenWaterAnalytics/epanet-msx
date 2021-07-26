# EPANET-MSX

The multi-species extension to the EPANET hydraulic analysis engine.

## Core Toolkit Usage
Notes:
Make sure that MSX_init() is called after everything is added, this must be done before
any simulations are done. MSX_init() also must be called before Hydraulics are set.


## Building
In order to build, it is easiest to have CMake (https://cmake.org/) installed.
The easiest way to build quickly is to run the bash script titled "build.sh" from
the root directory. If you can't run the bash file, the instructions will be below.

It is important to build all three modules (MSX Core, Legacy DLL, and CLI) in the correct order
as some of them have dependencies on each other.
First, build the MSX Core module since it has no dependencies. Navigate into the MSX Core directory
and create a new directory titled "build". Then navigate into that directory and run the cmake command.
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
Next, in order to build the Legacy DLL module, it is necessary to have a built version of the EPANET
repository outside of the epanet-msx repository. Then follow the same steps as used in the MSX Core
compilation.
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Lastly, to compile the CLI module, follow the same steps as used to compile the MSX Core. Note that it is
important that the other two modules were compiled successfully in order for the CLI to compile successfully.
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

In order for the executable to work, the following DLL's will need to be in the same directory: "core.dll",
"legacy.dll", and "epanet2.dll". The EPANET 2 DLL is required because it is linked with the Legacy DLL.

