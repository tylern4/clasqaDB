package clasqa

import groovy.json.JsonSlurper
import groovy.json.JsonOutput
import clasqa.Tools

class QADB {

  //..............
  // constructor
  //``````````````
  public QADB(boolean verbose_=false) {

    // setup
    verbose = verbose_
    T = new Tools()
    dbDirN = System.getenv('QADB')
    if(dbDirN==null) {
      System.err << "ERROR: env var QADB not set; source env.sh\n"
      return
    }
    if(verbose) println("QADB dir = $dbDirN")

    // concatenate qaTree.json files into one tree
    qaTree = [:]
    slurper = new JsonSlurper()
    def dbDir = new File(dbDirN)
    def dbFilter = ~/^qaTree.json$/
    dbDir.traverse( 
      type:groovy.io.FileType.FILES,
      maxDepth:1,
      nameFilter:dbFilter) { dbFile ->
      if(verbose) println "read " + dbFile.getAbsoluteFile()
      slurper.parse(dbFile).each{ obj -> qaTree << obj }
    }
    //if(verbose) println T.pPrint(qaTree) // pretty print qa tree

    // initialize local vars
    runnum = -1
    filenum = -1
    evnumMin = -1
    evnumMax = -1
    found = false
  }


  //..............
  // accessors
  //``````````````
  public int getFilenum() { return found ? filenum : -1 }
  public int getEvnumMin() { return found ? evnumMin : -1 }
  public int getEvnumMax() { return found ? evnumMax : -1 }
  public int getDefect() { return found ? defect : -1 }
  public int getDefectForSector(int sector) {
    return found ? sectorDefects[sector] : -1
  }
  public String getComment() { return found ? comment : "" }


  //..............
  // query qaTree to get QA info for this run number and event number
  // - a lookup is only performed if necessary: if the run number changes
  //   or if the event number goes outside of the range of the file which
  //   most recently queried
  //``````````````
  public boolean Query(int runnum_, int evnum_) {

    // perform lookup, only if needed
    if( runnum_ != runnum ||
        ( runnum_ == runnum && (evnum_ < evnumMin || evnum_ > evnumMax ))
    ) {
      runnum = runnum_
      filenum = -1
      evnumMin = -1
      evnumMax = -1
      found = false

      // search for file which contains this event
      qaTree[runnum].each{ f, qaFile ->
        evnumMinTmp = qaFile['evnumMin']
        evnumMaxTmp = qaFile['evnumMax']

        // found matching file, set all variables and stop the search
        if(evnum_>=evnumMinTmp && evnum_<=evnumMaxTmp) {
          filenum = f
          evnumMin = evnumMinTmp
          evnumMax = evnumMaxTmp
          comment = qaFile['comment']
          defect = qaFile['defect']
          sectorDefects = [:]
          qaFile['sectorDefects'].each{ sec,defs ->
            sectorDefects[sec] = 0
            defs.each{ sectorDefects[sec] += 0x1 << it }
          }
          found = true
          return true
        }
      }

      // print a warning if a file was not found for this event
      if(!found) {
        System.err << "WARNING: QADB::Query could not find " <<
        "runnum=$runnum_ evnum=$evnum_
      }
    }

    // lookup was not needed, return current value of found
    return found
  }

  private boolean verbose
  private def dbDirN
  private def qaTree
  private def slurper
  private Tools T

  private def runnum,filenum,evnumMin,evnumMax,evnumMinTmp,evnumMaxTmp
  private def defect
  private def sectorDefects
  private def comment
  private boolean found
}
