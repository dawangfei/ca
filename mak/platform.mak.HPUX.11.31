#|----------------------------------------------------------------------------|
#|                           TOPMake 2.0                                      |
#| Copyright (c) 2000-2003 Shanghai Huateng Software Systems Co., Ltd.        |
#|    All Rights Reserved                                                     |
#|----------------------------------------------------------------------------|
#| FILE NAME     : platform.hp.mak                                           |
#|----------------------------------------------------------------------------|

SHELL = /bin/sh

OS    = $(shell uname -s)

CC    = gcc
CXX   = xCC
DLINK = ld
SLINK = ar
DLFLG = -b +s
SLFLG = -ruc
CCFLG = -mlp64 -D_REENTRANT -Wall
CXXFLG= 
DBFLG = -g
OPFLG = -O0
MKFLG = -j 4
DLFIX = .so
SLFIX = .a
UXLIBS = -lm -lc
DLLIBS = -ldl
NETLIBS = -lnsl
CURLIBS = -lcurses
CRYLIBS =
THREADLIBS = -lpthreads
XLIBS   = -lXext -lX11
XLIBP   = /usr/lib/X11R6
BITDEF  = -DBIT64
RANLIB  = ranlib
CLEANFILES=core *.core core.* *.o


#
CP     = cp -f
BINP   = $(CACHE)/bin
LIBP   = $(CACHE)/lib
INCS   = -I$(GNUHOME)/include -I$(CACHE)/incl -I.
LIBS   = -L$(GNUHOME)/lib     -L$(CACHE)/lib -lm -L.
FLAGS  = $(CCFLG) $(DBFLG)

# platform.mak
