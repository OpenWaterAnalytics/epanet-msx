Microsoft Visual Studio
-----------------------
The following configuration properties should be set in the
properties for all configurations of the MSX VC project, to ensure
generation of a correct DLL and executable.

General -> Character Set -> NOT SET (uses ASCII character set)
C/C++   -> Detect 64-bit Portability Issues -> NO (prevents a "deprecated option" warning message from appearing)
C/C++   -> Code Generation -> Runtime Library -> Multi-threaded DLL (Release) -or- Multi-threaded Debug DLL (Debug)
C/C++   -> Code Generation -> Floating Point Model -> Precise
C/C++   -> Advanced -> Compile As -> Compile As C Code
C/C++   -> Preprocessor -> Preprocessor Definitions -> _CRT_SECURE_NO_WARNINGS (prevents "non safe code" warning messages)


These compilation instructions are for the Microsoft Visual C++
6.0 compiler.

To compile the function library form of EPANET-MSX:

1. Create a sub-directory named MSXDLL under the directory where
   the EPANET-MSX source code files are stored and copy MSXDLL.MAK to it.

2. Make sure that the EPANET2.LIB file is also present in the EPANET-MSX
   source code directory.

3. Open a command line window and navigate to the MSXDLL directory.

4. Issue the command:

      nmake /f MSXDLL.MAK

   to create EPANETMSX.DLL which will be found in the MSXDLL\Release
   directory.

To compile the command line executable version of EPANET-MSX:

1. Create a sub-directory named MSXEXE under the directory where
   the EPANET-MSX source code files are stored and copy MSXEXE.MAK to it.

2. Make sure that the EPANET2.LIB file is also present in the EPANET-MSX
   source code directory.

3. Open a command line window and navigate to the MSXEXE directory.

4. Issue the command:

      nmake /f MSXEXE.MAK

   to create EPANETMSX.EXE which will be found in the MSXEXE\Release
   directory.
