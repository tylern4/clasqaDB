// groovy script to probe a qaTree.json file
// - run with no arguments to print help

if(args.size()==0) {
  println("\n\nARGUMENTS: jsonfile [runnum] [filenum] [object]")
  println("""
  pretty prints part or all of a qaTree.json file
  - specify a run number to restrict the printout to a single run
  - further specify a file number to restrict printout to a file
  - it's possible to pass additional arguments to further restrict printout
  """)
  return
}

import groovy.json.JsonSlurper
import groovy.json.JsonOutput

def slurper = new JsonSlurper()
def jsonFile = new File(args[0])
def jsonObj = slurper.parse(jsonFile)

def jAccess ( tree,path ) {
  if(path.size()<1) tree
  else if(path.size()==1) jAccess(tree[path[0]],[])
  else jAccess(tree[path[0]],path[1..-1])
}

def accessPath = []
args.length.times { if(it>=1) accessPath << args[it] }
println JsonOutput.prettyPrint(JsonOutput.toJson(jAccess(jsonObj,accessPath)))
