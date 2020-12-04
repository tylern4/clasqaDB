#include <iostream>
#include <bitset>

// include QADB header
#include "QADB.h"

using namespace std;

int main(int, char*[]) {

  // instantiate QADB
  QADB * qa = new QADB(5160,5170,true);

  // fake event loop, for demonstration
  int runnum = 5160;
  for(int evnum=1; evnum<75000000; evnum+=100000) {

    // perform the lookup, which finds the file assocated with this event
    if(qa->OkForAsymmetry(runnum,evnum)) {

      /* continue your analysis */
    
    }
    //else printf("omit event %d\n",evnum);
    else printf("omit file %d\n",qa->GetFilenum());
  };

  return 0;
}
