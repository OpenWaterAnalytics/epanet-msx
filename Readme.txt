Contents of this archive:

\Bin
   epanet2.dll		standard EPANET toolkit DLL
   epanetmsx.dll	EPANET-MSX toolkit DLL
   epanet2.lib		LIB file for epanet2.dll
   epanetmsx.lib	LIB file for epanetmsx.dll
   epanetmsx.exe	command line version of EPANET-MSX

\Test
   enmsx.c		source code for test of epanetmsx.dll
			(produces an executable that is equivalent to
			the command line version of EPANET-MSX)
   epanet2.h		header file for EPANET2 toolkit
   epanetmsx		header file for EPANET-MSX toolkit
   net3.inp		EPANET input file for the NET3 example
   net3-cl2.msx		MSX input file for chlorine decay in NET3
   net3-cl2.rpt		output results for NET3 chlorine example

\Doc
   manual.doc		EPANET-MSX users manual (unfinished)

\Src			source files for EPANET-MSX

Notes:

1.  The epanet2.dll toolkit and its include file epanet2.h have been
    modified to expose a new parameter, EN_INITVOLUME, that can be used
    with the ENgetnodevalue function to retrieve the initial storage
    volume in a Tank node.

2.  When compling the test file enmsx.c the two .lib files must be 
    linked in and both epanet2.dll and epanetmsx.dll must be in the
    same directory as the compiled executable.
