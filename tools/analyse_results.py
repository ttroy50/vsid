#!/usr/bin/python
#
"""
Analyse the classification results
"""
# to have division result be a double
from __future__ import division

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
        self.under_defining_limit = 0
        self.expected = 0
        self.false_positive_protocols = {}
        self.false_negative_protocols = {}

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

    def increase_under_defining_limit(self):
        self.under_defining_limit = self.under_defining_limit + 1

    def add_false_positive_protocol(self, protocol):
        if protocol in self.false_positive_protocols:
            self.false_positive_protocols[protocol] = self.false_positive_protocols[protocol] + 1
        else:
            self.false_positive_protocols[protocol] = 1

    def add_false_negative_protocol(self, protocol):
        if protocol in self.false_negative_protocols:
            self.false_negative_protocols[protocol] = self.false_negative_protocols[protocol] + 1
        else:
            self.false_negative_protocols[protocol] = 1


    def calculate_analysis(self):
        if self.true_positive == 0 and self.false_negative == 0:
            recall = 0
        else:
            recall = self.true_positive / (self.true_positive + self.false_negative)

        if self.true_positive == 0 and self.false_positive == 0:
            precision = 0
        else:
            precision = self.true_positive / (self.true_positive + self.false_positive)

        if precision == 0 and recall == 0:
            fmeasure = 0
        else:
            fmeasure = (2 * precision * recall) / ( precision + recall )
        return (recall*100, precision*100, fmeasure*100)

    def calculate_analysis_inc_not_classified(self):
        
        exp = self.expected - self.under_defining_limit
        nc = self.not_classified - self.under_defining_limit 

        fn = self.false_negative + nc
        
        if self.true_positive == 0 and fn == 0:
            recall = 0
        else:
            recall = self.true_positive / (self.true_positive + fn)

        if self.true_positive == 0 and self.false_positive == 0:
            precision = 0
        else:
            precision = self.true_positive / (self.true_positive + self.false_positive)

        if precision == 0 and recall == 0:
            fmeasure = 0
        else:
            fmeasure = (2 * precision * recall) / ( precision + recall )
        return (recall*100, precision*100, fmeasure*100)
        

    def calculate_not_classified(self):

        exp = self.expected - self.under_defining_limit
        nc = self.not_classified - self.under_defining_limit 

        adjusted_nc_pc = 0
        if exp != 0:
            adjusted_nc_pc = (nc / exp) * 100

        ul_pc = 0
        if self.expected != 0:
            ul_pc = (self.under_defining_limit / self.expected) * 100

        return (adjusted_nc_pc, ul_pc)

    def dict_repr(self):
        return self.name

    def as_dict(self):
        ret = {}
        ret["true_positive"] = self.true_positive
        ret["false_positive"] = self.false_positive
        ret["true_negative"] = self.true_negative
        ret["false_negative"] = self.false_negative
        ret["not_classified"] = self.not_classified
        ret["under_defining_limit"] = self.under_defining_limit
        ret["expected"] = self.expected
        ret["false_positive_protocols"] = self.false_positive_protocols
        ret["false_negative_protocols"] = self.false_negative_protocols
        return ret

class Analyser:

    def __init__(self, default_protocol, protocol_database):
        self.protocol_results = {}
        self.results = None
        self.flows = None
        self.default_protocol = default_protocol
        self.protocol_database = protocol_database
        self.defining_limit = 0
        self.cutoff_limit = 0

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
                file_str = stream.read()
                runs = file_str.split("---")

                if len(runs) > 2:
                    print "More than one result set in file"
                    for run in runs:
                        if len(run) == 0 or run is None:
                            continue
                        #run =  "---" + run

                        one_run = yaml.load(run)

                        if one_run is None:
                            print "Error: run is none "
                            continue 

                        if self.results is not None:
                            if one_run["ProtocolDatabase"]["FileName"] != self.results["ProtocolDatabase"]["FileName"]:
                                print "Warning: protocol databases different. [%s] [%s]" %(one_run["ProtocolDatabase"]["FileName"], self.results["ProtocolDatabase"]["FileName"])
                            if one_run["ProtocolDatabase"]["DefiningLimit"] != self.results["ProtocolDatabase"]["DefiningLimit"]:
                                print "Warning: protocol databases DefiningLimit different. [%s] [%s]" %(one_run["ProtocolDatabase"]["DefiningLimit"], self.results["ProtocolDatabase"]["DefiningLimit"])
                    
                            self.defining_limit = one_run["ProtocolDatabase"]["DefiningLimit"]
                            self.cutoff_limit = one_run["ProtocolDatabase"]["CutoffLimit"]

                            if one_run["Results"] is not None:
                                self.results["Results"] = self.results["Results"] + one_run["Results"]
                            else:
                                print "Warning: results is none"
                        else:
                            self.results = one_run
                else:
                    if file_str is not None:
                        self.results = yaml.load(file_str)
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

        if self.flows is None:
            self.flows = {}

        for flow in yaml_flows["flows"]:
            ip_tuple = IPv4Tuple(flow["src_ip"], flow["src_port"],flow["dst_ip"],flow["dst_port"],flow["transport"], flow["protocol"])
        
            self.flows[ip_tuple.dict_repr()] = ip_tuple

            if flow["protocol"] not in self.protocol_results:
                print "Warning protocol [%s] in flows but not database" %flow["protocol"]
                self.protocol_results[flow["protocol"]] = ProtocolResult(flow["protocol"])
            

            self.protocol_results[flow["protocol"]].increase_expected()


    def gather_results(self):
        """
        Calculate all the results into the results dictionary
        """
        if self.results is None:
            print "ERROR: No results to analyse"
            sys.exit(1)

        if self.flows is None and self.default_protocol is None:
            print "ERROR: No protocol or flows to check against"
            sys.exit(1)

        if self.default_protocol is not None and self.default_protocol not in self.protocol_results:
                self.protocol_results[default_protocol] = ProtocolResult(self.default_protocol)

        for result in self.results["Results"]:
            flow = result["Flow"]
            src = flow["src"].split(":")
            dst = flow["dst"].split(":")

            ip_tuple = IPv4Tuple(src[0], int(src[1]), dst[0], int(dst[1]), flow["transport"])
            
            expected_l7protocol = self.default_protocol
            if self.flows is not None and ip_tuple.dict_repr() in self.flows:
                expected_l7protocol = self.flows[ip_tuple.dict_repr()].l7protocol
            else:
                if self.default_protocol is not None:
                    self.protocol_results[self.default_protocol].increase_expected()

            if expected_l7protocol not in self.protocol_results:
                self.protocol_results[expected_l7protocol] = ProtocolResult(expected_l7protocol)

            #self.protocol_results[expected_l7protocol].increase_expected()

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
                    self.protocol_results[expected_l7protocol].add_false_negative_protocol(result["Protocol"])  


                    self.protocol_results[result["Protocol"]].increase_false_positive()
                    self.protocol_results[result["Protocol"]].add_false_positive_protocol(expected_l7protocol)

                    for k, v in self.protocol_results.iteritems():
                        if k == expected_l7protocol or k == result["Protocol"]:
                            continue
                        else:
                            v.increase_true_negative()



            else:
                self.protocol_results[expected_l7protocol].increase_not_classified()

                if flow["pkt_count"] < self.defining_limit:
                    self.protocol_results[expected_l7protocol].increase_under_defining_limit()

                for k, v in self.protocol_results.iteritems():
                    if k == expected_l7protocol:
                        continue
                    else:
                        v.increase_true_negative()


    def analyse_results(self):
        """
        Analyse the raw results into Recall / precision
        """
        analysis = {}
        for k, pr in self.protocol_results.iteritems():
            (recall, precision, fmeasure) = pr.calculate_analysis()
            analysis[k] = {}
            analysis[k]["recall"] = float("%.2f" %recall)
            analysis[k]["precision"] = float("%.2f" %precision)
            analysis[k]["fmeasure"] = float("%.2f" %fmeasure)

            (recall_pc, precision_pc, fmeasure_pc) = pr.calculate_analysis_inc_not_classified()
            analysis[k]["recall_pc"] = float("%.2f" %recall_pc)
            analysis[k]["precision_pc"] = float("%.2f" %precision_pc)
            analysis[k]["fmeasure_pc"] = float("%.2f" %fmeasure_pc)

            (nc, ul) = pr.calculate_not_classified()
            analysis[k]["unclassified"] = float("%.2f" %nc)
            analysis[k]["under_limit"] = float("%.2f" %ul)

        print yaml.dump(analysis,  indent=4, default_flow_style=False)


    def print_results(self):
        """
        Print the results
        """
        if self.flows is not None:
            print "Total flows in results file is %d" %len(self.flows)
        if self.results is not None:
            print "Total results is %d" %(len(self.results["Results"]))

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


    analyser.gather_results()

    analyser.print_results()


    analyser.analyse_results()

    

if __name__ == "__main__":
    # execute only if run as a script
    main()