

noinst_LIBRARIES=lib_vsid_hash.a 


lib_vsid_hash_a_INCS = -I$(srcdir)/../inc

CC=g++
# what flags you want to pass to the compiler & linker
AM_CFLAGS = --pedantic -Wall -O2 $(lib_vsid_hash_a_INCS)
AM_LDFLAGS =
AM_CXXFLAGS =  -std=c++11 $(lib_vsid_hash_a_INCS)

lib_vsid_hash_a_SOURCES = murmur3.c
