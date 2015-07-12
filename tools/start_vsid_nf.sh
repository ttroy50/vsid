#!/bin/bash

echo "setting iptables"

iptables -A INPUT -p tcp -j NFQUEUE --queue-balance 0:2
iptables -A OUTPUT -p tcp -j NFQUEUE --queue-balance 0:2

echo "Starting $1 with config $2"
#$1 -c $2 &
src/netfilter/src/vsid_netfilter -c ../config/config_netfilter.yaml &
echo "Started"