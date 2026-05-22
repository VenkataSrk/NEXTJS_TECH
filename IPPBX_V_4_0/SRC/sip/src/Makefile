# ///////////////////////////////////////////////////////////////////////////
# 
#            Copyright 2020 by Vectone Mobile.
#            All rights reserved.
# 
# ///////////////////////////////////////////////////////////////////////////

top_dir = ../..

include $(top_dir)/Makefile.conf

XGATESIPLIB = $(top_dir)/lib/libxgatesip.so

SRC   =  SipHandler.cpp

CFLAGS += -I ../include -I $(top_dir)/common/include -I /usr/local/include/sofia-sip-1.12

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

