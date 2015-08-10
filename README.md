# Video Stream Identification using Netfilter

M.EEng Dublin City University

Thom Troy

## Building

### Requirements

* autoconf
* libpcap
* [YAML-cpp])(https://github.com/jbeder/yaml-cpp)
* boost-dev
* libnetfilter-queue

### Steps

The steps to build are

```
 $ mkdir build
 $ cd build
 $ ../configure
 $ make
```

## Running 

### VSID Netfilter Classifier

Once build you can run the project from the build folder by running
```
 $ sudo ../tools/start_vsid_nf.sh
```

This will run with the default protocol database from config/protocol_model.db.yaml with the default config from config/config_netfilter.yaml

To change the iptables command that passes packets to the queues, edit that script.

To stop the process and unbind from iptables
```
 $ sudo ../tools/stop_vsid_nf.sh
```

### VSID Pcap Classifier

```
 $ src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /path/to/pcap

```
### VSID Pcap Trainer

```
 $ src/training/src/vsid_training -c ../config/config_training.yaml -t /path/to/training_file.yaml
```


## Related Projects

* [Original SPID algorithm](http://sourceforge.net/projects/spid/)

* [Alternative Implementation](https://github.com/cit/Spid)