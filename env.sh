#!/bin/bash

export QADB="$PWD"

JYPATH="${JYPATH}:${PWD}/src/"
export JYPATH=$(echo $JYPATH | sed 's/^://')
