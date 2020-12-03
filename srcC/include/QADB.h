#ifndef QADB_H_
#define QADB_H_

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <map>
#include <vector>

using namespace rapidjson;
using namespace std;

class QADB {
  public:

    //.................
    // constructor
    //`````````````````
    //arguments:
    // - runnumMin and runnumMax: if both are negative (default), then the
    //   entire QADB will be read; you can restrict to a specific range of
    //   runs to limit QADB, which may be more effecient
    // - verbose: if true, print (a lot) more information
    QADB(int runnumMin_=-1, int runnumMax=-1, bool verbose_=false);



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


    int runnumMin, runnumMax;
    bool verbose = true;
    string dbDirN;
    vector<string> qaJsonList;
    vector<string> chargeJsonList;

    FILE * jsonFile;
    Document qaTree, chargeTree, tmpTree;
    char readBuffer[65536];
    void chainTrees(vector<string> jsonList, Document * treePtr);

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
QADB::QADB(int runnumMin_, int runnumMax_, bool verbose_) {

  runnumMin = runnumMin_;
  runnumMax = runnumMax_;
  verbose = verbose_;

  // get QADB directory
  if(verbose) cout << "\n[+] find json files" << endl;
  dbDirN = getenv("QADB") ? getenv("QADB") : "";
  if(dbDirN.compare("")==0) {
    cerr << "ERROR: QADB environment variable not set" << endl;
    return;
  };
  dbDirN += "/qadb";
  if(verbose) cout << "QADB at " << dbDirN << endl;

  // get list of json files
  DIR * dbDir = opendir(dbDirN.c_str());
  struct dirent * dbDirent;
  while((dbDirent=readdir(dbDir))) {
    string qaDirN = string(dbDirent->d_name);
    if(qaDirN.find("qa.")!=string::npos) {
      qaJsonList.push_back(dbDirN+"/"+qaDirN+"/qaTree.json");
      chargeJsonList.push_back(dbDirN+"/"+qaDirN+"/chargeTree.json");
    };
  };
  closedir(dbDir);
  if(verbose) {
    cout << "qaTree files:" << endl;
    for(string str : qaJsonList) cout << " - " << str << endl;
    cout << "chargeTree files:" << endl;
    for(string str : chargeJsonList) cout << " - " << str << endl;
  };


  // read json files and concatenate, including only runs within specified
  // range [runnumMin,runnumMax]
  if(verbose) cout << "\n[+] read specified runs from json files" << endl;
  this->chainTrees(qaJsonList,&qaTree);
  this->chainTrees(chargeJsonList,&chargeTree);


  return;


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


// concatenate trees from json files in jsonList to tree at treePtr
void QADB::chainTrees(vector<string> jsonList, Document * treePtr) {

  // loop through list of json files
  for(string jsonFileN : jsonList) {

    // open json file stream
    if(verbose) cout << "read json stream " << jsonFileN << endl;
    jsonFile = fopen(jsonFileN.c_str(),"r");
    FileReadStream * jsonStream = new FileReadStream(
      jsonFile,readBuffer,sizeof(readBuffer)
    );
    
    // parse stream to tmpTree
    if(tmpTree.ParseStream(*jsonStream).HasParseError()) {
      cerr << "ERROR: QADB could not parse " << jsonFileN << endl;
      return;
    };

    // append to tmpTree to treePtr
    bool once = true;
    for( auto it = tmpTree.MemberBegin(); it != tmpTree.MemberEnd(); ++it ) {
      runnum = atoi((it->name).GetString());
      if( ( runnumMin<0 && runnumMax<0) ||
          ( runnum>=runnumMin && runnum<=runnumMax))
      {
        if(verbose) cout << "- add run " << runnum << endl;
        if(once) treePtr->CopyFrom(tmpTree,treePtr->GetAllocator());
        else treePtr->AddMember(it->name,it->value,treePtr->GetAllocator());
        once = false;
      };
    };

    // close json file
    fclose(jsonFile);
    if(jsonStream) delete jsonStream;
  };

  // reset runnum
  runnum = -1;
};


bool QADB::Query(int runnum_, int evnum_) {

  // if the run number changed, or if the event number is outside the range
  // of the previously queried file, perform a new lookup
  if( runnum_ != runnum ||
      ( runnum_ == runnum && (evnum_ < evnumMin || evnum_ > evnumMax ))) {
    runnum = runnum_;
    sprintf(runnumStr,"%d",runnum);
    assert(qaTree.HasMember(runnumStr));
    auto runTree = qaTree[runnumStr].GetObject();
    filenum = -1;
    evnumMin = -1;
    evnumMax = -1;
    found = false;

    for( auto it = runTree.MemberBegin(); 
      it != runTree.MemberEnd(); ++it ) {
      auto fileTree = (it->value).GetObject();
      evnumMinTmp = fileTree["evnumMin"].GetInt();
      evnumMaxTmp = fileTree["evnumMax"].GetInt();
      if( evnum_ >= evnumMinTmp && evnum_ <= evnumMaxTmp ) {
        filenum = atoi((it->name).GetString());
        evnumMin = evnumMinTmp;
        evnumMax = evnumMaxTmp;
        comment = fileTree["comment"].GetString();
        defect = fileTree["defect"].GetInt();
        auto sectorTree = fileTree["sectorDefects"].GetObject();
        for(int s=0; s<6; s++) {
          sprintf(sectorStr,"%d",s+1);
          const Value& defList = sectorTree[sectorStr];
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
