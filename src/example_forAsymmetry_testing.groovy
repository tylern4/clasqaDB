// NOTE: this script has a simulated event loop; see 
// `example_forAsymmetry.groovy` for a better example reading a HIPO file
// - this testing script is for crosschecking with the C++ implementation

import clasqa.QADB


// setup QA database
QADB qa = new QADB()

// event loop
def evCount = 0
int evnum
def runnum = 5160

// fake event loop, for demonstration
for(evnum=1; evnum<75000000; evnum+=100000) {
  if(evCount>0 && evCount%100000==0) println "read $evCount events"

  // QA cut for spin asymmetries
  if(qa.OkForAsymmetry(runnum,evnum)) {

    /* continue your analysis */

  }
  //else println "omit event $evnum"
  else println "omit file " + qa.getFilenum()

  evCount++
}
