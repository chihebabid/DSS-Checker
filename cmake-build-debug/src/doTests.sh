#!/bin/bash
list=`ls *.pn`
./distributedstatespace
for i in $list 
do
	./distributedstatespace --file $i > ${i}.txt &
done

