#!/bin/bash
# prints list of golden files
for file in qa.*/qaTree.json.table; do
  grep -w GOLDEN $file | awk '{print $1" "$2}'
done
