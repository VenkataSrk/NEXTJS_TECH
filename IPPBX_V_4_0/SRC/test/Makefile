# ///////////////////////////////////////////////////////////////////////////
# 
#            Copyright 2020 by Vectone Mobile.
#            All rights reserved.
# 
# ///////////////////////////////////////////////////////////////////////////

topdir = ..
CPP   = g++ -g
LIBPATH   = $(topdir)/lib
INCLUDE =  -I $(topdir)/common/include -I $(topdir)/sip/include -I /usr/local/include/sofia-sip-1.12

OBJS    =

SRC    =

all : TestApp

#Comple:
#
TestApp: TestApp.cpp
	$(CPP) $(INCLUDE) -o $@ $< $(OBJS) -L $(LIBPATH) -lxgate_common -lxgatesip -luuid -lACE -lsofia-sip-ua -Wl,-rpath=$(topdir)/lib


allclean:
	rm -f TestApp
	make clean
clean:
	rm -f *.o
	rm -f core*

