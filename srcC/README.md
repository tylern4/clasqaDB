# C++ access to clasqaDB

this directory contains the C++ source code to access the QA database

- first, make sure you have the
  [`rapidjson`](https://github.com/Tencent/rapidjson/) libary; 
  it is a submodule of this repository and can be obtained by
  ```
  git clone --recurse-submodules https://github.com/c-dilks/clasqaDB.git
  ```
- next, merge the QA json files into one file by running `groovy mergeQaTrees.groovy`
- in your analysis code, add the following directories to your project's include path:
  - `srcC/include` 
  - `srcC/rapidjson/include`
  - you may need to specify these as "absolute paths" (i.e., not relative paths); see
    `Makefile` macro `DEPS` for an example
- example event loops, which can be built with `make`
  - `example_forAsymmetry.cpp` for a usage example for spin asymmetry
    analysis
    - instantiate QADB, specifying the location of the merged `qaTree.json`
      file
    - only need to use `QADB::OkForAsymmetry` within an event loop, passing
      the run number and event number as arguments; this returns true
      if the event is within a DST file that passes the QA criteria for a 
      spin asymmetry analysis
    - no need to use `QADB::Query` (as in the example below)
    - make sure the event has `REC::Particle` and `RUN::config` banks before
      any lookup (otherwise you'll find events which were not part of the QA)
  - `example_dumpQA.cpp` to dump QA information for a specific run
    - instantiate QADB, specifying the location of the merged `qaTree.json`
      file
    - in an event loop, you must call `QADB::Query`, giving it a run number and
      event number; this will perform the lookup (only as necessary)
    - if `QADB::Query` returns true, it found a file with QA information; then
      you can use all the other methods to access it, which are exemplified
    - make sure the event has `REC::Particle` and `RUN::config` banks before
      any lookup (otherwise you'll find events which were not part of the QA)
- **NOTE** `QADB` has been tested with compiled analysis code, but some users
  encounter problems when interpreting analysis code with ROOT Cling; it is 
  recommended to compile your analysis code
