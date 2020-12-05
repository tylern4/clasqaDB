#include <iostream>

// Clas12Tool headers
#include "reader.h"
#include "clas12reader.h"

// QADB header
#include "QADB.h"


using namespace clas12;
using namespace std;


int main(int argc, char** argv) {

  // read list of hipo files
  string infileN;
  if(argc<=1) {
    cerr << "USAGE: " << argv[0] << " [hipo file1]" << endl;
    exit(0);
  };


  // instantiate clas12reader object
  clas12reader * c12 = new clas12reader(string(argv[1]));


  // instantiate QADB
  QADB * qa = new QADB();
  // alternatively, specify run range to restrict QADB (may be more efficient)
  //QADB * qa = new QADB(5000,5500);


  // define variables
  int runnum,evnum;


  // event loop
  cout << "begin event loop..." << endl;
  while(c12->next()==true) {

    cout << runnum << " " << evnum << endl;

    // limiter (for quick testing)
    if(evnum>1e5) { cout << "STOP" << endl; break; };

    // get run number and event number
    runnum = c12->runconfig()->getRun();
    evnum = c12->runconfig()->getEvent();


    // QA cuts
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

  cout << "total accumulated charge analyzed: " << 
    qa->GetAccumulatedCharge() << " nC" << endl;

  //printf("%d %f\n",runnum,qa->GetAccumulatedCharge()); // for cross check

};
