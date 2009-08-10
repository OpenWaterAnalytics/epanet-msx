The EPANET-MSX program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program.  If not, see
<http://www.gnu.org/licenses/>.  Since the GNU Lesser General Public
License extends the rights provided under the GNU General Public
License, you may wish to obtain a copy of the latter as well from
<http://www.gnu.org/licenses/>.

The contents of this archive are as follows:

\Bin
   epanetmsx.exe        command line version of EPANET-MSX
   epanetmsx.dll        EPANET-MSX function library
   epanet2.dll          standard EPANET function library (release 2.0.12)

\Examples
   readme.txt           descriptions of the example files
   example.inp          example EPANET input file
   example.msx          example MSX input file
   example.rpt          example results file

\Doc
   epanetmsx.pdf        EPANET-MSX users manual
   license.txt          licensing agreement for using EPANET-MSX

\Include
   epanetmsx.h          C/C++ header file for EPANET-MSX function library
   epanetmsx.bas        Visual Basic declarations of EPANET-MSX functions
   epanetmsx.pas        Delphi-Pascal declarations of EPANET-MSX functions
   epanetmsx.lib        Microsoft C/C++ LIB file for epanetmsx.dll
   epanet2.h            C/C++ header file for EPANET2 function library
   epanet2.bas          Visual Basic declarations of EPANET2 functions
   epanet2.pas          Delphi-Pascal declarations of EPANET2 functions
   epanet2.lib          Microsoft C/C++ LIB file for epanet2.dll

\Src
   *.c, *.h             EPANET-MSX source code files

\Make
   \Windows
      readme.txt        instructions for compiling EPANET-MSX on Windows
      msxexe.mak        makefile for compiling epanetmsx.exe
      msxdll.mak        makefile for compiling epanetmsx.dll
   \Linux
      Makefile          makefile for compiling both a shared object library
                        and a stand-alone executable version of EPANET-MSX
                        using the GNU C++ compiler on the Linux platform
      runepanetmsx.sh   a shell wrapper for running EPANET-MSX under Linux

EPANET-MSX will only run correctly with release 2.0.11 or higher of the
EPANET2 engine which is supplied with this archive. The source code for
EPANET-MSX is copyrighted and is distributed under the GNU Lesser General
Public License. See license.txt for the terms of this license.

All requests for bug-fixes to EPANET-MSX are being managed using server
software called Bugzilla. Bugzilla is an online bug-tracking system. To
report a new EPANET-MSX bug, please do the following to ensure that your
bug is recorded for eventual action:

1. Go to http://engcvs.uc.edu and select "Bugzilla" at the top of the page.
2. If this is your first bug entry, select "open a new Bugzilla account" and
   follow the on-line instructions.
3. Enter your login (your email address) and password.
4. Select "Enter a new bug report"
5. Select "EPANET-MSX"
6. If this is your first bug report, it is helpful to first read the "bug writing
   guidelines."
7. Select the version, component, platform, operating system, and severity; enter
   a summary description of the bug, and a more detailed text description.
8. Select "commit" to store your bug report and automatically forward it to the
   developers for action.

Following the above procedures will help to ensure that EPANET-MSX bugs are
addressed in a timely manner.
