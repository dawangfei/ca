include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak

PC        = proc
ORACFLGS  = ${DBINCP} -DORACLE
BASEINCL  = ${CACHE}/incl/dbincl
PCFLGS    = mode=ansi parse=none code=ansi_c lines=yes include=${BASEINCL} close_on_commit=no

VERSION_ORA=`proc | grep Release | awk '{print $$3}'`

SQLOBJS_ORA= orasqldyn.o orasqlsf.o oradbapi.o
TARGET=STATICLIB_ORA

all:
	ln -fs ../dbapi.c oradbapi.pc
	@if [ X${ORACLE_HOME} != X -a -d ${ORACLE_HOME}/ ]; \
	then \
		make "STATICLIB_ORA=${CACHE}/lib/libdbapi_ora.a.${VERSION_ORA}" ${TARGET}; \
	else \
		echo "environment ORACLE_HOME error, escape make ORACLE driver!!!"; \
	fi
dep:all

clean:
	@-rm -f ${CLEANFILES} tp* \
		oradbapi.c orasqldyn.c orasqlsf.c \
		oradbapi.lis orasqldyn.lis orasqlsf.lis \
		${LIBDIR}/libdbapi_*

STATICLIB_ORA: ${SQLOBJS_ORA}
	${SLINK} ${SLFLG} ${STATICLIB_ORA} ${SQLOBJS_ORA}
	${RANLIB} ${STATICLIB_ORA}

ora: ora.o
	$(CC) -o $@ $+ $(INCS) $(LIBS) $(FLAGS) -ldbapi  $(DBFLGS)

.SUFFIXES: .pc .o

.pc.o:
	${PC} ${PCFLGS} iname=$<
	${CC} ${CCFLG} ${ORACFLGS} -I${BASEINCL}  -c $*.c
	rm -rf $*.c $*.lis

.c.o:
	${CC} ${CCFLG} ${ORACFLGS} -I${BASEINCL}  -c $*.c


# Makefile
