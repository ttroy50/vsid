#!/usr/bin/python
#
"""
Reset an Protocol in the database to 0
"""
import sys
import yaml
from optparse import OptionParser


def reset_protocol(file, dest, name, attributes=None):

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
        if proto["ProtocolName"] == name:
            print "resetting %s" % proto["ProtocolName"]
            for meter in proto["AttributeMeters"]:
                if attributes is not None:
                    if meter["AttributeName"] not in attributes:
                        continue

                print "resetting %s" % meter["AttributeName"]
                num = len(meter["FingerPrint"])
                for val in range(0, num):
                    meter["FingerPrint"][val] = 0


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
                      help="Protocol Name", metavar="name")
    parser.add_option("-a", "--attribute", action="append", dest="attributes",
                      help="Attributes to reset. Not adding this means all")

    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Database file supplied\n"
        parser.print_help()
        sys.exit(1)

    if options.name is None or options.name == "":
        print "ERROR: No Name\n"
        parser.print_help()
        sys.exit(1)

    reset_protocol(options.filename, options.destfile, options.name, options.attributes)


if __name__ == "__main__":
    # execute only if run as a script
    main()