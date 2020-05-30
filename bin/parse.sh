#!/bin/bash
# convert json files into human-readable tables
# needs `clasqa` repository
source `dirname $0`/../env.sh
for file in qa.*/qaTree.json; do
  run-groovy src/clasqa/parseQaTree.groovy $file
done
