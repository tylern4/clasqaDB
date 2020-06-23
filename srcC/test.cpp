#include <iostream>
#include <bitset>
#include "QADB.h"

using namespace std;

int main(int, char*[]) {

  QADB * qa = new QADB("./qaTree.merged.json");

  // fake event loop
  int runnum = 5160;
  for(int evnum=1; evnum<79000000; evnum+=400000) {

    // perform the lookup, which finds the file assocated with this event
    if(qa->Query(runnum,evnum)) {

      // print run number and file number
      cout << "run " << runnum << " event " << evnum << endl;
      cout << " filenum = " << qa->GetFilenum() << endl;

      // print whether or not this is a golden file
      if(qa->Golden()) cout << " --> GOLDEN FILE!" << endl;
      else cout <<  " --> file has defects" << endl;

      // print event number range
      cout << " evnum range = " << qa->GetEvnumMin() << " - " <<
                                   qa->GetEvnumMax() << endl;
                                   
      // print defect bits
      cout << " defect = " << qa->GetDefect() << " = 0b" <<
        bitset<6>(qa->GetDefect()) << endl;

      // accessing each defect bit (see below for doing this for a particular sector)
      if(qa->HasDefect("TotalOutlier"))    cout << "   - TotalOutlier defect" << endl;
      if(qa->HasDefect("TerminalOutlier")) cout << "   - TerminalOutlier defect" << endl;
      if(qa->HasDefect("MarginalOutlier")) cout << "   - MarginalOutlier defect" << endl;
      if(qa->HasDefect("SectorLoss"))      cout << "   - SectorLoss defect" << endl;
      if(qa->HasDefect("LowLiveTime"))     cout << "   - LowLiveTime defect" << endl;
      if(qa->HasDefect("Misc"))            cout << "   - Misc defect" << endl;

      // print defect bits for each sector
      for(int s=1; s<=6; s++) {
        cout << "  sector " << s << " defect = " <<
          qa->GetDefectForSector(s) << " = 0b" <<
          bitset<6>(qa->GetDefectForSector(s)) << endl;
      };

      // if there is a sector loss, print which sectors
      for(int s=1; s<=6; s++) {
        if(qa->HasDefect("SectorLoss",s)) {
          cout << "    -> sector " << s << " loss" << endl;
        };
      };

      // print comment
      cout << " comment = \"" << qa->GetComment() << "\"\n" << endl;

    };
  };

  return 0;
}
