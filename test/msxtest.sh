#!/bin/sh
# MSX test script

# WDSSim parameters
version=7
storagemethod=1

# As adsorption example
echo + + + Executing As Adsorption Example + + +
runwdssim.sh -s $storagemethod -v $version -m -tso As5Adsorb/example.tso As5Adsorb/example.inp
