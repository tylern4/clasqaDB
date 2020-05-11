#!/bin/bash
# convert json files into human-readable tables
# needs `clasqa` repository
for file in qa.*/qaTree.json; do
  groovy ../clasqa/QA/parseQaTree.groovy $file
done
