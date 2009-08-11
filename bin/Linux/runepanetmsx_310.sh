#!/bin/sh
export PATH=~/lib:$PATH
export LD_LIBRARY_PATH=~/lib:$LD_LIBRARY_PATH
exec ~/bin/epanetmsx_gcc_310 "$@"
