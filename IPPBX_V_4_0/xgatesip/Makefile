CC = g++ -std=c++0x -lrt -lm

CFLAGS = -g -DNDEBUG

INCLUDES = -I. -I../xgate -I../xgate_reactor -I./ -I../xgatecontroller -I./xgate_parser -I./xgate_xmlparser -I/usr/include  -I/usr/local/include -I/usr/local/include/sofia-sip-1.12  -I/usr/local/aculab/v6/TiNG -I/usr/local/aculab/v6/TiNG/apilib -I/usr/local/aculab/v6/TiNG/apilib/POSIX -I/usr/local/aculab/v6/include -I/usr/include/netinet -I./pwlib/include -I./pwlib/include/ptlib -I./pwlib/include/ptclib -I./slsip/pchdir  -I/usr/local/aculab/v6/include -I/usr/local/aculab/v6/TiNG/highapi -I/usr/local/aculab/v6/TiNG/include -I./slsip/include -I./AudioCodes/voiplib -I./AudioCodes.v46/voiplib.v46/include -I./slsip/jrtplib/ -I/usr/local/include/curl -I/usr/include/curl

LFLAGS = -L/usr/local/lib -L$(ACE_ROOT)/ace

LIBS = -lsofia-sip-ua -lpthread -lpt -lcurl -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lgmodule-2.0 -lACE


SRCS = ./xgate_parser/xGateRapidXML.cpp \
       ./xgate_parser/xGateXMLParser.cpp \
       ./xgate_xmlparser/xGateRSXMLParserUtil.cpp \
       ./xgate_xmlparser/xGateRSXMLParser.cpp \
       SofiaSiphandler.cpp \
       xGateBusinessHandler.cpp \
       xGateMultiConference.cpp \
       xGateSipRecording.cpp \
       xGateSipSignalDispatcher.cpp \
       xGateSipSignalProcessor.cpp \
       xGateSipSignalReceiver.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(OBJS)
	@echo libxGateSip.so has been built

libxGateSip.so: $(OBJS)
	$(CC) $< $(LFLAGS) $(INCLUDES) -fPIC -shared -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf *.o
	rm -rf libxGateSip.so
