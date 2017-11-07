include ${ESWTDIR}/etc/Make.defines

SQLOBJS_EXT= dbext.o
TARGET=${CACHE}/lib/${SQLOBJS_EXT}

all: ${TARGET}

dep:all

clean:
	@-rm -f ${TARGET} ${SQLOBJS_EXT}

${TARGET}: ${SQLOBJS_EXT}
	cp -f ${SQLOBJS_EXT} ${TARGET}

.SUFFIXES: .o

.c.o:
	${CC} ${DB2CFLGS} ${ESWTCFLGS} -DDB2 -c -I${ESWTDIR}/autoconf/include $<

