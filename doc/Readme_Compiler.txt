2. [IGNORED] Error 522 is output to screen as "Cannot read
EPANET-MSX file" but in report file is "could not compile
chemistry functions" which is undoubtedly the truth.

3. The reason for Error 2 is possibly that VS2008 was not set
up for command line builds, as suggested in the manual.
However the manual could be more explicit perhaps.  For
reference see "Setting the Path and Environment Variables for
Command-Line Builds" in the VS2008 help.  This describes how to
use the vcvars32.bat batch file to set up the needed
environment variables. Annoyingly, the changes made to the
necessary environment variables are not permanent, but only for
that process.  To avoid having to use vcvars32.bat each time,
follow the following procedure.

- open a command window, cd to the bin directory for VS, and
run vcvars32.bat - export the current environment variables to
a file, env_list.txt, by set > C:\env_list.txt - Open
env_list.txt in a text editor (e.g., notepad, winedt), and copy
and paste the values for the following environment variables:
PATH LIB LIBPATH INCLUDE into the predefined environment
variables under the "system properties" dialog, the advanced
tab.  The environment variables LIB, LIBPATH, and INCLUDE will
likely not be defined already, and you will need to create new
environment variables with these names.
