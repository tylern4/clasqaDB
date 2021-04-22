#include "Two.h"

ClassImp(Two)

Two::Two() {
  printf("Instantiating Two...\n");
  qa = new QADB();
  printf("done\n");
};

Two::~Two() {};
