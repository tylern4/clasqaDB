// demonstrates how to select events that pass the set
// of QA criteria specified by the user
// - you must specify a hipo file as an argument

#include <iostream>
#include <bitset>

// clas12root headers
#include "reader.h"
#include "clas12reader.h"

// QADB header and namespace
#include "QADB.h"
using namespace QA;

using namespace clas12; // for clas12root
using namespace std;

int main(int argc, char** argv) {

  // instantiate clas12reader object for specified hipo file
  string infileN;
  if(argc<=1) {
    cerr << "USAGE: " << argv[0] << " [hipo file]" << endl;
    exit(0);
  };
  clas12reader * c12 = new clas12reader(string(argv[1]));


  // instantiate QADB
  QADB * qa = new QADB();
  // alternatively, specify run range to restrict QADB (may be more efficient)
  //QADB * qa = new QADB(5000,5500);

  // custom QA cut definition
  // - decide which defects you want to check for; an event will not pass
  //   the QA cut if the associated file has any of the specified defects
  // - set to true to check the bit
  // - set to false to ignore the bit (by default, all bits are ignored)
  qa->SetMaskBit("TotalOutlier",false);
  qa->SetMaskBit("TerminalOutlier",false);
  qa->SetMaskBit("MarginalOutlier",false);
  qa->SetMaskBit("SectorLoss",true); // this is the only bit we check here
  qa->SetMaskBit("LowLiveTime",false);
  qa->SetMaskBit("Misc",false);

  // print the defect bit mask
  cout << "\ndefect mask = 0b" << bitset<6>(qa->GetMask()) << endl;


  // define variables
  int runnum,evnum;
  int evCount = 0;
  int evCountOK = 0;


  // event loop
  cout << "begin event loop..." << endl;
  while(c12->next()==true) {
    if(evCount%10000==0) cout << evCount << " events analyzed" << endl;
    
    // truncate event loop (for quick testing)
    if(evCount>1e5) { cout << "event loop truncated!" << endl; break; };

    // get run number and event number
    runnum = c12->runconfig()->getRun();
    evnum = c12->runconfig()->getEvent();

    // QA cuts
    if(qa->Pass(runnum,evnum)) {
      evCountOK++;

      /* continue your analysis here */

    };

    // do not increment evCount for events with runnum==0, which fail QA
    if(runnum>0) evCount++;
  };

  // print fraction of events which pass QA cuts
  printf("\nrun = %d\n",runnum);
  printf("number of events analyzed = %d\n",evCount);
  printf("number of events which pass QA cuts = %d  (%f%%)\n",
    evCountOK,100.*(double)evCountOK/evCount);
};
