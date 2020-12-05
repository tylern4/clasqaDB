// include QADB header
#include "QADB.h"

using namespace std;

int main() {

  // instantiate QADB
  QADB * qa = new QADB();
  //QADB * qa = new QADB(5700,5720,true);

  // fake event loop, for demonstration
  int runnum = 5717;
  for(int evnum=1; evnum<100011440; evnum+=1000) {

    // QA cut
    if(qa->OkForAsymmetry(runnum,evnum)) {

      // accumulate charge; note that although the call to
      // QADB::accumulateCharge() charge happens for each
      // event within a DST file that passed the QA cuts, that
      // file's charge will only be accumulated once, so
      // overcounting is not possible 
      qa->AccumulateCharge();

      /* continue your analysis here */
    
    };
  };

  // after the event loop, print the total accumulated charge
  // that was analyzed
  printf(
    "total accumulated charge analyzed: %f nC\n",
    qa->GetAccumulatedCharge()
  );


  return 0;
}
