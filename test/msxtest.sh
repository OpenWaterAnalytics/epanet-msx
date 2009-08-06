#!/bin/sh
# MSX test script

# VC executable
vcexe=epanetmsx.exe
# Cyg executable
cygexe=runcygepanetmsx_291.sh

# As adsorption example
echo
echo + + + Executing As Adsorption Example + + +
$vcexe  As5Adsorb/example.inp As5Adsorb/example.msx    As5Adsorb/example.rpt    As5Adsorb/example.bwq
$vcexe  As5Adsorb/example.inp As5Adsorb/example-vc.msx As5Adsorb/example-vc.rpt As5Adsorb/example-vc.bwq
$cygexe As5Adsorb/example.inp As5Adsorb/example-gc.msx As5Adsorb/example-gc.rpt As5Adsorb/example-gc.bwq
echo > > > Differences VC compiler < < <
diff --text --side-by-side As5Adsorb/example.rpt As5Adsorb/example-vc.rpt
echo > > > Differences GC compiler < < <
diff --text --side-by-side As5Adsorb/example.rpt As5Adsorb/example-gc.rpt

# nh2cl batch example
echo + + + Executing Batch NH2CL Example + + +
$vcexe  Batch-NH2CL/batch-nh2cl.inp Batch-NH2CL/batch-nh2cl.msx    Batch-NH2CL/batch-nh2cl.rpt    Batch-NH2CL/batch-nh2cl.bwq
$vcexe  Batch-NH2CL/batch-nh2cl.inp Batch-NH2CL/batch-nh2cl-vc.msx Batch-NH2CL/batch-nh2cl-vc.rpt Batch-NH2CL/batch-nh2cl-vc.bwq
$cygexe Batch-NH2CL/batch-nh2cl.inp Batch-NH2CL/batch-nh2cl-gc.msx Batch-NH2CL/batch-nh2cl-gc.rpt Batch-NH2CL/batch-nh2cl-gc.bwq
echo > > > Differences VC compiler < < <
diff --text --side-by-side Batch-NH2CL/batch-nh2cl.rpt Batch-NH2CL/batch-nh2cl-vc.rpt
echo > > > Differences GC compiler < < <
diff --text --side-by-side Batch-NH2CL/batch-nh2cl.rpt Batch-NH2CL/batch-nh2cl-gc.rpt

# CL2 example
echo + + + Executing CL2 Example + + +
$vcexe  Net2-CL2/net2-cl2.inp Net2-CL2/net2-cl2.msx    Net2-CL2/net2-cl2.rpt    Net2-CL2/net2-cl2.bwq
$vcexe  Net2-CL2/net2-cl2.inp Net2-CL2/net2-cl2-vc.msx Net2-CL2/net2-cl2-vc.rpt Net2-CL2/net2-cl2-vc.bwq
$cygexe Net2-CL2/net2-cl2.inp Net2-CL2/net2-cl2-gc.msx Net2-CL2/net2-cl2-gc.rpt Net2-CL2/net2-cl2-gc.bwq
echo > > > Differences VC compiler < < <
diff --text --side-by-side Net2-CL2/net2-cl2.rpt Net2-CL2/net2-cl2-vc.rpt
echo > > > Differences GC compiler < < <
diff --text --side-by-side Net2-CL2/net2-cl2.rpt Net2-CL2/net2-cl2-gc.rpt

# Biofilm example
echo + + + Executing Biofilm Example + + +
$vcexe  Net3-Bio/net3-bio.inp Net3-Bio/net3-bio.msx    Net3-Bio/net3-bio.rpt    Net3-Bio/net3-bio.bwq
$vcexe  Net3-Bio/net3-bio.inp Net3-Bio/net3-bio-vc.msx Net3-Bio/net3-bio-vc.rpt Net3-Bio/net3-bio-vc.bwq
$cygexe Net3-Bio/net3-bio.inp Net3-Bio/net3-bio-gc.msx Net3-Bio/net3-bio-gc.rpt Net3-Bio/net3-bio-gc.bwq
echo > > > Differences VC compiler < < <
diff --text --side-by-side Net3-Bio/net3-bio.rpt Net3-Bio/net3-bio-vc.rpt
echo > > > Differences GC compiler < < <
diff --text --side-by-side Net3-Bio/net3-bio.rpt Net3-Bio/net3-bio-gc.rpt

# NH2CL example
echo + + + Executing NH2CL Example + + +
$vcexe  Net3-NH2CL/Net3-NH2CL.inp Net3-NH2CL/Net3-NH2CL.msx    Net3-NH2CL/Net3-NH2CL.rpt    Net3-NH2CL/Net3-NH2CL.bwq
$vcexe  Net3-NH2CL/Net3-NH2CL.inp Net3-NH2CL/Net3-NH2CL-vc.msx Net3-NH2CL/Net3-NH2CL-vc.rpt Net3-NH2CL/Net3-NH2CL-vc.bwq
$cygexe Net3-NH2CL/Net3-NH2CL.inp Net3-NH2CL/Net3-NH2CL-gc.msx Net3-NH2CL/Net3-NH2CL-gc.rpt Net3-NH2CL/Net3-NH2CL-gc.bwq
echo > > > Differences VC compiler < < <
diff --text --side-by-side Net3-NH2CL/Net3-NH2CL.rpt Net3-NH2CL/Net3-NH2CL-vc.rpt
echo > > > Differences GC compiler < < <
diff --text --side-by-side Net3-NH2CL/Net3-NH2CL.rpt Net3-NH2CL/Net3-NH2CL-gc.rpt

