# README
# AIX6 + DB2-9.7

##################################################################
STEP1: ${HOME}/.profile
add and enable it
CACHE=${HOME}/cache; export CACHE
PATH=$PATH:${CACHE}/bin:${CACHE}/sbin; export PATH

##################################################################
STEP2: Compile and get library
cd ${CACHE}/mak        && ln -sf database.mak.db2 database.mak
cd ${CACHE}/mak        && ln -sf platform.mak.AIX6 platform.mak
cd ${CACHE}/src/db/ora && make clean all
cd ${CACHE}/src/db     && make -f dbext.mak.ora clean all
cd ${CACHE}/src        && make clean all

##################################################################
STEP3: get admin client
cd ${CACHE}/lib        && ln -sf libdbapi_db2.a.v9.7.0.10  libdbapi.a
cd ${CACHE}/tool       && make clean all

##################################################################
FINAL: get objects as:
${CACHE}/lib/libca.a
${CACHE}/incl/api.h
${CACHE}/bin/adm

##################################################################
# README
