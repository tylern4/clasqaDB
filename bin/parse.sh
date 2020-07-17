#!/bin/bash
# convert json files into human-readable tables
# run this from top-level directory of `clasqaDB`
if [ -z "$QADB" ]; then
  echo "ERROR: you must source env.sh first"; exit
fi
pushd $QADB
for file in qa.*/qaTree.json; do
  run-groovy src/clasqa/parseQaTree.groovy $file
done
popd
