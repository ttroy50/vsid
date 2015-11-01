###################################################
# Install for any non project specific script tools

install (PROGRAMS 
    add_meter_to_database.py
    change_protocol_position.py
    list_enabled_attribute_meters.py
    list_flows_per_protocol.py
    parse_folder.sh
    parse_pcap_flows.py
    reset_protocol_model.py
    set_defining_limit.py
    start_nfq_test.sh
    stop_nfq_test.sh
    start_vsid_nf.sh
    stop_vsid_nf.sh
    toggle_attribute_meter.py
    toggle_protocol_model.py
DESTINATION ${SCRIPT_INSTALL_DIR})

