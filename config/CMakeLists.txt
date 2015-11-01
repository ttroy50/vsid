###################################################
# Install for any non project specific configuration files

install (FILES 
    logging.conf
    protocol_model_db.yaml
    protocol_model_db.default.empty.yaml 
DESTINATION ${CONF_INSTALL_DIR})
