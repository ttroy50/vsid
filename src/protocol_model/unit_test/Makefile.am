TESTS = test_model

check_PROGRAMS = test_model
test_model_SOURCES = test_model.cpp

test_model_INCS = \
				-I/usr/local/include \
				-I$(srcdir)/../inc \
				-I$(srcdir)/../../common/inc \
				-I$(top_srcdir)/contrib/easyloggingpp

test_model_CFLAGS = --pedantic -Wall -O2 $(test_model_INCS)
test_model_CXXFLAGS =  -std=c++11 -DELPP_THREAD_SAFE $(test_model_INCS)

test_model_LDADD = 	../../../contrib/hash/src/lib_vsid_hash.a \
					../../common/src/lib_vsid_common.a \
					../src/lib_vsid_protocol_model.a \
					-lyaml-cpp \
					/usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.a
