// this script shows how to check if an event is part of a "golden file", 
// which means the file passes all QA criteria

// instantiate QADB
import clasqa.QADB
QADB qa = new QADB()

// set run number
int runnum = 5036

// event loop
for(int evnum=1; evnum<42000000; evnum+=1000000) {
  if(qa.golden(runnum,evnum))
    println "run $runnum event $evnum is in a golden file"
  else
    println "run $runnum event $evnum is in a file that has a defect"
}
