include ${ESWTDIR}/etc/Make.defines

SQLOBJS_NODB= nodbdbapi.o
STATICLIB=${ESWTDIR}/lib/libdbapi.a

TARGET=STATICLIB_NODB

all:
	ln -fs ../dbapi.c nodbdbapi.c
	@make "STATICLIB_NODB=${LIBDIR}/libdbapi_nodb.a" ${TARGET}
dep:all

clean:
	@-rm -f ${CLEANFILES} \
		${LIBDIR}/libdbapi_*

STATICLIB_NODB: ${SQLOBJS_NODB}
	${AR} ${AROPT} ${STATICLIB_NODB} ${SQLOBJS_NODB}
	${RANLIB} ${STATICLIB_NODB}
	@-if [ ! -L ${ESWTDIR}/lib/libdbapi.a ]; \
		then  ln -s ${STATICLIB_NODB} ${ESWTDIR}/lib/libdbapi.a; \
	fi

.SUFFIXES: .c .o

.c.o:
	${CC} ${NODBCFLGS} ${ESWTCFLGS} -c -I${ESWTDIR}/autoconf/include -c $<
