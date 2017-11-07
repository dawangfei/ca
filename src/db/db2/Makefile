include ${ESWTDIR}/etc/Make.defines

VERSION_DB2=`db2level|grep Informational|awk '{print $$5}' | awk -F '\"' '{print $$1}'`


SQLOBJS_DB2= db2sqldyn2.o db2sqlsf2.o db2dbapi.o
TARGET=STATICLIB_DB2

all:
	ln -fs ../dbapi.c db2dbapi.c
	@if [ X${DB2PATH} != X -a -d ${DB2PATH}/ ]; \
	then \
		make "STATICLIB_DB2=${CACHE}/lib/libdbapi_db2.a.${VERSION_DB2}" ${TARGET}; \
	else \
		echo "environment DB2PATH error, escape make DB2 driver!!!"; \
	fi
dep:all

clean:
	@-rm -f ${CLEANFILES} \
		db2sqldyn2.c db2sqlsf2.c
#		${LIBDIR}/libdbapi_*

STATICLIB_DB2: ${SQLOBJS_DB2}
	${AR} ${AROPT} ${STATICLIB_DB2} ${SQLOBJS_DB2}
	${RANLIB} ${STATICLIB_DB2}

.SUFFIXES: .sqc .o

.sqc.o:
	${EMBCONN};${EMBPREP} $< bindfile using ${<:.sqc=.bnd} datetime iso DYNAMICRULES RUN VERSION DCCACHE;${EMBBIND} ${<:.sqc=.bnd};${EMBDISC}
	${CC} ${DB2CFLGS} ${ESWTCFLGS} -c -I${ESWTDIR}/autoconf/include $*.c
	@-rm ${<:.sqc=.c}
	@-mv ${<:.sqc=.bnd} ${ESWTDIR}/lib/; cp ${ESWTDIR}/etc/starbnd.lst ${ESWTDIR}/lib
#	${EMBCONN};${EMBPREP} $< ;${EMBDISC}
#	${EMBCONN};${EMBPREP} $< connect 2;${EMBDISC}

.c.o:
	${CC} ${DB2CFLGS} ${ESWTCFLGS} -c -I${ESWTDIR}/autoconf/include $<
