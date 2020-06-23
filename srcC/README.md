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
- see `srcC/test.cpp` for a usage example; build it with `make`
  - instantiate QADB, specifying the location of the merged `qaTree.json` file
  - in an event loop, you must call `QADB::Query`, giving it a run number and
    event number; this will perform the lookup (only as necessary)
  - if `QADB::Query` returns true, it found a file with QA information; then
    you can use all the other methods to access it, which are exemplified in
    `test.cpp`
