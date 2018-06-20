#!/bin/bash
SCRIPT_HOME=`dirname $0 | while read a; do cd $a && pwd && break; done`
XX="-DLISTFORMAT "
listme() {
	$SCRIPT_HOME/listpositany.sh $1 -DLISTFORMAT=Biov
	mv tmp "posit$1.bin"
}

listme int8_t,8,0,uint16_t,false
listme int8_t,8,0,uint16_t,true 
listme int8_t,8,1,uint16_t,false 
listme int8_t,8,1,uint16_t,true 
listme int8_t,8,2,uint16_t,false 
listme int8_t,8,2,uint16_t,true 
listme int8_t,8,3,uint16_t,false 
listme int8_t,8,3,uint16_t,true 
listme int16_t,12,0,uint16_t,false 
listme int16_t,12,0,uint16_t,true 
listme int16_t,12,1,uint16_t,false 
listme int16_t,12,1,uint16_t,true 
listme int16_t,12,2,uint16_t,false 
listme int16_t,12,2,uint16_t,true 
listme int16_t,12,3,uint16_t,false 
listme int16_t,12,3,uint16_t,true 
listme int16_t,12,4,uint16_t,false 
listme int16_t,12,4,uint16_t,true 
listme int16_t,10,0,uint16_t,false 
listme int16_t,10,0,uint16_t,true 
listme int16_t,10,2,uint16_t,false 
listme int16_t,10,1,uint16_t,true 
listme int16_t,10,1,uint16_t,false 
listme int16_t,10,2,uint16_t,true 
listme int16_t,10,3,uint16_t,true 
listme int16_t,10,3,uint16_t,false 
listme int16_t,16,0,uint16_t,false 
listme int16_t,16,0,uint16_t,true 