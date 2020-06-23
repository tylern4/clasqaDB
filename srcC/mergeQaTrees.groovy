// concatenate ../qa.*/qaTree.json files into one file, ./qaTree.merged.json

import groovy.json.JsonSlurper
import groovy.json.JsonOutput

def qaTree = [:]
def slurper = new JsonSlurper()
def dbDir = new File('..')
def dbFilter = ~/^qaTree.json$/
dbDir.traverse( 
  type:groovy.io.FileType.FILES,
  maxDepth:1,
  nameFilter:dbFilter
) { dbFile ->
  println "read " + dbFile.getAbsoluteFile()
  slurper.parse(dbFile).each{ obj -> qaTree << obj }
}
def outfile = 'qaTree.merged.json'
new File(outfile).write(JsonOutput.toJson(qaTree))
println "produced merged file $outfile"
