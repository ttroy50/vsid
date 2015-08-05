# what flags you want to pass to the compiler & linker
#CFLAGS = --pedantic -Wall -O2
#AM_LDFLAGS =

noinst_LIBRARIES = lib_vsid_protocol_model.a

lib_vsid_protocol_model_a_INCS = \
								-I/usr/local/include \
								-I$(srcdir)/../inc \
								-I$(srcdir)/../../common/inc \
								-I$(top_srcdir)/contrib/easyloggingpp

AM_CFLAGS = --pedantic -Wall -O2 $(lib_vsid_protocol_model_a_INCS)
AM_LDFLAGS =
AM_CXXFLAGS =  -std=c++11 -DELPP_THREAD_SAFE -pthread $(lib_vsid_protocol_model_a_INCS)

lib_vsid_protocol_model_a_SOURCES = ProtocolModelDb.cpp \
									ProtocolModel.cpp \
									AttributeMeter.cpp \
									AttributeMeterFactory.cpp \
									ByteFrequency.cpp \
									DirectionBytesCountMeter.cpp \
									ByteFrequencyFirstOrigToDestPacket.cpp \
									First4ByteFrequencyMeter.cpp \
									First16ByteFrequencyMeter.cpp \
									DirectionChangesMeter.cpp \
									ByteFrequencyFirstDestToOrigPacket.cpp \
									DirectionBytesCountFirst10PacketMeter.cpp \
									ByteFrequencyFirst8PacketsMeter.cpp \
									DirectionChangesFirst8PacketsMeter.cpp \
									RtmpRegexMatchMeter.cpp \
									EntropyFirstOrigToDestPacket.cpp \
									ActionReactionFirst3ByteHashMeter.cpp \
									PayloadSizeFirstPacketMeter.cpp \
									FirstBitPositionsMeter.cpp
 
#lib_vsid_protocol_model_a_INCS = \
#								-I/usr/local/include \
#								-I$(srcdir)/../inc \
#								-I$(top_srcdir)/contrib/easyloggingpp \
#								-I$(top_srcdir)/contrib/hash/inc

#lib_vsid_protocol_model_a_LDDADD = 	-lyaml-cpp 


