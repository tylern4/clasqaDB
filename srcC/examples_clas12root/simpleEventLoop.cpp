#include <iostream>

// Clas12Tool
#include "reader.h"
#include "bank.h"
#include "particle.h"
#include "clas12reader.h"

using namespace clas12;
using namespace std;

int main(int argc, char** argv) {

  // read list of hipo files
  string infileN;
  if(argc<=1) {
    cerr << "USAGE: " << argv[0] << " [hipo file1]" << endl;
    exit(0);
  };


  clas12reader * c12 = new clas12reader(string(argv[1]));

  // EVENT LOOP ----------------------------------------------
  cout << "begin event loop..." << endl;
  long int nTotal=0;
  int runnum,evnum;

  while(c12->next()==true) {
    if(nTotal>1e5) { 
      cout << "STOP LOOP at " << nTotal << " events" << endl;
      break;
    };

    runnum = c12->runconfig()->getRun();
    evnum = c12->runconfig()->getEvent();

    cout << runnum << " " << evnum << endl;

    nTotal++;

  }; // end event loop

};
