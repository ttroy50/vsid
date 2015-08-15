#!/usr/bin/python
#
"""
Enable or disable an Attribute Meter
"""
import sys
import yaml
from optparse import OptionParser


def toggle_attribute_meter(file, dest, name, enabled=False, protocols=None):

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

        for meter in proto["AttributeMeters"]:
            if meter["AttributeName"] == name or name == "all":
                meter["Enabled"] = enabled


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
                      help="Database file to write to. If not supplied will use input database", metavar="FILE")
    parser.add_option("-n", "--name", dest="name",
                      help="Attribute Name. If 'all' then all attributes will be changed", metavar="name")
    parser.add_option("-e", "--enable", dest="enabled", action="store_true", default=False, help="If set enable the meter. Otherwise disable")
    parser.add_option("-p", "--protocols", action="append", dest="protocols",
                      help="Protocols to enable / disable the Attribute to. Not adding this means all")

    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Database file supplied\n"
        parser.print_help()
        sys.exit(1)

    if options.destfile is None or options.destfile == "":
        options.destfile = options.filename

    if options.name is None or options.name == "":
        print "ERROR: No Name\n"
        parser.print_help()
        sys.exit(1)

    toggle_attribute_meter(options.filename, options.destfile, options.name, options.enabled, options.protocols)


if __name__ == "__main__":
    # execute only if run as a script
    main()