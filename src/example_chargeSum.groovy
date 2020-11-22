// calculate total analyzed charge for an example event loop
// with QA cuts enabled
// - you must specify a hipo file as an argument

// open hipo file reader; the hipo file must be specified as the argument
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


    // QA cut: standard cut for spin asymmetry analysis
    if(qa.OkForAsymmetry(runnum,evnum)) {

      // accumulate charge; note that although the call to
      // QADB::AccumulateCharge() charge happens for each
      // event within a DST file that passed the QA cuts, that
      // file's charge will only be accumulated once, so
      // overcounting is not possible 
      qa.AccumulateCharge()

      /* continue your analysis here */
    }

  }
  evCount++
}

// after the event loop, print the total accumulated charge
// that was analyzed
println "total accumulated charge analyzed: " + 
  qa.getAccumulatedCharge() + " nC"
