// calculate total charge from FC, with QA cuts enabled
// 
// NOTE: also highlights where we have a QADB syncing problem:
// eventnumMax of DST file N is larger than eventnumMin of DST file N+1
// (more correctly, of N+5, since these are 5-files)


// open QADB
import clasqa.QADB
def dbDirN = System.getenv('QADB')
if(dbDirN==null) {
  System.err << "ERROR: env var QADB not set; source env.sh\n\n\n"
  return
}
QADB qa = new QADB()


// open `data_table.dat` file, from `clasqa`
def inDatFile
if(args.length>=1) inDatFile = args[0]
else { System.err << "ERROR: specify data_table.dat file\n"; return; }


// open `data_table.dat`
def dataFile = new File(inDatFile)
def tok
def r
def runnum, filenum, eventNumMin, eventNumMax, sector
def nElec, nElecFT
def fcStart, fcStop
def ufcStart, ufcStop
if(!(dataFile.exists())) throw new Exception("$inDatFile not found")


// loop through `data_table.dat`, summing the charge
def fcCharge = 0
def ufcCharge = 0
dataFile.eachLine { line ->

  // read columns of data_table.dat (in order left-to-right)
  tok = line.tokenize(' ')
  r=0
  runnum = tok[r++].toInteger()
  filenum = tok[r++].toInteger()
  eventNumMin = tok[r++].toInteger()
  eventNumMax = tok[r++].toInteger()
  sector = tok[r++].toInteger()
  nElec = tok[r++].toBigDecimal()
  nElecFT = tok[r++].toBigDecimal()
  fcStart = tok[r++].toBigDecimal()
  fcStop = tok[r++].toBigDecimal()
  ufcStart = tok[r++].toBigDecimal()
  ufcStop = tok[r++].toBigDecimal()

  // FC is the same for all calorimeter sectors; pick one here
  if(sector==1) {

    // QA CUT
    // this does a lookup of the DST file associated to the event number, which
    // is redundant because we already know the DST file number here, but it
    // allows us to just use the existing QADB::query accessor
    if(qa.OkForAsymmetry(runnum,eventNumMax)) {
      fcCharge += fcStop - fcStart
      ufcCharge += ufcStop - ufcStart
      if(filenum!=qa.filenum) {
        System.err << "SERIOUS ERROR: QADB out-of-sync: runnum=" << 
          runnum << 
          " filenum=" << filenum << 
          " qa.filenum=" << qa.filenum << "\n"
      }
    }
  }
}

// convert nC -> mC
fcCharge /= 1e6
ufcCharge /= 1e6

println "total DAQ-gated FC charge: $fcCharge mC"
println "total ungated FC charge:   $ufcCharge mC"
