#!/bin/bash
liste=`ls *.ltl`
for i in $liste 
do
cat $i >> test
echo "">>test
done

