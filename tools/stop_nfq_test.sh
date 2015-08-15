#!/bin/bash

if (( $EUID != 0 )); then
    echo "Please run as root"
    exit
fi

echo "resetting iptables"

#iptables -D INPUT -p tcp -j NFQUEUE --queue-balance 0:1
#iptables -D OUTPUT -p tcp -j NFQUEUE --queue-balance 0:1

iptables -D FORWARD -i eth1 -o vboxnet0 -j NFQUEUE --queue-num 0 #--queue-balance 0:1
iptables -D FORWARD -i vboxnet0 -o eth1 -j NFQUEUE --queue-num 0 #--queue-balance 0:1


#iptables -D INPUT -p tcp -j NFQUEUE --queue-num 0
#iptables -D OUTPUT -p tcp -j NFQUEUE --queue-num 0

echo "Stopping process"

#kill $(ps aux | grep '[v]sid_netfilter' | awk '{print $2}')
pkill -9 vsid_nfqueue_test
pkill -9 vsid_nfqueue_test

echo "Stopped"