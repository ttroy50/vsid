# what flags you want to pass to the compiler & linker
#CFLAGS = --pedantic -Wall -O2
#AM_LDFLAGS =

noinst_LIBRARIES = lib_vsid_common.a

lib_vsid_common_a_INCS = \
								-I/usr/local/include \
								-I$(srcdir)/../inc \
								-I$(top_srcdir)/contrib/easyloggingpp \
								-I$(top_srcdir)/contrib/hash/inc

AM_CFLAGS = --pedantic -Wall -O2 $(lib_vsid_common_a_INCS)
AM_LDFLAGS =
AM_CXXFLAGS =  -std=c++11 -DELPP_THREAD_SAFE $(lib_vsid_common_a_INCS)

lib_vsid_common_a_SOURCES = Flow.cpp \
							FlowManager.cpp \
							IPv4.cpp
							
													