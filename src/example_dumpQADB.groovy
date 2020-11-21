// instantiate QADB
import clasqa.QADB
QADB qa = new QADB()

// set run number
int runnum = 5160

// event loop
for(int evnum=1; evnum<75000000; evnum+=1000000) {

  /* query the database
  - this requires the event number, and it will find the file number
    associated to this event
  - QADB::query returns `true` if the file is found
  - the query is performed only if any of the following occur:
    - it's the first time
    - the run number changes
    - the event number is outside of the previously queried file's event 
      number range
  */

  if(qa.query(runnum,evnum)) {
    println "\nevent number $evnum"

    // print file number and range of event numbers associated to this file
    println "  filenum = " + qa.getFilenum()
    println "  evnumRange = " + qa.getEvnumMin() + " to " + qa.getEvnumMax()

    // call QADB::getDefect with no arguments to get the bitmask for
    // this file's defects
    println "  defect = " + qa.getDefect() + 
      " = " + qa.util.printBinary(qa.getDefect())

    // print a description for each defect bit associated for this file
    println "  defect bits:"
    (0..qa.nbits).each { bit ->
      if(qa.hasDefect(bit)) println "    " + qa.util.bitNames[bit]
    }

    // if there was a sector loss, print which sectors
    // (see clasqa/Tools.groovy for a list of the bit names)
    if(qa.hasDefectName('SectorLoss')) {
      print "  -> sector loss defect found for sectors "
      def sectorsLost = (1..6).findAll{ sector -> 
        qa.hasDefectName('SectorLoss',sector)
      }
      println sectorsLost
    }

    // call QADB::getDefect for a particular sector
    println "  defect for each sector:"
    (1..6).each{ sector ->
      println "    sector $sector defect = " +
        qa.getDefect(sector) +
        " = " + qa.util.printBinary(qa.getDefect(sector))
    }

    // print a comment associated with the QA result
    println "  comment = \"" + qa.getComment() + "\""
  }
}
