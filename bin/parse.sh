#!/bin/bash
# convert json files into human-readable tables
# needs `clasqa` repository
for file in qa.*/qaTree.json; do
  run-groovy src/clasqa/parseQaTree.groovy $file
done
