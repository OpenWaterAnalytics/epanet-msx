# Microsoft Developer Studio Generated NMAKE File, Based on msxexe.dsp
!IF "$(CFG)" == ""
CFG=msxexe - Win32 Release
!MESSAGE No configuration specified. Defaulting to msxexe - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "msxexe - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "msxexe.mak" CFG="msxexe - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msxexe - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\epanetmsx.exe"


CLEAN :
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\mathexpr.obj"
	-@erase "$(INTDIR)\mempool.obj"
	-@erase "$(INTDIR)\msxchem.obj"
	-@erase "$(INTDIR)\msxcompiler.obj"
	-@erase "$(INTDIR)\msxfile.obj"
	-@erase "$(INTDIR)\msxfuncs.obj"
	-@erase "$(INTDIR)\msxinp.obj"
	-@erase "$(INTDIR)\msxmain.obj"
	-@erase "$(INTDIR)\msxout.obj"
	-@erase "$(INTDIR)\msxproj.obj"
	-@erase "$(INTDIR)\msxqual.obj"
	-@erase "$(INTDIR)\msxrpt.obj"
	-@erase "$(INTDIR)\msxtank.obj"
	-@erase "$(INTDIR)\msxtoolkit.obj"
	-@erase "$(INTDIR)\msxutils.obj"
	-@erase "$(INTDIR)\newton.obj"
	-@erase "$(INTDIR)\rk5.obj"
	-@erase "$(INTDIR)\ros2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\epanetmsx.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\msxexe.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\epanetmsx.pdb" /machine:I386 /out:"$(OUTDIR)\epanetmsx.exe" 
LINK32_OBJS= \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\mathexpr.obj" \
	"$(INTDIR)\mempool.obj" \
	"$(INTDIR)\msxchem.obj" \
	"$(INTDIR)\msxcompiler.obj" \
	"$(INTDIR)\msxfile.obj" \
	"$(INTDIR)\msxfuncs.obj" \
	"$(INTDIR)\msxinp.obj" \
	"$(INTDIR)\msxmain.obj" \
	"$(INTDIR)\msxout.obj" \
	"$(INTDIR)\msxproj.obj" \
	"$(INTDIR)\msxqual.obj" \
	"$(INTDIR)\msxrpt.obj" \
	"$(INTDIR)\msxtank.obj" \
	"$(INTDIR)\msxtoolkit.obj" \
	"$(INTDIR)\msxutils.obj" \
	"$(INTDIR)\newton.obj" \
	"$(INTDIR)\rk5.obj" \
	"$(INTDIR)\ros2.obj" \
	"..\epanet2.lib"

"$(OUTDIR)\epanetmsx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

CPP_PROJ=/nologo /ML /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\msxexe.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("msxexe.dep")
!INCLUDE "msxexe.dep"
!ELSE 
!MESSAGE Warning: cannot find "msxexe.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "msxexe - Win32 Release"
SOURCE=..\hash.c

"$(INTDIR)\hash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\mathexpr.c

"$(INTDIR)\mathexpr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\mempool.c

"$(INTDIR)\mempool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxchem.c

"$(INTDIR)\msxchem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxcompiler.c

"$(INTDIR)\msxcompiler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxfile.c

"$(INTDIR)\msxfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxfuncs.c

"$(INTDIR)\msxfuncs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxinp.c

"$(INTDIR)\msxinp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxmain.c

"$(INTDIR)\msxmain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxout.c

"$(INTDIR)\msxout.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxproj.c

"$(INTDIR)\msxproj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxqual.c

"$(INTDIR)\msxqual.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxrpt.c

"$(INTDIR)\msxrpt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxtank.c

"$(INTDIR)\msxtank.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxtoolkit.c

"$(INTDIR)\msxtoolkit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\msxutils.c

"$(INTDIR)\msxutils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\newton.c

"$(INTDIR)\newton.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\rk5.c

"$(INTDIR)\rk5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\ros2.c

"$(INTDIR)\ros2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

