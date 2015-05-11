# what flags you want to pass to the compiler & linker
CFLAGS = --pedantic -Wall -O2
AM_LDFLAGS =


# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = vsid_training


vsid_training_INCS = \
				-I$(srcdir)/../inc \
				-I$(top_srcdir)/contrib/easyloggingpp

vsid_training_CXXFLAGS =  -std=c++11 $(vsid_training_INCS)

vsid_training_LDADD = -lpcap

vsid_training_SOURCES = main.cpp \
						Config.cpp \
						PcapReader.cpp


