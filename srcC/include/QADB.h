#ifndef QADB_H_
#define QADB_H_

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <map>
#include <vector>
#include <algorithm>

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


    //...............................
    // golden QA cut
    //```````````````````````````````
    // returns false if the event is in a file with *any* defect
    bool Golden(int runnum_, int evnum_) { 
      bool foundHere = this->Query(runnum_,evnum_);
      return foundHere && defect==0;
    };


    //.....................................
    // QA for spin asymmetry analysis
    //`````````````````````````````````````
    // if true, this event is good for a spin asymmetry analysis
    bool OkForAsymmetry(int runnum_, int evnum_);


    //...............................
    // user-defined custom QA cuts
    //```````````````````````````````
    // first set which defect bits you want to filter out; by default
    // none are set; the variable `mask` will be applied as a mask
    // on the defect bits
    void SetMaskBit(const char * defectName, bool state=true);
    // access the custom mask, if you want to double-check it
    int GetMask() { return mask; };
    // then call this method to check your custom QA cut for a given
    // run number and event number
    bool Pass(int runnum_, int evnum_);


    //.................
    // accessors
    //`````````````````
    // --- access this file's info
    int GetFilenum() { return found ? filenum : -1; };
    string GetComment() { return found ? comment : ""; };
    int GetEvnumMin() { return found ? evnumMin : -1; };
    int GetEvnumMax() { return found ? evnumMax : -1; };
    double GetCharge() { return found ? charge : -1; };
    // --- access QA info
    // check if the file has a particular defect
    // - if sector==0, checks the OR of all the sectors
    // - if an error is thrown, return true so file will be flagged
    bool HasDefect(const char * defectName, int sector=0) {
      return HasDefectBit(Bit(defectName),sector);
    };
    // - alternatively, check for defect by bit number
    bool HasDefectBit(int defect_, int sector=0) {
      return (this->GetDefect(sector) >> defect_) & 0x1;
    };
    // get this file's defect bitmask;
    // - if sector==0, gets OR of all sectors' bitmasks
    int GetDefect(int sector=0);
    // translate defect name to defect bit
    int Bit(const char * defectName);


    //.................................................................
    // query qaTree to get QA info for this run number and event number
    // - a lookup is only performed if necessary: if the run number changes
    //   or if the event number goes outside of the range of the file which
    //   most recently queried
    // - this method is called automatically when evaluating QA cuts
    //`````````````````````````````````````````````````````````````````
    bool Query(int runnum_, int evnum_);


    //.................................
    // Faraday Cup charge
    //`````````````````````````````````
    // -- accumulator
    // call this method after evaluating QA cuts (or at least after calling Query())
    // to add the current file's charge to the total charge;
    // - charge is accumulated per DST file, since the QA filters per DST file
    // - a DST file's charge is only accounted for if we have not counted it before
    void AccumulateCharge();
    // -- accessor
    // returns total accumlated charge that passed your QA cuts; call this
    // method after your event loop
    double GetAccumulatedCharge() { return chargeTotal; };



  private:

    int runnumMin, runnumMax;
    bool verbose = true;
    vector<string> qaJsonList;
    vector<string> chargeJsonList;

    Document qaTree, chargeTree, tmpTree;
    char readBuffer[65536];
    void chainTrees(vector<string> jsonList, Document * treePtr);

    char runnumStr[32];
    char filenumStr[32];
    int runnum,filenum,evnumMin,evnumMax,evnumMinTmp,evnumMaxTmp;
    int defect;
    int sectorDefect[6];
    char sectorStr[8];
    string comment;
    double charge, chargeTotal;
    bool chargeCounted;
    vector<pair<int,int>> chargeCountedFiles;

    map<string,int> defectNameMap;
    int nbits;

    bool found;
    int asymMask;
    int mask;
};



//...............
// constructor
//```````````````
QADB::QADB(int runnumMin_, int runnumMax_, bool verbose_) {

  runnumMin = runnumMin_;
  runnumMax = runnumMax_;
  verbose = verbose_;

  // get QADB directory
  if(verbose) cout << "\n[+] find json files" << endl;
  string dbDirN = getenv("QADB") ? getenv("QADB") : "";
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

  // define bits (must match those in Tools.groovy, in order)
  nbits=0;
  defectNameMap.insert(pair<string,int>("TotalOutlier",nbits++));
  defectNameMap.insert(pair<string,int>("TerminalOutlier",nbits++));
  defectNameMap.insert(pair<string,int>("MarginalOutlier",nbits++));
  defectNameMap.insert(pair<string,int>("SectorLoss",nbits++));
  defectNameMap.insert(pair<string,int>("LowLiveTime",nbits++));
  defectNameMap.insert(pair<string,int>("Misc",nbits++));

  // 6 elements

  // defect mask used for asymmetry analysis
  asymMask = 0;
  asymMask += 0x1 << Bit("TotalOutlier");
  asymMask += 0x1 << Bit("TerminalOutlier");
  asymMask += 0x1 << Bit("MarginalOutlier");
  asymMask += 0x1 << Bit("SectorLoss");

  // initialize local vars
  runnum = -1;
  filenum = -1;
  evnumMin = -1;
  evnumMax = -1;
  found = false;
  mask = 0;
  charge = 0;
  chargeTotal = 0;
  chargeCounted = false;
};


//....................................................................
// concatenate trees from JSON files in jsonList to tree at treePtr
//````````````````````````````````````````````````````````````````````
// - includes only runs within specified range [runnumMin,runnumMax]
void QADB::chainTrees(vector<string> jsonList, Document * treePtr) {

  // loop through list of json files
  for(string jsonFileN : jsonList) {

    // open json file stream
    if(verbose) cout << "read json stream " << jsonFileN << endl;
    FILE * jsonFile = fopen(jsonFileN.c_str(),"r");
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
          ( runnum>=runnumMin && runnum<=runnumMax)
      ) {
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


//...................................
// QA for spin asymmetry analysis
//```````````````````````````````````
bool QADB::OkForAsymmetry(int runnum_, int evnum_) {

  // perform lookup
  bool foundHere = this->Query(runnum_,evnum_);
  if(!foundHere) return false;

  // check for bits which will always cause the file to be rejected 
  // (asymMask is defined in the constructor)
  if( defect & asymMask ) return false;

  // special cases for `Misc` bit
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
};


//...............................
// user-defined custom QA cuts
//```````````````````````````````
void QADB::SetMaskBit(const char * defectName, bool state) {
  int defectBit = this->Bit(defectName);
  if(defectBit<0 || defectBit>=nbits)
    cerr << "ERROR: QADB::SetMaskBit called for unknown bit" << endl;
  else {
    mask &= ~(0x1 << defectBit);
    if(state) mask |= (0x1 << defectBit);
  };
};
bool QADB::Pass(int runnum_, int evnum_) {
  bool foundHere = this->Query(runnum_,evnum_);
  return foundHere && !(defect & mask);
}




//.................
// accessors
//`````````````````
int QADB::GetDefect(int sector) {
  if(!found) return -1;
  if(sector==0) return defect;
  else if(sector>=1 && sector<=6) return sectorDefect[sector-1];
  else {
    cerr << "ERROR: bad sector number for QADB::GetDefect" << endl;
    return -1;
  };
};
int QADB::Bit(const char * defectName) {
  int defectBit;
  try { defectBit = defectNameMap.at(string(defectName)); }
  catch(const out_of_range & e) {
    cerr << "ERROR: QADB::Bit() unknown defectName" << endl;
    return -1;
  };
  return defectBit;
};
  



//.....................................................................
// query qaTree to get QA info for this run number and event number
// - a lookup is only performed if necessary: if the run number changes
//   or if the event number goes outside of the range of the file which
//   most recently queried
//`````````````````````````````````````````````````````````````````````
bool QADB::Query(int runnum_, int evnum_) {

  // if the run number changed, or if the event number is outside the range
  // of the previously queried file, perform a new lookup
  if( runnum_ != runnum ||
      ( runnum_ == runnum && (evnum_ < evnumMin || evnum_ > evnumMax ))
  ) {
    // reset vars
    runnum = runnum_;
    filenum = -1;
    evnumMin = -1;
    evnumMax = -1;
    charge = -1;
    found = false;


    // search for file which contains this event
    sprintf(runnumStr,"%d",runnum);
    if(qaTree.HasMember(runnumStr)) {
      auto runTree = qaTree[runnumStr].GetObject();

      for( 
        auto it = runTree.MemberBegin(); 
        it != runTree.MemberEnd();
        ++it
      ) {
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

          sprintf(filenumStr,"%d",filenum);
          charge = chargeTree[runnumStr][filenumStr]["fcCharge"].GetDouble();
          chargeCounted = false;

          found = true;
          break;
        };
      };
    };

    // print a warning if a file was not found for this event
    // - this warning is suppressed for 'tag1' events
    if(!found && runnum_!=0) {
      cerr << "WARNING: QADB::Query could not find runnum=" <<
        runnum_ << " evnum=" << evnum_ << endl;
    };
  };

  // result of query
  return found;
};



//.................................
// Faraday Cup charge accumulator
//`````````````````````````````````
void QADB::AccumulateCharge() {
  if(!chargeCounted) {
    if(
      find(
        chargeCountedFiles.begin(),
        chargeCountedFiles.end(),
        pair<int,int>(runnum,filenum)
      ) == chargeCountedFiles.end()
    ) {
      chargeTotal += charge;
      chargeCountedFiles.push_back(pair<int,int>(runnum,filenum));
    };
    chargeCounted = true;
  };
};


#endif
