Cygwin and Linux Compiler Setup (gcc)
-------------------------------------
A standard installation of gcc on cygwin/windows or Linux
should permit use of the COMPILER GC option without any
modifications to the environment.


Visual Studio Compiler Setup (CL.exe)
-------------------------------------
The Visual C++ 2008 compiler can be downloaded for free from
the Microsoft site at http://www.microsoft.com/express/vc/.
Alternatively you may already have purchased and installed the
Visual Studio C++ integrated development environment. In either
case, you will have installed the Visual C++ compiler, CL.EXE.

The standard installation of Visual Studio 2008 or the Visual
C++ 2008 compiler does not set up the environment for command
line compilation, and this is a requirement for using the MSX
compilation option COMPILER VC. Following are instructions in
setting up your environment that have worked for us; for
further reference see "Setting the Path and Environment
Variables for Command-Line Builds" in the Visual Studio 2008
help or on the web. That document describes use of the provided
vcvars32.bat batch file to set up the needed environment
variables. Annoyingly, however, the changes made to the
necessary environment variables are not permanent, but only for
that dos command prompt process. Follow the following procedure
to set up your environment variables one time.

1. Open a command prompt window, change (cd) to the bin
directory for VS (most likely C:\Program Files\Microsoft Visual
Studio 9.0\VC\bin).

2. Execute the batch file "vcvars32.bat" at the command prompt.

3. Export the current environment variables to a file,
env_list.txt
     set > C:\env_list.txt
Close the command prompt window.

4. Open env_list.txt in a text editor (e.g., notepad).

5. Right-click on "My Computer"; select "properties" and then
the "advanced" tab. At the bottom select "Environment
Variables".

6. Copy the values for the environment variables PATH, LIB,
LIBPATH, DevEnvDir, and INCLUDE, from the file env_list.txt
(just the values specified to the right of the = sign). Paste
them into the environment variables of the same name by
selecting the environment variable from the list and selecting
"edit". Completely replace the current value with the new
value. The environment variables LIB, LIBPATH, DevEnvDir, and
INCLUDE may not be defined, in which case you will need to
create new environment variables with these names by selecting
"new". Select "OK" after this is completed.

7. Test your environment by opening up a new command prompt
window and typing "CL" at the prompt. You should see output
indicating the proper usage of the CL compiler, showing that
the operating system is finding the compiler within your
environment. If that is successful, then the MSX COMPILER VC
option is enabled.
