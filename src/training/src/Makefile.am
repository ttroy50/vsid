# what flags you want to pass to the compiler & linker
CFLAGS = --pedantic -Wall -O2
AM_LDFLAGS =


# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = vsid_training


vsid_training_INCS = \
				-I/usr/local/include \
				-I$(srcdir)/../inc \
				-I$(srcdir)/../../common/inc \
				-I$(srcdir)/../../protocol_model/inc \
				-I$(top_srcdir)/contrib/easyloggingpp \
				-I$(top_srcdir)/contrib/hash/inc

vsid_training_CXXFLAGS =  -std=c++11 -DELPP_THREAD_SAFE -pthread  $(vsid_training_INCS)

vsid_training_LDADD = 	-lpcap \
						../../common/src/lib_vsid_common.a \
						../../protocol_model/src/lib_vsid_protocol_model.a \
						../../../contrib/hash/src/lib_vsid_hash.a \
						-lyaml-cpp

vsid_training_SOURCES = main.cpp \
						Config.cpp \
						PcapReader.cpp \
						TrainingInput.cpp \
						ProtocolModelUpdater.cpp


