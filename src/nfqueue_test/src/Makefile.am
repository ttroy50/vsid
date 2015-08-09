# what flags you want to pass to the compiler & linker
CFLAGS = --pedantic -Wall -O2
AM_LDFLAGS =


# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = vsid_nfqueue_test


vsid_nfqueue_test_INCS = \
				-I/usr/local/include \
				-I$(srcdir)/../inc \
				-I$(srcdir)/../../common/inc \
				-I$(srcdir)/../../protocol_model/inc \
				-I$(top_srcdir)/contrib/easyloggingpp \
				-I$(top_srcdir)/contrib/hash/inc

vsid_nfqueue_test_CXXFLAGS =  -std=c++11 -DELPP_THREAD_SAFE -pthread $(vsid_nfqueue_test_INCS)

vsid_nfqueue_test_LDADD = 	\
						../../common/src/lib_vsid_common.a \
						../../../contrib/hash/src/lib_vsid_hash.a \
						../../protocol_model/src/lib_vsid_protocol_model.a \
						-lyaml-cpp \
						-lnfnetlink \
						-lnetfilter_queue

vsid_nfqueue_test_SOURCES = \
						main.cpp \
						Process.cpp \
						Config.cpp \
						PacketHandler.cpp



