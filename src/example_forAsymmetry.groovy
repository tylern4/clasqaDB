// This is the standard usage example of QA cuts
// - reads a hipo (skim) file, and evaluates QA cuts in an event loop
// - you must specify a hipo file as an argument


// open hipo file reader
import org.jlab.io.hipo.HipoDataSource
def inHipoFile
if(args.length>=1) inHipoFile = args[0]
else { System.err << "ERROR: specify hipo file\n"; return; }
def reader = new HipoDataSource()
reader.open(inHipoFile)

// setup QA database
import clasqa.QADB
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
    else println "omit file " + qa.getFilenum()
  }

  evCount++
}
