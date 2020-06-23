# Groovy access to clasqaDB

this directory contains the Groovy source code to access the QA database

- first, set environment variables with `source env.sh` (do this in `..`,
  the parent directory)
- then run your analysis script with `run-groovy`, a groovy wrapper script
  provided by `coatjava`
- see example scripts in this directory 
- usage:
  - include the `QADB` class with `import clasqa.QADB`, then instantiate
  - the `QADB` class provides several methods for accessing the QA info;
    you only need to provide it a run number and event number
    - database lookups are only performed as needed, so it is safe to
      use any accessor in a standard analysis event loop
