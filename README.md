![QADB](/util/logo.png)

**CLAS12 Quality Assurance Database**
* provides storage of and access to the QA monitoring results for the 
  CLAS12 experiment at Jefferson Lab


## QA Database Access

* Text Access
  * this only provides human-readable access; see below for access with
    common programming languages and software used at CLAS
    * using the Groovy or C++ access is the preferred method to apply QA cuts
  * the human-readable tables are stored in `qadb/*/qaTree.json.table`; see
    the section *QA data storage, Table files* below for details for how
    to read these files
  * QADB JSON files are stored in `qadb/*/qaTree.json`
    * the JSON files are the QADB files
      * for now we use JSON out of convenience, although it's not a proper
        database format; future development plans include considering more
        efficient formats, such as SQLlite
  * run `bin/printGoldenFiles.sh` to print a list of all golden files (that is,
    files with no defects)
    * note: first set environment variables by running `source env.sh` (in
      bash, or in tcsh use `source env.csh`)
    * for convenience, this list is stored as `text/listOfGoldFiles.txt`
  * alternatively, run `bin/printGoldenRuns.sh` to print a list of golden runs
    * for convenience, this list is stored as `text/listOfGoldRuns.txt`
    * each run is classified as one of the following:
      * `gold`: all files have no defects. Note that this is **very strict**,
        so not many runs are `gold`, since most runs have at least one file
        with a defect; in practice it is better to apply QA cuts per file,
        using the QADB software
      * `silver`: the only defects are terminal outliers (first or last file is
        an outlier); note that `gold` runs are, by definition, also `silver`.
        This is also **very strict**: so far, only about half the runs are
        `silver`
      * defect: not `gold` or `silver`

* Groovy Access
  * first set environment variables by running `source env.sh`
    * `bash` is recommended, though if you choose to use `tcsh`, run
      instead `source env.csh`
  * then proceed following `src/README.md`

* C++ Access
  * __NOTE:__ [`clas12root`](https://github.com/JeffersonLab/clas12root) now provides
    access to the QADB
  * needs [`rapidjson`](https://github.com/Tencent/rapidjson/) libary; 
    it is a submodule of this repository and can be obtained by
    ```
    git clone --recurse-submodules https://github.com/JeffersonLab/clasqaDB.git
    ```
  * first set environment variables by running `source env.sh`
    * alternatively, set environment variable `$QADB` to the path to this
      `clasqaDB` repository
    * `bash` is recommended, though if you choose to use `tcsh`, run
      instead `source env.csh`
  * then proceed following `srcC/README.md`


## QA data storage

### Table files
Human-readable format of QA result, stored in `qadb/qa.*/qaTree.json.table`
* each run begins with the keyword `RUN:`; lines below are for each of that 
  run's file and its QA result, with the following syntax:
  * `run number` `file number`  `defect bits` `comment`
  * the `defect bits` are listed by name, and the numbers in the `[brackets]`
    indicate which sectors have that defect; the defect bits are:
    * `GOLDEN`: if no defect bits are assigned in any sector, the file is called "golden"
    * `TotalOutlier`: outlier N/F, but not terminal, marginal, or sector loss
    * `TerminalOutlier`: outlier N/F of first or last file of run, not marginal
    * `MarginalOutlier`: marginal outlier N/F, within one stddev of cut line
    * `SectorLoss`: N/F diminished within a sector for several consecutive files
    * `LowLiveTime`: live time < 0.9
    * `Misc`: miscellaneous defect, documented as comment
  * if a comment is included, it will be printed after the defect bits, following the
    `::` delimiter
* these table files can be generated from the JSON files using `bin/makeTables.sh`

### JSON files

#### qaTree.json
* The QADB itself is stored as JSON files in `qadb/qa.*/qaTree.json`
* the format is a tree (nested maps):
```
qaTree.json ─┬─ run number 1
             ├─ run number 2 ─┬─ file number 1
             │                ├─ file number 2
             │                ├─ file number 3 ─┬─ evnumMin
             │                │                 ├─ evnumMax
             │                │                 ├─ sectorDefects
             │                │                 ├─ defect
             │                │                 └─ comment
             │                ├─ file number 4
             │                └─ file number 5
             ├─ run number 3
             └─ run number 4
```
* for each file, the following variables are defined:
  * `evnumMin` and `evnumMax` represent the range of event numbers associated
    to this file; use this to map a particular event number to a file number
  * `sectorDefects` is a map with sector number keys paired with lists of associated
    defect bits
  * `defect` is a decimal represantion of the `OR` of each sector's defect bits, for
    example, `11=0b1011` means the `OR` of the defect bit lists is `[0,1,3]`
  * `comment` stores an optional comment regarding the QA result

#### chargeTree.json
* the charge is also stored in JSON files in `qadb/qa.*/chargeTree.json`, with
  a similar format:
```
chargeTree.json ─┬─ run number 1
                 ├─ run number 2 ─┬─ file number 1
                 │                ├─ file number 2
                 │                ├─ file number 3 ─┬─ fcChargeMin
                 │                │                 ├─ fcChargeMax
                 │                │                 ├─ ufcChargeMin
                 │                │                 ├─ ufcChargeMax
                 │                │                 └─ nElec ─┬─ sector 1
                 │                │                           ├─ sector 2
                 │                │                           ├─ sector 3
                 │                │                           ├─ sector 4
                 │                │                           ├─ sector 5
                 │                │                           └─ sector 6
                 │                ├─ file number 4
                 │                └─ file number 5
                 ├─ run number 3
                 └─ run number 4
```
* for each file, the following variables are defined:
  * `fcChargeMin` and `fcChargeMax` represent the minimum and maximum DAQ-gated
    Faraday cup charge, in nC
  * `ufcChargeMin` and `ufcChargeMax` represent the minimum and maximum FC charge,
    but not gated by the DAQ
  * the difference between the maximum and minimum charge is the accumulated charge
    in that file
  * `nElec` lists the number of electrons from each sector


## Accessing Faraday Cup Charge
* the charge is stored in the QADB for each DST file, so that it is possible to
  determine the amount of accumulated charge for data that satisfy your
  specified QA criteria.
* see `src/examples/chargeSum.groovy` or `srcC/examples/chargeSum.cpp` for
  usage example in an analysis event loop
  * call `QADB::AccumulateCharge()` within your event loop, after your QA cuts
    are satsified; the QADB instance will keep track of the accumulated charge
    you analyzed (accumulation performed per DST file)
  * at the end of your event loop, the total accumulated charge you analyzed is
    given by `QADB::getAccumulatedCharge()`
* note: we find some evidence that the charge from file to file may slightly overlap,
  or there may be gaps in the accumulated charge between each file; the former leads to
  a slight overcounting and the latter leads to a slight undercounting
  * for RGK, we find the correction to this issue would be very small
    (no more than the order of 0.1%)
  * corrections of this issue are therefore not applied
  * if you require higher precision of the accumulated charge than what is
    provided, contact the developers to discuss an implementation of the
    corrections


# QADB Management

Documentation for QADB maintenance and revision

## Adding to or revising the QADB
* the QADB files are produced by [`clasqa`](https://github.com/c-dilks/clasqa);
  if you have produced QA results for a new data set, and would like to add
  them to the QADB, or if you would like to update results for an existing
  dataset, follow the following procedure:
  * `mkdir qadb/qa.${dataset}/`, then copy the final `qaTree.json` and
    `chargeTree.json` to that directory
  * run `bin/makeTables.sh`
  * run `bin/printGoldenRuns.sh > text/listOfGoldRuns.txt`, then remove any
    extraneous output in `text/listOfGoldRuns.txt`, such as the COATJAVA
    version; repeat the same procedure for
    `bin/printGoldenFiles.sh > text/listOfGoldFiles.txt`
  * use `git status` and `git diff` to review changes, then add and commit to
    git, and push to the remote branch


# Change Log

### December 2020
* `qa.*/` directories have been moved to the subdirectory `qadb/`
  `qadb/qa.inbending1` and `qadb/qa.inbending2` have been merged and 
  renamed to `qadb/qa.rga_inbending`; furthermore, `qadb/qa.outbending`
  has been renamed to `qadb/qa.rga_outbending`
* `data_table.dat` files replaced by `chargeTree.json` files
* FC charge is now accessible in both Groovy and C++ classes; see examples
* C++ and Groovy classes have been updated and synchronized
  * several new methods added, especially for charge access
  * the methods `hasDefect` and/or `hasDefectName` have been changed such
    that:
    * `hasDefect` takes a defect *name* as an argument
    * `hasDefectBit` takes a defect *bit number* as an argument
    * `hasDefectName` is no longer used; use `hasDefect` instead
  * C++ class no longer requires the JSON files be merged with a Groovy
    script; the class only requires the environment variable `$QADB`
