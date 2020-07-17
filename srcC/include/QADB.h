#ifndef QADB_H_
#define QADB_H_

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>

using namespace rapidjson;
using namespace std;

class QADB {
  public:

    QADB(const char * jsonFileName);

    // use this method for a spin asymmetry analysis
    bool OkForAsymmetry(int runnum_, int evnum_);

    // perform lookup (only as necessary); returns true if the file
    // associated with the event is found; this must be called *before*
    // using any other accessor
    bool Query(int runnum_, int evnum_);


    // accessors
    int GetFilenum() { return found ? filenum : -1; };
    int GetEvnumMin() { return found ? evnumMin : -1; };
    int GetEvnumMax() { return found ? evnumMax : -1; };
    int GetDefect() { return found ? defect : -1; };
    int GetDefectForSector(int sector_);
    string GetComment() { return found ? comment : ""; };
    bool Golden() { return found ? defect==0 : false; };
    bool HasDefect(const char * defectName, int sector);

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

    map<string,int> defectNameMap;

    bool found;
    int asymMask;
};



// constructor
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
  
  defectNameMap.insert(pair<string,int>("TotalOutlier",0));
  defectNameMap.insert(pair<string,int>("TerminalOutlier",1));
  defectNameMap.insert(pair<string,int>("MarginalOutlier",2));
  defectNameMap.insert(pair<string,int>("SectorLoss",3));
  defectNameMap.insert(pair<string,int>("LowLiveTime",4));
  defectNameMap.insert(pair<string,int>("Misc",5));

  // defect mask used for asymmetry analysis
  asymMask = 0;
  asymMask += 0x1 << 0; // TotalOutlier
  asymMask += 0x1 << 1; // TerminalOutlier
  asymMask += 0x1 << 2; // MarginalOutlier
  asymMask += 0x1 << 3; // SectorLoss
};


bool QADB::Query(int runnum_, int evnum_) {

  // if the run number changed, or if the event number is outside the range
  // of the previously queried file, perform a new lookup
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

  // if a file is not found, print a warning (this is suppressed for
  // tag1 events, where both runnum and evnum are 0)
  if(!found) {
    if(runnum_!=0) {
      fprintf(stderr,
        "WARNING: QADB::Query could not find run %d event %d\n",
        runnum_,evnum_);
    };
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


// return true if the file has the specified defect
// - optionally specify a sector if you just want to check one sector
bool QADB::HasDefect(const char * defectName, int sector=-1) {
  int defectBit;
  try { defectBit = defectNameMap.at(string(defectName)); }
  catch(const out_of_range & e) {
    fprintf(stderr,"ERROR: QADB::HasDefect does not understand defectName\n");
    return true;
  };
  if(sector>=1 && sector<=6) {
    return (this->GetDefectForSector(sector) >> defectBit) & 0x1;
  } else {
    return (this->GetDefect() >> defectBit) & 0x1;
  };
};


// QA for spin asymmetry analysis
// - if true, this event is good for a spin asymmetry analysis
bool QADB::OkForAsymmetry(int runnum_, int evnum_) {

  // perform lookup
  bool foundHere = this->Query(runnum_,evnum_);
  if(!foundHere) return false;

  // check for bits which will always cause the file to be rejected 
  // (asymMask is defined in the constructor)
  if( defect & asymMask ) return false;

  // special cases for `Misc` bit
  int miscBit = defectNameMap.at("Misc"); 
  if(this->HasDefect("Misc")) {

    // check if this is a run on the list of runs with a large fraction of
    // events with undefined helicity; if so, accept this run, since none of
    // these files are marked with `Misc` for any other reasons
    if(runnum_==5128 ||
       runnum_==5129 ||
       runnum_==5130 ||
       runnum_==5158 ||
       runnum_==5159 ||
       runnum_==5160 ||
       runnum_==5163 ||
       runnum_==5165 ||
       runnum_==5166 ||
       runnum_==5167 ||
       runnum_==5168 ||
       runnum_==5169 ||
       runnum_==5180 ||
       runnum_==5181 ||
       runnum_==5182 ||
       runnum_==5183 ||
       runnum_==5567) return true;
    else return false;
  };

  // otherwise, this file passes the QA
  return true;
}






#endif
