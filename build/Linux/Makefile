# Linux Makefile for EPANET-MSX

# This will build EPANET-MSX as a shared object
# library (libepanetmsx.so) under Linux,
# and a standalone executable (epanetmsx) that is linked
# to this shared object library.

# The following targets are defined (for execution in
# the build directory):
# make
#     -Builds libepanetmsx.so, epanetmsx
# make install
#     -Creates shell wrapper runepanetmsx.sh that
#      executes epanetmsx.
#      The runepanetmsx.sh wrapper simply exports
#      environment variables so that the library is found at runtime,
#      allowing you to specify your own locations for installing
#      the libraries.
#     -Installs epanetmsx, and runepanetmsx.sh
#      in <prefix>/bin, where <prefix> defaults to ~ (and can be set
#      below to something different - see "Install directories")
#     -Installs libepanetmsx.so (shared library) in <prefix>/lib
#     -Installs epanetmsx.h in <prefix>/include.  This is the required
#      header file for the EPANET programmer's toolkit, and thus
#      <prefix>/include should be on your CPP include search path
#      for subsequent use of the toolkit and linking with the shared
#      library libepanetmsx.so
# make clean
#     -Removes object and library files, returning the build directory
#      to its pristine state.

SHELL = /bin/sh

# C H A N G E  H E R E  A S  N E E D E D
# You may wish to change the install path 'prefix',
# or the compiler flags, but these defaults should be fine.

# Target filenames
epanetrootname := epanet2
epanetmsxrootname := epanetmsx
soname := lib$(epanetmsxrootname).so
exename := $(epanetmsxrootname)
# Shell wrapper
runcmdtemplate = runepanetmsx.sh.template
runcmdname = runepanetmsx.sh
# Location of EPANET-MSX toolkit includes
epanetmsxincludedir = ../../include
# Search path for sources
epanetmsxsrcdir = ../../src
VPATH = $(epanetmsxsrcdir)

# Install directories
prefix = ~
exec_prefix = $(prefix)
srcdir = .
libdir = $(exec_prefix)/lib
bindir = $(exec_prefix)/bin
includedir = $(prefix)/include
datarootdir = $(prefix)/share
docdir = $(datarootdir)/doc/epanetmsx

# Compiler and flags
# gcc
CC = gcc
CFLAGS = -g -O3 -fPIC
CPPFLAGS = -I $(srcdir) -I $(epanetmsxincludedir) -I $(includedir)
LDFLAGS = -L . -L $(libdir) -W1,-rpath,$(libdir) -lm -ldl

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
all: $(soname) $(exename)

$(soname): $(epanetmsx_objs)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(exename): $(soname) $(epanetmsx_main_heads)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(epanetmsxsrcdir)/$(epanetmsx_main).c
	$(CC) $(CFLAGS) -o $@ $(epanetmsx_main).o -l$(epanetmsxrootname) -l$(epanetrootname) $(LDFLAGS)

$(epanetmsx_objs): $(epanetmsx_heads)

.PHONY: install
install:
	cat $(runcmdtemplate) | sed 's|libdir|$(libdir)|' \
                                | sed 's|exename|$(bindir)/$(exename)|' \
                                > $(runcmdname)
	$(INSTALL_PROGRAM) -D $(exename) $(bindir)/$(exename)
	$(INSTALL_PROGRAM) -D $(soname) $(libdir)/$(soname)
	$(INSTALL_DATA) -D $(epanetmsxincludedir)/epanetmsx.h $(includedir)/epanetmsx.h
	$(INSTALL_PROGRAM) -D $(runcmdname) $(bindir)/$(runcmdname)

.PHONY: uninstall
uninstall:

.PHONY: check
check:

.PHONY: clean
clean:
	-/bin/rm *.o $(soname) $(exename) $(runcmdname)
