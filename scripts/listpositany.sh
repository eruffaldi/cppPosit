#!/bin/bash
SCRIPT_HOME=`dirname $0 | while read a; do cd $a && pwd && break; done`
shopt -s expand_aliases
. ~/.bash_profile
T=$1
shift
tabs -12
cling "-DPOSIT_TYPE=$T" -I$SCRIPT_HOME/../src -I$SCRIPT_HOME/../extern -I$SCRIPT_HOME/../include $SCRIPT_HOME/../src/listposits.cpp $*