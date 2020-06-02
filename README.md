# clasqaDB
Result of `clasqa`, stored in `.json` files


## QA Database Accessors

* Text Access
  * this only provides human-readable access; see below for access with
    common programming languages used at CLAS
  * run `bin/parse.sh` to convert `.json` files into `.table` files, which are
    easier to understand
  * after making `.table` files, you can run `bin/printGoldenList.sh` to print
    a list of all golden files

* Groovy Access
  * __STATUS:__ working and tested, please report any bugs
  * first, set environment variables with `source env.sh`
  * then run your analysis script with `run-groovy`, a groovy wrapper script
    provided by `coatjava`
  * see example scripts in the `src/` subdirectory
  * usage:
    * include the `QADB` class with `import clasqa.QADB`, then instantiate
    * the `QADB` class provides several methods for accessing the QA info;
      you only need to provide it a run number and event number
      * database lookups are only performed as needed, so it is safe to
        use any accessor in a standard analysis event loop

* C++ Access
  * __STATUS:__ working, but not fully tested and lacks some features
  * needs [`rapidjson`](https://github.com/Tencent/rapidjson/) libary; 
    it is a submodule of this repository and can be obtained
    by
    ```
    git clone --recurse-submodules https://github.com/c-dilks/clasqaDB.git
    ```
  * add the following directories to your project's include path:
    * `srcC/include` 
    * `srcC/rapidjson/include`
  * see `srcC/test.cpp` for a usage example

* Java Access
  * __STATUS:__ not working yet
  * needs `JSON.simple`, which can be found in the Maven repository
    * on Debian-based distributions, it can be obtained via 
      `apt install libjson-simple-java`


## QA data storage

### Table files
Human-readable format of QA result, stored in `qa.*/qaTree.json.table`
* these need to be generatd with `bin/parse.sh`
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

### JSON files
A file which can be read by code, stored in `qa.*/qaTree.json`
* the format of this file is like a tree:
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


