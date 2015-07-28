#!/usr/bin/python
#
"""
Analyse the classification results
"""
import sys
import yaml
from optparse import OptionParser


class IPv4Tuple():
    """
    Basic representation of an IPv4 ip_tuple
    """

    def __init__(self, src_ip, src_port, dst_ip, dst_port, protocol, l7protocol=None ):
        self.src_ip = src_ip
        self.src_port = src_port
        self.dst_ip = dst_ip
        self.dst_port = dst_port
        if isinstance(protocol, (long, int)):
            self.protocol = protocol
        else:
            if protocol == "TCP":
                self.protocol = 6
            else:
                self.protocol = 17

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


class ProtocolResult:
    """
    Result of a protocol analysis
    """

    def __init__(self, name):
        self.name = name
        self.true_positive = 0
        self.false_positive = 0
        self.true_negative = 0
        self.false_negative = 0
        self.not_classified = 0
        self.expected = 0

    def increase_true_positive(self):
        self.true_positive = self.true_positive + 1
    
    def increase_false_positive(self):
        self.false_positive = self.false_positive + 1

    def increase_true_negative(self):
        self.true_negative = self.true_negative + 1

    def increase_false_negative(self):
        self.false_negative = self.false_negative + 1

    def increase_not_classified(self):
        self.not_classified = self.not_classified + 1

    def increase_expected(self):
        self.expected = self.expected + 1

    def dict_repr(self):
        return self.name

    def as_dict(self):
        ret = {}
        ret["true_positive"] = self.true_positive
        ret["false_positive"] = self.false_positive
        ret["true_negative"] = self.true_negative
        ret["false_negative"] = self.false_negative
        ret["not_classified"] = self.not_classified
        ret["expected"] = self.expected
        return ret

class Analyser:

    def __init__(self, default_protocol, protocol_database):
        self.protocol_results = {}
        self.results = None
        self.flows = None
        self.default_protocol = default_protocol
        self.protocol_database = protocol_database

    def read_database(self):
        """
        Read all protocol types in the Database
        """
        if self.protocol_database is None:
            self.protocol_database = self.results["ProtocolDatabase"]["FileName"]
            
        try:
            with open(self.protocol_database, 'r') as stream:
                database = yaml.load(stream)
        except Exception, ex:
            print "Exception loading db file : %s" %ex
            sys.exit(1)

        if database is None:
            print "Unable to load yaml %s" %ex
            sys.exit(1)

        for proto in database["ProtocolModels"]:
            if proto["ProtocolName"] not in self.protocol_results:
                self.protocol_results[proto["ProtocolName"]] = ProtocolResult(proto["ProtocolName"])

    def read_results(self, file):
        if file is None:
            print "Invalid results file"
            sys.exit(1)

        try:
            with open(file, 'r') as stream:
                self.results = yaml.load(stream)
        except Exception, ex:
            print "Exception loading results file : %s" %ex
            sys.exit(1)
   
    def read_flows(self, file):
        if file is None:
            return

        try:
            with open(file, 'r') as stream:
                yaml_flows = yaml.load(stream)
        except Exception, ex:
            print "Exception loading results file : %s" %ex
            sys.exit(1)

        for flow in yaml_flows["flows"]:
            ip_tuple = IPv4Tuple(flow["src_ip"], flow["src_port"],flow["dst_ip"],flow["dst_port"],flow["transport"], flow["protocol"])
        
            self.flows[ip_tuple.dict_repr()] = ip_tuple

            if flow["protocol"] not in self.protocol_results:
                print "Warning protocol [%s] in flows but not database" %flow["protocol"]
                self.protocol_results[flow["protocol"]] = ProtocolResult(flow["protocol"])
            

            self.protocol_results[flow["protocol"]].increase_expected()


    def analyse_results(self):

        if self.results is None:
            print "ERROR: No results to analyse"
            sys.exit(1)

        if self.flows is None and self.default_protocol is None:
            print "ERROR: No protocol or flows to check against"
            sys.exit(1)

        for result in self.results["Results"]:
            flow = result["Flow"]
            src = flow["src"].split(":")
            dst = flow["dst"].split(":")

            ip_tuple = IPv4Tuple(src[0], int(src[1]), dst[0], int(dst[1]), flow["transport"])
            
            expected_l7protocol = self.default_protocol
            if self.flows is not None and ip_tuple.dict_repr() in self.flows:
                expected_l7protocol = self.flows[ip_tuple.dict_repr()]["protocol"]
            else:
                self.protocol_results[self.default_protocol].increase_expected()

            if expected_l7protocol not in self.protocol_results:
                self.protocol_results[expected_l7protocol] = ProtocolResult(expected_l7protocol)

            if "Protocol" in result and result["Protocol"] not in self.protocol_results:
                print "Warning [%s] in results but not in flows" %result["Protocol"]
                self.protocol_results[result["Protocol"]] = ProtocolResult(result["Protocol"])

    #
    # From : KISS: Stochastic Packet Inspection Classifier for UDP Traffic
    #   Classification accuracy is often reported in terms of False
    #   Positive {FP} and True Positive (TP), and the False Negative
    #   (FN) and True Negative (TN). A test is said to be "True" if the
    #   classification result and the oracle are in agreement. A test is
    #   said "False" on the contrary. The result of a test is "Positive"
    #   if the classifier accepts the sample as belonging to the specific
    #   class. On the contrary, a test is "Negative" For example, con-
    #   sider a flow. The oracle states that this flow is an eMule flow.
    #   If the flow is classified as an eMule flow, then we have a True
    #   Positive. If not, then we have a False Negative. Consider instead
    #   a flow that is not an eMule flow according to the oracle. If the
    #   flow is classified as an eMule flow, then we have a False Posi-
    #   tive. If not, then we have a True Negative
    #   
            if flow["classified"]:
                if result["Protocol"] == expected_l7protocol:
                    self.protocol_results[expected_l7protocol].increase_true_positive()

                    for k, v in self.protocol_results.iteritems():
                        if k == expected_l7protocol:
                            continue
                        else:
                            v.increase_true_negative()

                else:
                    self.protocol_results[expected_l7protocol].increase_false_negative()  

                    self.protocol_results[result["Protocol"]].increase_false_positive()

                    for k, v in self.protocol_results.iteritems():
                        if k == expected_l7protocol or k == result["Protocol"]:
                            continue
                        else:
                            v.increase_true_negative()



            else:
                self.protocol_results[expected_l7protocol].increase_not_classified()
                for k, v in self.protocol_results.iteritems():
                    if k == expected_l7protocol:
                        continue
                    else:
                        v.increase_true_negative()


    def print_results(self):
        print yaml.dump(self.protocol_results, indent=4, default_flow_style=False) 


def main():
    parser = OptionParser()
    parser.add_option("-r", "--results", dest="filename",
                      help="Results file from classifier to analyse", metavar="FILE")
    parser.add_option("-f", "--flows", dest="flowsfile",
                      help="File with a list of flows and their protocols.", metavar="FILE")
    parser.add_option("-p", "--protocol", dest="protocol",
                      help="Protocol Name to match against. This for matching all files not in the flows file", metavar="protocol")
    parser.add_option("-d", "--database", dest="database",
                      help="Protocol Database. If empty will attempt to use one from results", metavar="database")

    (options, args) = parser.parse_args()

    if options.filename is None or options.filename == "":
        print "ERROR: No Results file supplied\n"
        parser.print_help()
        sys.exit(1)

    if (options.flowsfile is None or options.flowsfile == "") and (options.protocol is None or options.protocol == ""):
        print "ERROR: No flows file or protocol to compare results against\n"
        parser.print_help()
        sys.exit(1)

    analyser = Analyser(options.protocol, options.database)

    analyser.read_results(options.filename)
    analyser.read_database()
    analyser.read_flows(options.flowsfile)


    analyser.analyse_results()
    analyser.print_results()

if __name__ == "__main__":
    # execute only if run as a script
    main()