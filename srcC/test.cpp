#include <iostream>
#include <bitset>
#include "QADB.h"

using namespace std;

int main(int, char*[]) {

  // TODO: read concatenation qaTree.json files
  QADB * qa = new QADB("../qa.inbending1/qaTree.json");

  int runnum = 5194;
  for(int evnum=1; evnum<2700000; evnum+=1000) {
    if(qa->Query(runnum,evnum)) {
      cout << event << " event " << evnum << endl;
      cout << " filenum = " << qa->GetFilenum() << endl;
      cout << " evnum range = " << qa->GetEvnumMin() << " - " <<
                                   qa->GetEvnumMax() << endl;
      cout << " defect = " << qa->GetDefect() << " = 0b" <<
        bitset<6>(qa->GetDefect()) << endl;
      for(int s=1; s<=6; s++) 
        cout << "  sector " << s << " defect = " <<
          qa->GetDefectForSector(s) << " = 0b" <<
          bitset<6>(qa->GetDefectForSector(s)) << endl;
      cout << " comment = \"" << qa->GetComment() << "\"" << endl;
    };
  };

  return 0;
}
