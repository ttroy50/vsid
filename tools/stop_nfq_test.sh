#!/bin/bash

if (( $EUID != 0 )); then
    echo "Please run as root"
    exit
fi

echo "resetting iptables"

iptables -D INPUT -p tcp -j NFQUEUE --queue-balance 0:1
iptables -D OUTPUT -p tcp -j NFQUEUE --queue-balance 0:1

#iptables -D INPUT -p tcp -j NFQUEUE --queue-num 0
#iptables -D OUTPUT -p tcp -j NFQUEUE --queue-num 0

echo "Stopping process"

#kill $(ps aux | grep '[v]sid_netfilter' | awk '{print $2}')
pkill vsid_nfqueue_test

echo "Stopped"