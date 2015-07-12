#!/bin/bash

echo "resetting iptables"

iptables -D INPUT -p tcp -j NFQUEUE --queue-balance 0:2
iptables -D OUTPUT -p tcp -j NFQUEUE --queue-balance 0:2

echo "Stopping process"

#kill $(ps aux | grep '[v]sid_netfilter' | awk '{print $2}')
pkill vsid_netfilter

echo "Stopped"