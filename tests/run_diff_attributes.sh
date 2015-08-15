#!/bin/bash

rm logs/vsid_pcap_classifier*

################
# Run with all attributes enabled
../tools/run_all.sh

mv logs/vsid_pcap_classifier_classification.log logs/vsid_pcap_classifier_classification_all.log


###########
# Run with ByteFrequency Disabled
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n ByteFrequencyMeter

../tools/run_all.sh

mv logs/vsid_pcap_classifier_classification.log logs/vsid_pcap_classifier_classification_no_ByteFrequencyMeter.log

###############
# Run with Direction Changes Disabled
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n ByteFrequencyMeter -e
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n DirectionChangesMeter

../tools/run_all.sh

mv logs/vsid_pcap_classifier_classification.log logs/vsid_pcap_classifier_classification_no_DirectionChangesMeter.log


###############
# Run iwht ByteFrequency and DirectionChanges disabled
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n ByteFrequencyMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n DirectionChangesMeter

../tools/run_all.sh

mv logs/vsid_pcap_classifier_classification.log logs/vsid_pcap_classifier_classification_no_DCM_BFM.log

###############
#
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n ByteFrequencyMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n DirectionChangesMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n EntropyFirstOrigToDestPacket

../tools/run_all.sh

mv logs/vsid_pcap_classifier_classification.log logs/vsid_pcap_classifier_classification_no_DCM_BFM_Entropy.log

###############
#
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n ByteFrequencyMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n DirectionChangesMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n EntropyFirstOrigToDestPacket -e
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n RtmpRegexMatchMeter
../tools/run_all.sh

mv logs/vsid_pcap_classifier_classification.log logs/vsid_pcap_classifier_classification_no_DCM_BFM_RtmpRegex.log

###############
#
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n ByteFrequencyMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n DirectionChangesMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n ActionReactionFirst3ByteHashMeter
../tools/toggle_attribute_meter.py -f ../config/protocol_model_db.yaml -d ../config/protocol_model_db.yaml -n RtmpRegexMatchMeter -e
../tools/run_all.sh

mv logs/vsid_pcap_classifier_classification.log logs/vsid_pcap_classifier_classification_no_DCM_BFM_AR3Byte.log
