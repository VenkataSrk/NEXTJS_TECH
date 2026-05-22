# ///////////////////////////////////////////////////////////////////////////
# 
#            Copyright 2020 by Vectone Mobile.
#            All rights reserved.
# 
# ///////////////////////////////////////////////////////////////////////////

top_dir = ../..

include $(top_dir)/Makefile.conf

XGATESIPLIB = $(top_dir)/lib/libxgate_common.so

SRC   =  SipLogger.cpp \
	 SrcIdGen.cpp

CFLAGS += -I ../include

OBJS  =  $(patsubst %.cpp, %.o, $(SRC))

%.o: %.cpp
	$(CPP) -fPIC -shared  $(CFLAGS) -c -o $@ $<

$(XGATESIPLIB): $(OBJS)
	$(CPP) -shared -o $@ $(OBJS)

dep: $(SRC)
	$(DEPEND) $(CFLAGS) $(SRC) > Makefile.dep

rmlib:
	rm $(XGATESIPLIB)

clean:
	rm -f *.o $(XGATESIPLIB)

include Makefile.dep

