// highlights where we have a QADB syncing problem:
// eventnumMax of DST file N is larger than eventnumMin of DST file N+1
// (more correctly, of N+5, since these are 5-files)


// open QADB
import clasqa.QADB
QADB qa = new QADB()


def printSep

// loop through qaTree runs (sorted by run number)
qa.getQaTree().sort{a,b -> 
  a.key.toInteger() <=> b.key.toInteger()
}.each{ runnum,runTree ->
  printSep = false

  // loop through run's files (sorted by file number)
  runTree.sort{c,d ->
    c.key.toInteger() <=> d.key.toInteger()
  }.each { filenum,fileTree ->
    def evnumMax = fileTree['evnumMax']
    def filenumNxt = filenum.toInteger() + 5
    if(runTree["$filenumNxt"]!=null) {
      def fileTreeNxt = runTree["$filenumNxt"]
      def evnumMinNxt = fileTreeNxt['evnumMin']
      if( evnumMax > evnumMinNxt) {
        def overlap = evnumMax - evnumMinNxt
        println "SYNC ERROR: runnum=$runnum"
        println "  file $filenum evnumMax = $evnumMax"
        println "  file $filenumNxt evnumMin = $evnumMinNxt"
        println "  overlap = $overlap"
        printSep = true
      }
    }
  }
  if(printSep) println "=================================="
}
