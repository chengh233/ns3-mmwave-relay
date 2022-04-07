#!/bin/bash


N_RELAY=4


# split file for gnb and iab

for i in 1 2 3 4 
do
    grep "\([0-9]*\.[0-9]*\) $i *" DlPdcpStats.txt > DlPdcpStats_$i.txt
    wc -l DlPdcpStats_$i.txt
done

wc -l DlPdcpStats.txt

