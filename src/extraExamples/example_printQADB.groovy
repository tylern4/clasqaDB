// prints the qaTree and chargeTree in a human readable format
// - recommended to redirect output to a file

// define QADB object
// - specify run range to restrict QADB
// - if you do not specify a run range, it will by default
//   read the entire QADB
import clasqa.QADB
QADB qa = new QADB(5681,5684,true)
 
// closure to print the tree
import groovy.json.JsonOutput
def prettyPrint = { str -> 
  JsonOutput.prettyPrint(JsonOutput.toJson(str))
}

// print trees
println "================= qaTree ==============="
println prettyPrint(qa.getQaTree())
println "================= chargeTree ==============="
println prettyPrint(qa.getChargeTree())
