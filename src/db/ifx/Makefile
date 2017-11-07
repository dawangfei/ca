include ${ESWTDIR}/etc/Make.defines


VERSION_IFX=`esql -V | grep Version | awk '{print $$3}'`

SQLOBJS_IFX= ifxsqldyn.o ifxsqlsf.o ifxdbapi.o
TARGET=STATICLIB_IFX

all:
	ln -fs ../dbapi.c ifxdbapi.ec
	@if [ X${INFORMIXDIR} != X -a -d ${INFORMIXDIR}/ ]; \
	then \
		make "STATICLIB_IFX=${LIBDIR}/libdbapi_ifx.a.${VERSION_IFX}" ${TARGET}; \
	else \
		echo "environment INFOMIXDIR error, escape make INFORMIX driver!!!"; \
	fi
dep:all

clean:
	@-rm -f ${CLEANFILES} \
		ifxsqldyn.c ifxsqlsf.c \
		${LIBDIR}/libdbapi_*

STATICLIB_IFX: ${SQLOBJS_IFX}
	${AR} ${AROPT} ${STATICLIB_IFX} ${SQLOBJS_IFX}
	${RANLIB} ${STATICLIB_IFX}

.SUFFIXES: .ec .o

.ec.o:
	${ESQL} ${ESWTECFLGS} -g -c -I${ESWTDIR}/autoconf/include $<
	rm ${<:.ec=.c}

.c.o:
	${CC} ${IFXCFLGS} ${ESWTCFLGS} -c -I${ESWTDIR}/autoconf/include $<

