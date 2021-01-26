#!/bin/bash
# prints list of golden files
# this is meant to be run after `makeTables.sh`
if [ -z "$QADB" ]; then
  echo "ERROR: you must source env.sh first"; exit
fi
run-groovy ${QADB}/util/printGoldenRuns.groovy
