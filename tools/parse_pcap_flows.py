#!/usr/bin/python
#
"""
Get a list of all TCP or flows in a pcap file. Used to get all flows in a pcap to insert 
into a training input file.

Note: this will count all different TCP flows to the same 5-tuple as being 1 flow
"""

from pcapfile import savefile
from pcapfile.protocols.linklayer import ethernet
from pcapfile.protocols.network import ip 
import binascii
import struct
import yaml
from optparse import OptionParser

class IPv4Tuple():
    """
    Basic representation of an IPv4 ip_tuple
    """

    def __init__(self, src_ip, dst_ip, protocol, payload, l7protocol ):
        self.src_ip = src_ip
        self.src_port = struct.unpack("!H", ip_packet.payload[0:4].decode('hex'))[0]
        self.dst_ip = dst_ip
        self.dst_port = struct.unpack("!H", ip_packet.payload[4:8].decode('hex'))[0]
        self.protocol = protocol
        self.l7protocol = l7protocol

    def dict_repr(self):
        """
        Returns a string that can identify the flow from either direction
        """
        ret = ""
        if self.src_ip < self.dst_ip:
            ret = "%s:%d %s:%d %d" %(self.src_ip, self.src_port,
                                    self.dst_ip, self.dst_port, self.protocol)
        elif self.src_ip > self.dst_ip:
            ret = "%s:%d %s:%d %d" %(self.dst_ip, self.dst_port,
                                    self.src_ip, self.src_port, self.protocol)
        else:
            if self.src_port <= self.dst_port:
                ret = "%s:%d %s:%d %d" %(self.src_ip, self.src_port,
                                    self.dst_ip, self.dst_port, self.protocol)
            else:
                ret = "%s:%d %s:%d %d" %(self.dst_ip, self.dst_port,
                                    self.src_ip, self.src_port, self.protocol)
        return ret

    def as_dict(self):
        ret = {}
        ret["src_ip"] = self.src_ip
        ret["src_port"] = self.src_port
        ret["dst_ip"] = self.dst_ip
        ret["dst_port"] = self.dst_port
        if self.protocol == 6:
            ret["transport"] = "TCP"
        else:
            ret["transport"] = "UDP"
        ret["protocol"] = self.l7protocol

        return ret


parser = OptionParser()
parser.add_option("-f", "--file", dest="filename",
                  help="pcap file to parse", metavar="FILE")
parser.add_option("-p", "--protocol", dest="protocol",
                  help="protocol to add", metavar="PROTOCOL", default="INSERT_PROTOCOL")

(options, args) = parser.parse_args()

if options.filename is None or options.filename == "":
    print "ERROR: Filename cannot be empty\n\n"
    parser.print_help();
    sys.exit(1)

testcap = open(options.filename)
capfile = savefile.load_savefile(testcap, layers=2)

flows = {}
flowsList = []

for packet in capfile.packets :
    ip_packet = packet.packet.payload

    # UDP or TCP
    if ip_packet.p != 17 and ip_packet.p != 6:
        print "Not TCP or UDP"
        continue

    ip_tuple = IPv4Tuple(ip_packet.src, ip_packet.dst, ip_packet.p, ip_packet.payload, options.protocol)
    
    if ip_tuple.dict_repr() in flows:
        pass
    else:
        flows[ip_tuple.dict_repr()] = ip_tuple
        flowsList.append(ip_tuple.as_dict())

print "Found  [%s] flows. YAML reprsentation below" %len(flows)
print ""
print ""
print ""

# Overall YAML output
outputYaml = []

# Output for this file
outputYamlFile = {}
outputYamlFile["file"] = options.filename
outputYamlFile["flows"] = flowsList

outputYaml.append(outputYamlFile)

output = yaml.dump(outputYaml, indent=4, default_flow_style=False)
print (output.replace('\n-', '\n\n-\n '))


     