#!/bin/bash
SCRIPT_HOME=`dirname $0 | while read a; do cd $a && pwd && break; done`
$SCRIPT_HOME/listpositany.sh int16_t,12,3,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,0,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,0,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,1,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,1,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,2,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,2,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,3,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int8_t,8,3,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,12,0,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,12,0,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,10,0,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,10,0,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,10,2,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,10,2,uint16_t,true -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,16,0,uint16_t,false -DHEADONLY
$SCRIPT_HOME/listpositany.sh int16_t,16,0,uint16_t,true -DHEADONLY