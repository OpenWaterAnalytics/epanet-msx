#!/bin/sh
# MSX test script

# Location of MSVC build directory
vcdir=~/ENGCVS/EPANET/MSX/trunk/build/MSVC/epanet-msx-exe/Release
# VC executable
vcexe=$vcdir/epanetmsx.exe
# Cyg executable
cygexe=runcygepanetmsx_302.sh

# As adsorption example
echo
echo ---------- + + + Executing As Adsorption Example + + + ---------- 
echo
time "$vcexe"  As5Adsorb/example.inp As5Adsorb/example.msx    As5Adsorb/example.rpt    As5Adsorb/example.bwq
time "$vcexe"  As5Adsorb/example.inp As5Adsorb/example-vc.msx As5Adsorb/example-vc.rpt As5Adsorb/example-vc.bwq
time $cygexe   As5Adsorb/example.inp As5Adsorb/example.msx    As5Adsorb/examplecyg.rpt As5Adsorb/examplecyg.bwq
time $cygexe   As5Adsorb/example.inp As5Adsorb/example-gc.msx As5Adsorb/example-gc.rpt As5Adsorb/example-gc.bwq
echo "> > > Differences CYG < < <"
diff --text  As5Adsorb/example.rpt As5Adsorb/examplecyg.rpt
echo "> > > Differences VC compiler < < <"
diff --text As5Adsorb/example.rpt As5Adsorb/example-vc.rpt
echo "> > > Differences GC compiler < < <"
diff --text  As5Adsorb/example.rpt As5Adsorb/example-gc.rpt

# nh2cl batch example
echo
echo ---------- + + + Executing Batch NH2CL Example + + + ---------- 
echo
time "$vcexe"  Batch-NH2CL/batch-nh2cl.inp Batch-NH2CL/batch-nh2cl.msx    Batch-NH2CL/batch-nh2cl.rpt    Batch-NH2CL/batch-nh2cl.bwq
time "$vcexe"  Batch-NH2CL/batch-nh2cl.inp Batch-NH2CL/batch-nh2cl-vc.msx Batch-NH2CL/batch-nh2cl-vc.rpt Batch-NH2CL/batch-nh2cl-vc.bwq
time $cygexe   Batch-NH2CL/batch-nh2cl.inp Batch-NH2CL/batch-nh2cl.msx    Batch-NH2CL/batch-nh2clcyg.rpt Batch-NH2CL/batch-nh2clcyg.bwq
time $cygexe   Batch-NH2CL/batch-nh2cl.inp Batch-NH2CL/batch-nh2cl-gc.msx Batch-NH2CL/batch-nh2cl-gc.rpt Batch-NH2CL/batch-nh2cl-gc.bwq
echo "> > > Differences CYG < < <"
diff --text  Batch-NH2CL/batch-nh2cl.rpt Batch-NH2CL/batch-nh2clcyg.rpt
echo "> > > Differences VC compiler < < <"
diff --text  Batch-NH2CL/batch-nh2cl.rpt Batch-NH2CL/batch-nh2cl-vc.rpt
echo "> > > Differences GC compiler < < <"
diff --text  Batch-NH2CL/batch-nh2cl.rpt Batch-NH2CL/batch-nh2cl-gc.rpt

# CL2 example
echo
echo ---------- + + + Executing CL2 Example + + + ---------- 
echo
time "$vcexe"  Net2-CL2/net2-cl2.inp Net2-CL2/net2-cl2.msx    Net2-CL2/net2-cl2.rpt    Net2-CL2/net2-cl2.bwq
time "$vcexe"  Net2-CL2/net2-cl2.inp Net2-CL2/net2-cl2-vc.msx Net2-CL2/net2-cl2-vc.rpt Net2-CL2/net2-cl2-vc.bwq
time $cygexe   Net2-CL2/net2-cl2.inp Net2-CL2/net2-cl2.msx    Net2-CL2/net2-cl2cyg.rpt Net2-CL2/net2-cl2cyg.bwq
time $cygexe   Net2-CL2/net2-cl2.inp Net2-CL2/net2-cl2-gc.msx Net2-CL2/net2-cl2-gc.rpt Net2-CL2/net2-cl2-gc.bwq
echo "> > > Differences CYG < < <"
diff --text  Net2-CL2/net2-cl2.rpt Net2-CL2/net2-cl2cyg.rpt
echo "> > > Differences VC compiler < < <"
diff --text  Net2-CL2/net2-cl2.rpt Net2-CL2/net2-cl2-vc.rpt
echo "> > > Differences GC compiler < < <"
diff --text  Net2-CL2/net2-cl2.rpt Net2-CL2/net2-cl2-gc.rpt

# Biofilm example
echo
echo ---------- + + + Executing Biofilm Example + + + ---------- 
echo
time "$vcexe"  Net3-Bio/net3-bio.inp Net3-Bio/net3-bio.msx    Net3-Bio/net3-bio.rpt    Net3-Bio/net3-bio.bwq
time "$vcexe"  Net3-Bio/net3-bio.inp Net3-Bio/net3-bio-vc.msx Net3-Bio/net3-bio-vc.rpt Net3-Bio/net3-bio-vc.bwq
time $cygexe   Net3-Bio/net3-bio.inp Net3-Bio/net3-bio.msx    Net3-Bio/net3-biocyg.rpt Net3-Bio/net3-biocyg.bwq
time $cygexe   Net3-Bio/net3-bio.inp Net3-Bio/net3-bio-gc.msx Net3-Bio/net3-bio-gc.rpt Net3-Bio/net3-bio-gc.bwq
echo "> > > Differences CYG < < <"
diff --text  Net3-Bio/net3-bio.rpt Net3-Bio/net3-biocyg.rpt
echo "> > > Differences VC compiler < < <"
diff --text  Net3-Bio/net3-bio.rpt Net3-Bio/net3-bio-vc.rpt
echo "> > > Differences GC compiler < < <"
diff --text  Net3-Bio/net3-bio.rpt Net3-Bio/net3-bio-gc.rpt

# NH2CL example
echo
echo ---------- + + + Executing NH2CL Example + + + ---------- 
echo
time "$vcexe"  Net3-NH2CL/Net3-NH2CL.inp Net3-NH2CL/Net3-NH2CL.msx    Net3-NH2CL/Net3-NH2CL.rpt    Net3-NH2CL/Net3-NH2CL.bwq
time "$vcexe"  Net3-NH2CL/Net3-NH2CL.inp Net3-NH2CL/Net3-NH2CL-vc.msx Net3-NH2CL/Net3-NH2CL-vc.rpt Net3-NH2CL/Net3-NH2CL-vc.bwq
time $cygexe   Net3-NH2CL/Net3-NH2CL.inp Net3-NH2CL/Net3-NH2CL.msx    Net3-NH2CL/Net3-NH2CLcyg.rpt Net3-NH2CL/Net3-NH2CLcyg.bwq
time $cygexe   Net3-NH2CL/Net3-NH2CL.inp Net3-NH2CL/Net3-NH2CL-gc.msx Net3-NH2CL/Net3-NH2CL-gc.rpt Net3-NH2CL/Net3-NH2CL-gc.bwq
echo "> > > Differences CYG < < <"
diff --text  Net3-NH2CL/Net3-NH2CL.rpt Net3-NH2CL/Net3-NH2CLcyg.rpt
echo "> > > Differences VC compiler < < <"
diff --text  Net3-NH2CL/Net3-NH2CL.rpt Net3-NH2CL/Net3-NH2CL-vc.rpt
echo "> > > Differences GC compiler < < <"
diff --text  Net3-NH2CL/Net3-NH2CL.rpt Net3-NH2CL/Net3-NH2CL-gc.rpt

