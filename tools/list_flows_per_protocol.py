#!/usr/bin/python
#
"""
Print the list of Protocols and their flow count
"""
import sys
import yaml
from optparse import OptionParser


def list_meters(file, protocols=None):

    try:
        with open(file, 'r') as stream:
            database = yaml.load(stream)
    except Exception, ex:
        print "Exception loading db file : %s" %ex
        sys.exit(1)

    if database is None:
        print "Unable to load yaml %s" %ex
        sys.exit(1)

    for proto in database["ProtocolModels"]:
        if protocols is not None:
            if proto["ProtocolName"] not in protocols:
                continue

        print "%s : %d" %(proto["ProtocolName"], proto["FlowCount"])


def main():
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="filename",
                      help="Database file to load", metavar="FILE")
    parser.add_option("-p", "--protocols", action="append", dest="protocols",
                      help="Protocols to enable / disable the Attribute to. Not adding this means all")

    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Database file supplied\n"
        parser.print_help()
        sys.exit(1)

    list_meters(options.filename, options.protocols)


if __name__ == "__main__":
    # execute only if run as a script
    main()