#!/usr/bin/python
#
"""
Add a new empty Attribute Meter to a VSID Protocol Model database
"""
import sys
import yaml
from optparse import OptionParser


def add_attribute_meter(file, dest, name, size, protocols):

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
                print "Skipping [%s]" %proto["ProtocolName"]
                continue

        am = {}
        am["AttributeName"] = name
        am["Enabled"] = True 
        am["FlowCount"] = 0 
        am["Fingerprint"] = []
        for x in range(0, size):
            am["Fingerprint"].append(0)

        proto["AttributeMeters"].append(am)


    if dest is not None:
        with open(dest, 'w') as outfile:
            outfile.write( yaml.dump(database, default_flow_style=True, explicit_start=True) )
    else:
        print yaml.dump(database, default_flow_style=True, explicit_start=True)


def main():
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="filename",
                      help="Database file to load", metavar="FILE")
    parser.add_option("-d", "--dest", dest="destfile",
                      help="Database file to write to. If not supplied will write to stdout", metavar="FILE")
    parser.add_option("-n", "--name", dest="name",
                      help="Attribute Name", metavar="name")
    parser.add_option("-s", "--size", dest="size", type="int", default=256,
                      help="Attribute Fingerprint Size", metavar="size")
    parser.add_option("-p", "--protocols", action="append", dest="protocols",
                      help="Protocols to enable the Attribute to. Not adding this means all")

    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Database file supplied\n"
        parser.print_help()
        sys.exit(1)

    if options.name is None or options.name == "":
        print "ERROR: No Name\n"
        parser.print_help()
        sys.exit(1)

    add_attribute_meter(options.filename, options.destfile, options.name, options.size, options.protocols)


if __name__ == "__main__":
    # execute only if run as a script
    main()