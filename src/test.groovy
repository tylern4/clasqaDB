import clasqa.QADB
import clasqa.Tools // only used for demonstration, to print binary numbers

QADB qa = new QADB(true)
Tools T = new Tools()

int runnum = 5194


for(int evnum=1; evnum<2700000; evnum+=100000) {
  if(qa.query(5194,evnum)) {
    println "event $evnum"
    println "  filenum = " + qa.getFilenum()
    println "  evnumRange = " + qa.getEvnumMin() + "-" + qa.getEvnumMax()
    println "  defect = " + qa.getDefect() + 
      " = " + T.printBinary(qa.getDefect())
    (1..6).each{ sector ->
      println "    sector $sector defect = " +
        qa.getDefectForSector(sector) +
        " = " + T.printBinary(qa.getDefectForSector(sector))
    }
    println "  comment = \"" + qa.getComment() + "\""
  }
}
