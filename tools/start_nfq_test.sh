#!/bin/bash

if (( $EUID != 0 )); then
    echo "Please run as root"
    exit
fi

echo "setting iptables"

iptables -F

iptables -A FORWARD -j NFQUEUE --queue-num 0

#
echo "Starting $1 with config $2"
#$1 -c $2 &
nice -n -10 src/nfqueue_test/src/vsid_nfqueue_test -c ../config/config_nfqueue_test.yaml &
echo "Started"
