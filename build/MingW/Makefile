# Cygwin/MinGW Makefile for EPANET-MSX

# This will build EPANET-MSX as a native windows DLL and import
# library (epanetmsx_gcc_<Rev>.dll/libepanetmsx_gcc_<Rev>.dll.a) under Cygwin/MinGW,
# and a standalone executable (epanetmsx_gcc_<Rev>.exe) that is linked
# to this import library.
# <Rev> is the atomic revision number of the EPANET SVN repo,
# so the results of each build can be unambiguously tracked to a repo Rev.
# This makefile also creates a windows batch file
# (see MScmdname below) that will produce an import
# library for linking the DLL using MSVC.
# This makefile assumes a Cygwin environment with minimal
# MinGW installation, and uses the MinGW gcc compiler from
# the MinGW install directory.
# The current release of MinGW can be obtained from
# http://www.mingw.org/wiki/Getting_Started -- the easiest
# approach is to download the latest MingW installer and
# accept all of the defaults.
# The current release of the Cygwin environment can be
# obtained from http://www.cygwin.com

# The following targets are defined (for execution in
# the build directory under the Cygwin environment):
# make
#     -Builds epanetmsx_gcc_<Rev>.dll, epanetmsx_gcc_<Rev>.def, libepanetmsx_gcc_<Rev>.dll.a,
#      epanetmsx_gcc_<Rev>.exe
# make install
#     -Creates shell wrappers runepanetmsx_<Rev>.sh and CreateEpanetMSXLib_<Rev>.bat that
#      execute epanetmsx_gcc_<Rev>.exe, and MSVC 'lib' (to create an MSVC
#      import library).  The runepanetmsx_<Rev>.sh wrapper simply exports
#      environment variables so that the DLL is found at runtime,
#      allowing you to specify your own locations for installing
#      the DLL.
#     -Installs epanetmsx_gcc_<Rev>.dll, epanetmsx_gcc_<Rev>.exe, and runepanetmsx_<Rev>.sh
#      in <prefix>/bin, where <prefix> defaults to ~ (and can be set
#      below to something different - see "Install directories")
#     -Installs libepanetmsx_gcc_<Rev>.dll.a (import library), epanetmsx_gcc_<Rev>.def,
#      and CreateEpanetMSXLib-<Rev>.bat in <prefix>/lib
#     -Installs epanetmsx.h in <prefix>/include.  This is the required
#      header file for the EPANET programmer's toolkit, and thus
#      <prefix>/include should be on your CPP include search path
#      for subsequent use of the toolkit and linking with the import
#      library libepanetmsx_gcc_<Rev>.dll.a.
# make clean
#     -Removes object and library files, returning the build directory
#      to its pristine state.

SHELL = /bin/sh

# C H A N G E  H E R E  A S  N E E D E D
# Change (as needed) the MinGW install directory below.
# Change (as needed) the location off the MS LIB tool.
# You may also wish to change the install path 'prefix',
# or the compiler flags, but these defaults should be fine.

# MinGW top level install directory, accessible from build environment
MinGWdir = /cygdrive/c/MinGW
# Microsoft lib tool directory (for creating an MS import library)
MSVClibexe = c:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\bin\\lib

# Target filenames
# svnname.sh constructs a name: <prefix><root><Rev><postfix>
# where <Rev> is the atomic revision number of the svn repo.
epanetsvnpath = ../../../../BASE
epanetmsxsvnpath = ../../..
epanetmsxrootname := $(shell ../svnname.sh $(epanetmsxsvnpath) "" epanetmsx_gcc_ "")
dllname := $(epanetmsxrootname).dll
defname := $(epanetmsxrootname).def
implibname = lib$(dllname).a
exename := $(epanetmsxrootname).exe
# Shell wrapper
runcmdtemplate = runepanetmsx.sh.template
runcmdname = $(shell ../svnname.sh $(epanetmsxsvnpath) "" runepanetmsx_ .sh)
# MSVC import lib batch file
MScmdtemplate = CreateEpanetMSXLib.bat.template
MScmdname = $(shell ../svnname.sh $(epanetmsxsvnpath) "" CreateEpanetMSXLib_ .bat)
# Location of EPANET-MSX toolkit includes
epanetincludedir = $(epanetsvnpath)/trunk/include
epanetmsxincludedir = ../../include
# Search path for sources
epanetmsxsrcdir = ../../src
# dependent libraries
epanetlibname := $(shell ../svnname.sh $(epanetsvnpath) "" epanet_gcc_ .dll)
VPATH = $(epanetmsxsrcdir)

# Install directories
winprefix = $(shell cygpath -w $$HOME)
prefix = ~
exec_prefix = $(prefix)
srcdir = .
libdir = $(exec_prefix)/lib
winlibdir = "$(winprefix)\lib"
bindir = $(exec_prefix)/bin
includedir = $(prefix)/include
winincludedir = "$(winprefix)\include"
datarootdir = $(prefix)/share
docdir = $(datarootdir)/doc/epanet

# Compiler and flags
# MinGW gcc
CC = $(MinGWdir)/bin/gcc
dlltool = $(MinGWdir)/bin/dlltool
CFLAGS = -g -O3
CPPFLAGS = -I $(srcdir) -I $(epanetmsxincludedir) -I $(epanetincludedir)
LDFLAGS = -L . -L $(winlibdir) -W1,-rpath,$(bindir) -l$(epanetlibname)

# Installer
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# Files for the shared object library
epanetmsx_objs=hash.o mempool.o mathexpr.o msxchem.o msxfile.o msxinp.o msxout.o msxproj.o \
	              msxqual.o msxrpt.o msxtank.o msxtoolkit.o msxutils.o newton.o rk5.o ros2.o \
	              msxcompiler.o msxfuncs.o
# Epanetmsx main program
epanetmsx_main=msxmain
# Epanetmsx header files
epanetmsx_heads=hash.h mempool.h mathexpr.h msxdict.h msxtypes.h msxutils.h \
	               newton.h rk5.h ros2.h msxfuncs.h $(epanetmsxincludedir)/epanetmsx.h
# Epanetmsx main program header files
epanetmsx_main_heads = $(epanetmsxincludedir)/epanetmsx.h

.PHONY: all
all: $(dllname) $(exename)

$(dllname): $(epanetmsx_objs)
#	$(dlltool) -z $(defname) --dllname $(dllname) --output-lib $(implibname) $^
#	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $(defname) $^
	$(CC) $(CFLAGS) $(CPPFLAGS) -DDLL -c $(epanetmsxsrcdir)/$(epanetmsx_main).c
	$(CC) $(CFLAGS) -shared -o $@ $(epanetmsx_main).o $^ -Wl,--output-def,$(defname) $(LDFLAGS)
	$(CC) $(CFLAGS) -shared -o $@ $^ -Wl,--kill-at $(LDFLAGS)
	$(dlltool) -d $(defname) --dllname $@ --output-lib $(implibname) --kill-at

$(exename): $(dllname) $(epanetmsx_main_heads)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(epanetmsxsrcdir)/$(epanetmsx_main).c
	$(CC) $(CFLAGS) -o $@ $(epanetmsx_main).o -l$(dllname) $(LDFLAGS)

$(epanetmsx_objs): $(epanetmsx_heads)

.PHONY: install
install:
	cat $(runcmdtemplate) | sed 's|libdir|$(bindir)|' \
                                | sed 's|exename|$(bindir)/$(exename)|' \
                                > $(runcmdname)
	cat $(MScmdtemplate) | sed 's|MSVClibexe|$(MSVClibexe)|' \
                                | sed 's|defname|$(defname)|' \
                                | sed 's|libdir|$(libdir)|' \
                                > $(MScmdname)
	$(INSTALL_PROGRAM) -D $(exename) $(bindir)/$(exename)
	$(INSTALL_PROGRAM) -D $(dllname) $(bindir)/$(dllname)
	$(INSTALL_PROGRAM) -D $(defname) $(libdir)/$(defname)
	$(INSTALL_PROGRAM) -D $(implibname) $(libdir)/$(implibname)
	$(INSTALL_DATA) -D $(epanetmsxincludedir)/epanetmsx.h $(includedir)/epanetmsx.h
	$(INSTALL_PROGRAM) -D $(runcmdname) $(bindir)/$(runcmdname)
	$(INSTALL_PROGRAM) -D $(MScmdname) $(libdir)/$(MScmdname)

.PHONY: uninstall
uninstall:

.PHONY: check
check:

.PHONY: clean
clean:
	-/bin/rm *.o $(dllname) $(defname) $(implibname) $(exename) $(runcmdname) $(MScmdname)
