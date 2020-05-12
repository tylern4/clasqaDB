#include "QADB.h"

int main(int, char*[]) {
  QADB * qa = new QADB("../qa.inbending1/qaTree.json");

  int runnum = 5194;
  for(int evnum=1; evnum<2700000; evnum+=1) {
    if(qa->Query(runnum,evnum)) {
      printf("%d event %d\n",runnum,evnum);
      printf(" filenum = %d\n",qa->GetFilenum());
      printf(" defect = %d\n",qa->GetDefect());
      for(int s=1; s<=6; s++) 
        printf("  sector %d defect = %d\n",s,qa->GetDefectForSector(s));
      printf(" comment = \"%s\"\n",qa->GetComment().c_str());
    };
  };

  return 0;
}
