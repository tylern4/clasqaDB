#include "One.h"

ClassImp(One)

One::One() {
  printf("Instantiating One...\n");
  qa = new QADB();
  printf("done\n");
};

One::~One() {};
