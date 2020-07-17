import org.jlab.io.hipo.HipoDataSource
import clasqa.QADB

// open hipo file reader; the hipo file must be specified as the argument
def inHipoFile
if(args.length>=1) inHipoFile = args[0]
else { System.err << "ERROR: specify hipo file\n"; return; }
def reader = new HipoDataSource()
reader.open(inHipoFile)

// setup QA database
QADB qa = new QADB()

// event loop
def evCount = 0
def event
def runnum,evnum
while(reader.hasEvent()) {
  //if(evCount>10000) break // limit to 10000 events
  if(evCount % 100000 == 0) println "read $evCount events"

  event = reader.getNextEvent()
  if(event.hasBank("RUN::config") && event.hasBank("REC::Particle")) {

    // get run and event numbers
    runnum = event.getBank("RUN::config").getInt('run',0)
    evnum = event.getBank("RUN::config").getInt('event',0)

    // QA cut for spin asymmetries
    if(qa.OkForAsymmetry(runnum,evnum)) {

      /* continue your analysis */

    }
    //else println "omit event $evnum"
    else println "omit file " + qa.getFilenum()
  }

  evCount++
}
