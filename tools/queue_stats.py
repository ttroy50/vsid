#!/usr/bin/python
#
"""
 Get the stats from /proc/net/netfilter/nfnetlink_queue and print in a nicer format

"""

import sys, os
from time import strftime, localtime, time, ctime
import datetime

def main():
    print "getting stats"
    
    c = "cat /proc/net/netfilter/nfnetlink_queue"
    try:
        f = os.popen(c, 'r')
        res = f.read()
        f.close()
    except IOError:
        print "Unable to read file"
        sys.exit(1)

  	if res is not None:
  		print "have res"
  		lines = res.splitlines()
  		print "%s lines" %len(lines)
  		for line in lines:
  			column = line.split('\t')
  			print "Queue : %s \t : received %s \t : queue dropped %s \t : userland dropped %d " %(line[0], line[7], line[5], line[6])

  	else:
  		print "res is None"

  	print "done"
if __name__ == "__main__":
    # execute only if run as a script
    main()