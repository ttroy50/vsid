#!/usr/bin/python
#
"""
Check a results file for duplicate flows
"""
import sys
import yaml
from optparse import OptionParser


def read_results(file):
    if file is None:
        print "Invalid results file"
        sys.exit(1)

    try:
        with open(file, 'r') as stream:
            results = yaml.load(stream)
    except Exception, ex:
        print "Exception loading results file : %s" %ex
        sys.exit(1)

    flowHashes = []
    for result in results["Results"]:
        h = result["Flow"]["hash"]
        if h in flowHashes:
            print "Duplicate found %d" %h
        flowHashes.append(h)

    print "Total Results is %d" %len(flowHashes)


def main():
    parser = OptionParser()
    parser.add_option("-r", "--results", dest="filename",
                      help="Results file from classifier to analyse", metavar="FILE")
   
    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Results file supplied\n"
        parser.print_help()
        sys.exit(1)

    read_results(options.filename)


if __name__ == "__main__":
    # execute only if run as a script
    main()