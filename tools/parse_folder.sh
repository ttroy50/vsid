#!/bin/bash

FILES=$1/*.pcap
for f in $FILES
do
#echo "$f"
../tools/parse_pcap_flows.py -f "$f" -p $2
done


 
