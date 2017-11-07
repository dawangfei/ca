#|----------------------------------------------------------------------------|
#|                           TOPMake 2.1                                      |
#| Copyright (c) 2000-2005 Shanghai Huateng Software Systems Co., Ltd.        |
#|    All Rights Reserved                                                     |
#|----------------------------------------------------------------------------|
#| FILE NAME    : mksstand.mak                                                |
#| DESCRIPTIONS : Makefile for all kind of platform, for c and pc file,       |
#|                for lib(dynamic & static) and execuatable object            |
#|----------------------------------------------------------------------------|

include $(MKHOME)/mak/platform.mak
include $(MKHOME)/mak/database.mak

# environment variables must be define outside of makefile
#
# $MKHOME : home directory of application 
# $CCFLG  : compiler options for c source code
# $CXXFLG : compiler options for c++ source code
# $DLFLG  : link options for dynamic library
# $SLFLG  : link options for static library
# $EXFLG  : link options for excecuteable object
# $DBFLG  : debug mode options
# $OPFLG  : optimize mode options
# $DLFIX  : suffix of dynamic library
# $SLFIX  : suffix of static library

# $OBJS   : c object files list
# $LIBS   : libs required for executable object
# $DEFS   : define flag list
# $TARGET : target objetc name

# $TUXSVCS: tuxedo services name
# $TUXRSMN: tuxedo resource manager name

# $DBNM   : db name (for db2)

# commands
 
PC		=	proc
EC		=	esql
DB2		=	db2

JAVAC   =   javac
JAVAH   =   javah
RMIC    =   rmic

RM		=	rm -f
CP		=	cp
MV		=   mv
CD		=   cd
ECHO	=	@echo

# paths and flags

SYSINCP	=	/usr/include
SYSLIBP	=	/usr/lib/hpux64
JINCP	=	$(JAVA_HOME)/include
JMDINCP	=	$(JAVA_HOME)/include/hp-ux

SRCP	=	$(MKHOME)/source
BINP	= 	$(MKHOME)/bin
LIBP	=	$(MKHOME)/lib
GNULIBP =   $(MKHOME)/gnu/lib
HSMLIBP =   $(HSMHOME)/lib
INCP	=	$(MKHOME)/include
GNUINCP =   $(MKHOME)/gnu/include

INCPS	=	-I$(SYSINCP) -I$(INCP) -I$(GNUINCP) -I$(DBINCP) -I.
LIBPS	=	-L$(SYSLIBP) -L$(LIBP) -L$(GNULIBP) $(DBLIBP)
 
CCFLGS	=	$(CCFLG) $(DEFS) $(DBDEF) $(OSDEF) $(PROCDEF) $(BITDEF)  $(INCPS)  $(NETFLG)
CXXFLGS =   $(CXXFLG) $(DEFS) $(TUXDEF) $(DBDEF) $(OSDEF) $(PROCDEF) $(BITDEF) $(INCPS)
DLFLGS	=	$(DLFLG) $(LIBPS)
EXFLGS	=	$(EXFLG) $(LIBPS)
OPFLGS  =   $(DBFLG)
 
DLTARG	= $(LIBP)/lib$(TARGET)$(DLFIX)
SLTARG	= $(LIBP)/lib$(TARGET)$(SLFIX)
EXTARG	= $(BINP)/$(TARGET)
TUXTARG	= $(TUXBINP)/$(TARGET)

debugdynamic: setdebug $(DLTARG)
releasedynamic: setrelease $(DLTARG)
debugstatic: setdebug $(SLTARG)
releasestatic: setrelease $(SLTARG)
debugexec: setdebug $(EXTARG)
releaseexec : setrelease $(EXTARG)

javaclasses : $(OBJS)
	$(ECHO) "Finish java package [$(TARGET)]"
rmi :
	$(ECHO) "rmic [$(TARGET)] ..."
	@$(CD) `dirname $(TARGET)`; \
	$(RMIC) `basename $(TARGET)`

setdebug:

setrelease:

$(SLTARG): $(OBJS) 
	$(ECHO) "Linking   [$(SLTARG)] ..."
	@$(SLINK) $(SLFLG) $@ $(OBJS)

$(DLTARG): $(OBJS) 
	$(ECHO) "Linking   [$(DLTARG)] ..."
	@$(RM) $@
	@if [ $(OSTYPE) = aix ]; \
	then \
		$(DLINK) -o $@ $(DLFLGS) $(OBJS) $(LIBS) $(UXLIBS); \
	else \
		$(DLINK) -o `basename $@` $(DLFLGS) $(OBJS) $(LIBS); \
		$(MV) `basename $@` `dirname $@`; \
	fi

$(EXTARG): $(OBJS)
	$(ECHO) "Linking [$(EXTARG)]..."
	@$(CC) -o $@  $(OBJS) $(EXFLGS) $(LIBS)

$(TUXTARG): $(OBJS)
	$(ECHO) "buildserver [$(TUXTARG)] ..."
	@CFLAGS="$(EXFLGS)" \
	$(TUXBS) -o $@ -f "$(OBJS)" $(TUXSVCS) -f "$(LIBS)" $(TUXRSMN)

# implicit
 
.SUFFIXES:
.SUFFIXES: .java .class .xc .ec .pc .sqC .sqc .cp .cpp .c .o
 
.xc.o:
	$(ECHO) "Compiling [$@] ..."
	@$(RM) $*$(DBFIX)
	@$(CP) $*.xc $*$(DBFIX)
	@if [ $(DBTYPE) = ora ]; \
	then \
		$(PC) lines=yes define=$(DBD) include=$(TUXINCP) include=$(INCP) include=$(JINCP) include=$(JMDINCP) include=`dirname $@` code=ANSI_C iname=$*$(DBFIX); \
		$(CC) -o $@ $(OPFLGS) $(CCFLGS) -c $*.c; \
		$(RM) $*.lis; \
	else \
		$(EC) -e -ED$(DBD) $(INCPS) $*$(DBFIX); \
		$(CC) -o $@ $(OPFLGS) $(CCFLGS) -c `basename $*`.c; \
		$(RM) `basename $*`.c; \
	fi
	@$(RM) $*$(DBFIX) $*.c

.pc.o:
	$(ECHO) "Compiling [$@] ..."
	@$(PC) lines=yes define=$(DBD) include=$(TUXINCP) include=$(INCP) include=$(JINCP) include=$(JMDINCP) include=`dirname $@` code=ANSI_C iname=$*$(DBFIX)
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ $(OPFLGS) $(CCFLGS) -c $*.c
	@$(RM) $*.lis $*.c

.ec.o:
	$(ECHO) "Compiling [$@] ..."
	@$(EC) -e -ED$(DBD) $(INCPS) $*$(DBFIX)
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ $(OPFLGS) $(CCFLGS) -c `basename $*`.c; \
	$(RM) `basename $*`.c

.sqc.o:
	$(ECHO) "Compiling [$@] ..."
	@$(RM) $*.bnd
	@$(DB2) connect to $(DBNM)
	@DB2INCLUDE=$(INCP) \
	$(DB2) prep $< bindfile
	@$(DB2) bind $*.bnd
	@$(DB2) connect reset
	@$(DB2) terminate
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ $(OPFLGS) $(CCFLGS) -c `basename $*`.c
	@$(RM) $*.bnd $*.c

.sqC.o:
	$(ECHO) "Compiling [$@] ..."
	@$(RM) $*.bnd
	@$(DB2) connect to $(DBNM)
	@DB2INCLUDE=$(INCP) \
	$(DB2) prep $< bindfile
	@$(DB2) bind $*.bnd
	@$(DB2) connect reset
	@$(DB2) terminate
	@$(CD) `dirname $@` ; \
	$(CXX) -o $@ $(OPFLGS) $(CCFLGS) -c `basename $*`.C
	@$(RM) $*.bnd $*.C

.cp.o:
	$(ECHO) "Compiling [$@] ..."
	$(CPRE) -y $(INCPS) -O $*.c $*.cp
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ $(OPFLGS) $(CCFLGS) -c `basename $*`.c
	@$(RM) $*.c

.cpp.o:
	$(ECHO) "Compiling [$@] ..."
	@$(CD) `dirname $@` ; \
	$(CXX) -o $@ $(OPFLGS) $(CXXFLGS) -c `basename $<`

.c.o:
	$(ECHO) "Compiling [$@] ..."
	@$(CD) `dirname $@`; \
	$(CC) -o $@ $(OPFLGS) $(CCFLGS) -c `basename $<`
 
.java.class:
	$(ECHO) "Compiling [$@] ..."
	@$(CD) `dirname $@`; \
	$(JAVAC) `basename $<`

# clearing object codes
 
clean: cleanup
	@$(RM) $(SLTARG) $(DLTARG) $(EXTARG) $(TUXTARG)
 
cleanup:
	@$(RM) $(OBJS)
