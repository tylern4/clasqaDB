#ifndef Two_
#define Two_

#include <stdio.h>
#include <QADB.h>
#include <TObject.h>

using namespace QA;

class Two : public TObject
{
  public:
    Two();
    ~Two();
  private:
    QADB * qa;
  ClassDef(Two,1);
};

#endif
