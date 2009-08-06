I.Compilation instructions for the Microsoft Visual C++ 6.0 compiler
====================================================================

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


II. Compilation instructions for the Gnu C++ compiler under Linux
==================================================================
If you haven't already done so, create the Epanet shared object library
file libepanet.so by running the shell script file create_epanet_lib in
the directory where the Epanet source files are located. Then copy the
file libepanet.so to the directory where the Epanet-MSX source files are
loacted.

To build a shared object library of Epanet-MSX, run the shell script file
named create_msx_lib in the Epanet-MSX source code directory. The library
file will be named libepanetmsx.so.

To build a command line executable version of Epanet-MSX, run the shell
script file named create_msx_exe in the Epanet-MSX source code directory.
The resulting execuatble will be named epanetmsx.


