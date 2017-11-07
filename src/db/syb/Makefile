include ${ESWTDIR}/etc/Make.defines

KRC_FLAGS= -C KR_C

VERSION_SYB=`${CPRE} -v|grep "ESQL/C" | awk '{print $$3}' | awk -F '/' '{print $$2}'`

SQLOBJS_SYB= sybesql.o sybsqldyn.o sybsqlsf.o sybdbapi.o
TARGET=STATICLIB_SYB

all:
	ln -fs ../dbapi.c sybdbapi.cp
	@if [ X${SYBASE} != X -a -d ${SYBASE}/ ]; \
	then \
		make "STATICLIB_SYB=${LIBDIR}/libdbapi_syb.a.${VERSION_SYB}" ${TARGET}; \
	else \
		echo "environment SYBASE error, escape make SYBASE driver!!!"; \
        fi

dep:all

clean:
	@-rm -f ${CLEANFILES} \
		sybdbapi.c sybsqldyn.c sybsqlsf.c ${SQLOBJS_SYB} \
		${LIBDIR}/libdbapi_*

sybesql.o: ${SYBASE}/${SYBASE_OCS}/include/sybesql.c
	${CC} ${SYBCFLGS} ${SRCCFLGS} -c ${SYBASE}/${SYBASE_OCS}/include/sybesql.c

STATICLIB_SYB: ${SQLOBJS_SYB}
	${AR} ${AROPT} ${STATICLIB_SYB} ${SQLOBJS_SYB}
	${RANLIB} ${STATICLIB_SYB}

.SUFFIXES: .cp .o

.cp.o:
	case $${SYBPLATFORM} in \
		sun4) \
			PRECOMPFLAGS="$(KRC_FLAGS) -m" export PRECOMPFLAGS ;; \
		rs6000) \
			PRECOMPFLAGS="-l -m" export PRECOMPFLAGS ;; \
		*) \
			PRECOMPFLAGS="-m" export PRECOMPFLAGS ;; \
	esac ; \
	unset LANG;${CPRE} $${PRECOMPFLAGS} $< ;\
	case $< in \
		sybsqldyn\.cp) \
			cat sybsqldyn.c|sed "s/1234567890/h_ques_num/g" >err;mv err sybsqldyn.c;; \
	esac
	${CC} ${SYBCFLGS} ${ESWTCFLGS} -c -I${ESWTDIR}/autoconf/include $*.c
	@-rm ${<:.cp=.c}

.c.o:
	${CC} ${SYBCFLGS} ${ESWTCFLGS} -c -I${ESWTDIR}/autoconf/include $<

