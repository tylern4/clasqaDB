#ifndef One_
#define One_

#include <stdio.h>
#include <QADB.h>
#include <TObject.h>

using namespace QA;

class One : public TObject
{
  public:
    One();
    ~One();
  private:
    QADB * qa;
  ClassDef(One,1);
};

#endif
