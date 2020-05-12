#ifndef QADB_H_
#define QADB_H_

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <stdio.h>
#include <stdlib.h>

using namespace rapidjson;
using namespace std;

class QADB {
  public:

    QADB(const char * jsonFileName);

    bool Query(int runnum_, int evnum_);
    int GetFilenum() { return found ? filenum : -1; };
    int GetDefect() { return found ? defect : -1; };
    int GetDefectForSector(int sector_);
    string GetComment() { return found ? comment : ""; };

  private:
    FILE * jsonFile;
    FileReadStream * jsonStream;
    Document jsonDOM;
    char readBuffer[65536];

    char runnumStr[32];
    int runnum,filenum,evnumMin,evnumMax,evnumMinTmp,evnumMaxTmp;
    int defect;
    int sectorDefect[6];
    char sectorStr[8];
    string comment;

    bool found;
};



QADB::QADB(const char * jsonFileName) {
  jsonFile = fopen(jsonFileName,"r");
  jsonStream = new FileReadStream(jsonFile,readBuffer,sizeof(readBuffer));
  if(jsonDOM.ParseStream(*jsonStream).HasParseError()) {
    fprintf(stderr,"ERROR: QADB could not parse %s\n",jsonFileName);
    return;
  };
  runnum = -1;
  filenum = -1;
  evnumMin = -1;
  evnumMax = -1;
  found = false;
};


bool QADB::Query(int runnum_, int evnum_) {

  if( runnum_ != runnum ||
      ( runnum_ == runnum && (evnum_ < evnumMin || evnum_ > evnumMax ))) {
    runnum = runnum_;
    sprintf(runnumStr,"%d",runnum);
    assert(jsonDOM.HasMember(runnumStr));
    auto runDOM = jsonDOM[runnumStr].GetObject();
    filenum = -1;
    evnumMin = -1;
    evnumMax = -1;
    found = false;

    for( Value::MemberIterator it = runDOM.MemberBegin(); 
      it != runDOM.MemberEnd(); ++it ) {
      auto fileDOM = (it->value).GetObject();
      evnumMinTmp = fileDOM["evnumMin"].GetInt();
      evnumMaxTmp = fileDOM["evnumMax"].GetInt();
      if( evnum_ >= evnumMinTmp && evnum_ <= evnumMaxTmp ) {
        filenum = atoi((it->name).GetString());
        evnumMin = evnumMinTmp;
        evnumMax = evnumMaxTmp;
        comment = fileDOM["comment"].GetString();
        defect = fileDOM["defect"].GetInt();
        auto sectorDOM = fileDOM["sectorDefects"].GetObject();
        for(int s=0; s<6; s++) {
          sprintf(sectorStr,"%d",s+1);
          const Value& defList = sectorDOM[sectorStr];
          sectorDefect[s] = 0;
          for(SizeType i=0; i<defList.Size(); i++) {
            sectorDefect[s] += 0x1 << defList[i].GetInt();
          };
        };

        found = true;
        break;
      };
    };
  };

  if(!found) {
    fprintf(stderr,
      "WARNING: QADB::Query could not find run %d event %d\n",
      runnum_,evnum_);
  };

  return found;
};


int QADB::GetDefectForSector(int sector_) {
  if(sector_>=1 && sector_<=6) return found ? sectorDefect[sector_-1] : -1;
  else {
    fprintf(stderr,"ERROR: bad sector number for QADB::GetDefectForSector\n");
    return -1;
  };
};

#endif
