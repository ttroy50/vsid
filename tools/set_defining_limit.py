#!/usr/bin/python
#
"""
Enable or disable an Attribute Meter
"""
import sys
import yaml
from optparse import OptionParser


def set_defining_limit(file, dest, limit):

    try:
        with open(file, 'r') as stream:
            database = yaml.load(stream)
    except Exception, ex:
        print "Exception loading db file : %s" %ex
        sys.exit(1)

    if database is None:
        print "Unable to load yaml %s" %ex
        sys.exit(1)

    database["DefiningLimit"] = limit

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
    parser.add_option("-n", "--limit", dest="limit", type="int",
                      help="Defining Limit for the database. Minimum is 2", metavar="NUM")

    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Database file supplied\n"
        parser.print_help()
        sys.exit(1)

    if options.destfile is None or options.destfile == "":
        options.destfile = options.filename

    if options.limit is None or options.limit < 2:
        print "ERROR: No Limit. Minimum limit is 2\n"
        parser.print_help()
        sys.exit(1)

    set_defining_limit(options.filename, options.destfile, options.limit)


if __name__ == "__main__":
    # execute only if run as a script
    main()