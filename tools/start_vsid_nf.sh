#!/bin/bash

if (( $EUID != 0 )); then
    echo "Please run as root"
    exit
fi

echo "setting iptables"

iptables -A INPUT -p tcp -j NFQUEUE --queue-balance 0:1
iptables -A OUTPUT -p tcp -j NFQUEUE --queue-balance 0:1

#iptables -A INPUT -p tcp -j NFQUEUE --queue-num 0
#iptables -A OUTPUT -p tcp -j NFQUEUE --queue-num 0
#
echo "Starting $1 with config $2"
#$1 -c $2 &
nice -n -10 src/netfilter/src/vsid_netfilter -c ../config/config_netfilter.yaml &
echo "Started"