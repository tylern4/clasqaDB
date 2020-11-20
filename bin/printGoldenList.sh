#!/bin/bash
# prints list of golden files
# this is meant to be run after `makeTables.sh`
if [ -z "$QADB" ]; then
  echo "ERROR: you must source env.sh first"; exit
fi
pushd $QADB > /dev/null
for file in qa.*/qaTree.json.table; do
  grep -w GOLDEN $file | awk '{print $1" "$2}'
done
popd > /dev/null
