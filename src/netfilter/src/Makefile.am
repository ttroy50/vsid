# what flags you want to pass to the compiler & linker
CFLAGS = --pedantic -Wall -O2
AM_LDFLAGS =


# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = vsid_netfilter


vsid_netfilter_INCS = \
				-I/usr/local/include \
				-I$(srcdir)/../inc \
				-I$(srcdir)/../../common/inc \
				-I$(srcdir)/../../protocol_model/inc \
				-I$(top_srcdir)/contrib/easyloggingpp \
				-I$(top_srcdir)/contrib/hash/inc

vsid_netfilter_CXXFLAGS =  -std=c++11 -DELPP_THREAD_SAFE -pthread $(vsid_netfilter_INCS)

vsid_netfilter_LDADD = 	\
						../../../contrib/hash/src/lib_vsid_hash.a \
						../../protocol_model/src/lib_vsid_protocol_model.a \
						-lyaml-cpp \
						-lnfnetlink \
						-lnetfilter_queue

vsid_netfilter_SOURCES = \ 
						main.cpp \
						Process.cpp \
						Config.cpp \
						PacketHandler.cpp



