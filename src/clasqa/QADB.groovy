package clasqa

import groovy.json.JsonSlurper
import groovy.json.JsonOutput
import clasqa.Tools

class QADB {

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

  public boolean Query...

  private boolean verbose
  private def dbDirN
  private def qaTree
  private def slurper
  private Tools T

  private def runnum,filenum,evnumMin,evnumMax,evnumMinTmp,evnumMaxTmp
  private def defect
  private def setorDefect
  private def sectorStr
  private def comment
  private boolean found
}


