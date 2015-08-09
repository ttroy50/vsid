#!/usr/bin/python
#
"""
Move a protocol to a different position in the list
"""
import sys
import yaml
from optparse import OptionParser


def move_protocol(file, dest, protocol, position):

    try:
        with open(file, 'r') as stream:
            database = yaml.load(stream)
    except Exception, ex:
        print "Exception loading db file : %s" %ex
        sys.exit(1)

    if database is None:
        print "Unable to load yaml %s" %ex
        sys.exit(1)

    if position > len(database["ProtocolModels"]):
        position = len(database["ProtocolModels"])
    elif position < 0:
        position = 0

    index = 0
    found= False
    for proto in database["ProtocolModels"]:
        if proto["ProtocolName"] == protocol:
            found = True
            break

        index = index + 1

    if found:
        database["ProtocolModels"].insert(position, database["ProtocolModels"].pop(index))

        print "Protocol Moved"
    else:
        print "Error: Protocol not found in database"
        return

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
    parser.add_option("-p", "--protocol", dest="protocol",
                      help="Protocols move")
    parser.add_option("-a", "--at", dest="at", type="int", default=256,
                      help="New Position Number", metavar="POS")

    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Database file supplied\n"
        parser.print_help()
        sys.exit(1)

    if options.protocol is None or options.protocol == "":
        print "ERROR: No Protocol supplied\n"
        parser.print_help()
        sys.exit(1)

    print "Moving [%s] to [%d]" %(options.protocol, options.at)
    move_protocol(options.filename, options.destfile, options.protocol, options.at)


if __name__ == "__main__":
    # execute only if run as a script
    main()