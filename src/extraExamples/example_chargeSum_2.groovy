// calculate total analyzed charge for an example event loop
// with QA cuts enabled
// NOTE: this script has a simulated event loop; see 
// `example_chargeSum.groovy` for a better example reading a HIPO file


// setup QA database
import clasqa.QADB
QADB qa = new QADB()

// fake event loop, for demonstration
def runnum = 5717
for(int evnum=1; evnum<100011440; evnum+=1000) {

  // QA cut
  if(qa.OkForAsymmetry(runnum,evnum)) {

    // accumulate charge; note that although the call to
    // QADB::accumulateCharge() charge happens for each
    // event within a DST file that passed the QA cuts, that
    // file's charge will only be accumulated once, so
    // overcounting is not possible 
    qa.accumulateCharge()

    /* continue your analysis here */

  }
}

// after the event loop, print the total accumulated charge
// that was analyzed
println "total accumulated charge analyzed: " + 
  qa.getAccumulatedCharge() + " nC"
