#!/bin/bash

if (( $EUID != 0 )); then
    echo "Please run as root"
    exit
fi

echo "resetting iptables"

iptables -F

echo "Stopping process"

#kill $(ps aux | grep '[v]sid_netfilter' | awk '{print $2}')
pkill vsid_netfilter

echo "Stopped"