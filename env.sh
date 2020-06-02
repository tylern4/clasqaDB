#!/bin/bash

export QADB=$(dirname $(realpath $0))

JYPATH="${JYPATH}:${PWD}/src/"
export JYPATH=$(echo $JYPATH | sed 's/^://')
