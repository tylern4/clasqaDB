// this example demonstrates how to loop through events and check whether
// they have the specified QA defect bits set
// - use the QADB::setMaskBit function to decide which defects are of concern


// instantiate QADB
import clasqa.QADB
QADB qa = new QADB()

// set run number
int runnum = 5036

// decide which defects you want to check for; an event will not pass
// the QA cut if the associated file has any of the specified defects
// - set to true to check the bit
// - set to false to ignore the bit (by default, all bits are ignored,
//   so there's no need to set any to false; it's done here to show
//   as an example)
qa.setMaskBit('TotalOutlier',false)
qa.setMaskBit('TerminalOutlier',false)
qa.setMaskBit('MarginalOutlier',false)
qa.setMaskBit('SectorLoss',true) // this is the only bit we check here
qa.setMaskBit('LowLiveTime',false)
qa.setMaskBit('Misc',false)

// print the defect bit mask
println "\nmask = " + qa.util.printBinary(qa.getMask()) + "\n"

// event loop
for(int evnum=1; evnum<42000000; evnum+=1000000) {
  print "run $runnum event $evnum "

  // print whether or not the file associated with this event has any of the
  // defects specified above via QADB::setMaskBit
  print qa.pass(runnum,evnum) ? "passed QA" : "failed QA"

  // print whether or not the file is "golden" (has no defects at all)
  println " (file " + qa.getFilenum() +
    (qa.golden(runnum,evnum) ? ", GOLDEN file)" : ", not a golden file)")
}
