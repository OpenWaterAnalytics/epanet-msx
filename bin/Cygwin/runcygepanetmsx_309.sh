#!/bin/sh
export PATH=~/bin:$PATH
export LD_LIBRARY_PATH=~/bin:$LD_LIBRARY_PATH
exec ~/bin/cygepanetmsx_gcc_309.exe "$@"
