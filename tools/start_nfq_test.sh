#!/bin/bash

if (( $EUID != 0 )); then
    echo "Please run as root"
    exit
fi

echo "setting iptables"

#iptables -A INPUT -p tcp -j NFQUEUE --queue-balance 0:1
#iptables -A OUTPUT -p tcp -j NFQUEUE --queue-balance 0:1

iptables -A FORWARD -i eth1 -o vboxnet0 -j NFQUEUE --queue-num 0 #--queue-balance 0:1
iptables -A FORWARD -i vboxnet0 -o eth1 -j NFQUEUE --queue-num 0 #--queue-balance 0:1


#iptables -A INPUT -p tcp -j NFQUEUE --queue-num 0
#iptables -A OUTPUT -p tcp -j NFQUEUE --queue-num 0
#
echo "Starting $1 with config $2"
#$1 -c $2 &
nice -n -10 src/nfqueue_test/src/vsid_nfqueue_test -c ../config/config_nfqueue_test.yaml &
echo "Started"