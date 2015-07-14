TESTS = test_common

check_PROGRAMS = test_common
test_common_SOURCES = test_common.cpp

test_common_INCS = \
				-I/usr/local/include \
				-I$(srcdir)/../inc \
                -I$(srcdir)/../../protocol_model/inc \
				-I$(top_srcdir)/contrib/easyloggingpp \
				-I$(top_srcdir)/contrib/hash/inc

test_common_CFLAGS = --pedantic -Wall -O2 $(test_common_INCS)
test_common_CXXFLAGS =  -std=c++11 -DELPP_THREAD_SAFE $(test_common_INCS)

test_common_LDADD = ../../../contrib/hash/src/lib_vsid_hash.a \
					../../common/src/lib_vsid_common.a \
					../../protocol_model/src/lib_vsid_protocol_model.a \
					-lyaml-cpp \
					/usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.a