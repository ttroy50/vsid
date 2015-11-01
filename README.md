# Video Stream Identification using Netfilter

M.EEng Dublin City University

Thom Troy

## Status

[ ![Codeship Status for matrim/vsid](https://codeship.com/projects/516b87e0-fa63-0132-7227-1a34b2d0f857/status?branch=master)](https://codeship.com/projects/86931)

## Building

### Requirements

* cmake or autoconf
* libpcap
* [YAML-cpp])(https://github.com/jbeder/yaml-cpp)
* boost-dev
* libnetfilter-queue

### Steps

The steps to build are

```
 $ mkdir build
 $ cd build
 $ ../cmake
 $ make
```

## Running 
Note: Some of these steps are not updated for the cmake build version yet....

### VSID Netfilter Classifier

Once build you can run the project from the build folder by running
```
 $ sudo ../tests/start_vsid_nf.sh
```

This will run with the default protocol database from config/protocol_model.db.yaml with the default config from config/config_netfilter.yaml

To change the iptables command that passes packets to the queues, edit that script.

To stop the process and unbind from iptables
```
 $ sudo ../tools/stop_vsid_nf.sh
```

### VSID Pcap Classifier

```
 $ src/pcap_classifier/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /path/to/pcap

```
### VSID Pcap Trainer

```
 $ src/training/vsid_training -c ../config/config_training.yaml -t /path/to/training_file.yaml
```

## Documentation

Doxygen documentation for the code may be found from [here](http://matrim.bitbucket.org/vsid/)

To regenerated the documentation when using cmake do

```
make doc
```

The output from this will be in the <build>/doxygen folder.

## Related Projects

* [Original SPID algorithm](http://sourceforge.net/projects/spid/)

* [Alternative Implementation](https://github.com/cit/Spid)