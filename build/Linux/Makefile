# Linux Makefile for EPANET-MSX

# This will build EPANET-MSX as a shared object
# library (libepanetmsx_gcc_<Rev>.so) under Linux,
# and a standalone executable (epanetmsx_gcc_<Rev>) that is linked
# to this shared object library.
# <Rev> is the atomic revision number of the EPANET SVN repo,
# so the results of each build can be unambiguously tracked to a repo Rev.

# The following targets are defined (for execution in
# the build directory):
# make
#     -Builds libepanetmsx_gcc_<Rev>.so, epanetmsx_gcc_<Rev>
# make install
#     -Creates shell wrapper runepanetmsx_<Rev>.sh that
#      executes epanetmsx_gcc_<Rev>.
#      The runepanetmsx_<Rev>.sh wrapper simply exports
#      environment variables so that the library is found at runtime,
#      allowing you to specify your own locations for installing
#      the libraries.
#     -Installs epanetmsx_gcc_<Rev>, and runepanetmsx_<Rev>.sh
#      in <prefix>/bin, where <prefix> defaults to ~ (and can be set
#      below to something different - see "Install directories")
#     -Installs libepanetmsx_gcc_<Rev>.so (shared library) in <prefix>/lib
#     -Installs epanetmsx.h in <prefix>/include.  This is the required
#      header file for the EPANET programmer's toolkit, and thus
#      <prefix>/include should be on your CPP include search path
#      for subsequent use of the toolkit and linking with the shared
#      library libepanetmsx_gcc_<Rev>.so
# make clean
#     -Removes object and library files, returning the build directory
#      to its pristine state.
# ---FUTURE NEEDED TARGETS---
# make uninstall
#     -Undoes 'make install'
# make check
#     -Checks the installation by running EPANET-MSX against defined
#      test networks and comparing the results quantitatively to a database
#      of known correct results.  Reports comparison statistics and
#      pass/fail status for each test.

SHELL = /bin/sh

# C H A N G E  H E R E  A S  N E E D E D
# You may wish to change the install path 'prefix',
# or the compiler flags, but these defaults should be fine.

# Target filenames
# svnname.sh constructs a name: <prefix><root><Rev><postfix>
# where <Rev> is the atomic revision number of the svn repo.
epanetsvnpath = ../../../../BASE/trunk
epanetrootname := $(shell ../svnname.sh $(epanetsvnpath) "" epanet_gcc_ "")
epanetmsxsvnpath = ../../../trunk
epanetmsxrootname := $(shell ../svnname.sh $(epanetmsxsvnpath) "" epanetmsx_gcc_ "")
soname := lib$(epanetmsxrootname).so
exename := $(epanetmsxrootname)
# Shell wrapper
runcmdtemplate = runepanetmsx.sh.template
runcmdname = $(shell ../svnname.sh $(epanetmsxsvnpath) "" runepanetmsx_ .sh)
# Location of EPANET-MSX toolkit includes
epanetincludedir = $(epanetsvnpath)/include
epanetmsxincludedir = ../../include
epanetcommondir = $(epanetsvnpath)/common
# Search path for sources
epanetmsxsrcdir = ../../src
VPATH = $(epanetmsxsrcdir):$(epanetcommondir)

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
CFLAGS = -g -O -fPIC
CPPFLAGS = -I $(srcdir) -I $(epanetmsxincludedir) -I $(epanetincludedir) -I $(epanetcommondir)
LDFLAGS = -L . -L $(libdir) -W1,-rpath,$(libdir)

# Installer
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# Files for the shared object library
epanetmsx_objs=chemistry.o hash.o inputmsx.o mathexpr.o mempool.o newton.o outputmsx.o \
	           project.o qualitymsx.o reportmsx.o rk5.o ros2.o toolkitmsx.o utils.o msxfile.o
# Epanetmsx main program
epanetmsx_main_obj=epanetmsx.o
# Epanetmsx header files
epanetmsx_heads=globals.h hash.h mathexpr.h mempool.h newton.h rk5.h ros2.h typesmsx.h utils.h \
	               $(epanetmsxincludedir)/epanetmsx.h
# Epanetmsx main program header files
epanetmsx_main_heads = $(epanetmsxincludedir)/epanetmsx.h

.PHONY: all
all: $(soname) $(exename)

$(soname): $(epanetmsx_objs)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(exename): $(epanetmsx_main_obj) $(soname) $(epanetmsx_main_heads)
	$(CC) $(CFLAGS) $< -o $@ -l$(epanetmsxrootname) -l$(epanetrootname) $(LDFLAGS)

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
