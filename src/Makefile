#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak

INCD   = $(CACHE)/incl
INCS   = -I$(INCD) -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL) -DENABLE_HIS

SPECIAL= # -DSTARRING_INTEGRATE

DBEXT  = $(CACHE)/lib/dbext.o
CA     = $(CACHE)/lib/libca.a 
TARGET = $(CA)

all: $(TARGET)

OBJS = config.o nt.o aux.o ext.o his.o load.o \
       object.o put.o rs.o seg.o api.o blk.o \
       query.o sec.o shm.o sql.o \
       where.o tree.o stack.o \
       hash.o list.o lock.o tm.o util.o log.o

OTHS = mydb.o

$(CA):  $(OBJS) $(OTHS)
	$(SLINK) $(SLFLG) $@  $(OBJS) $(DBEXT)
	@cp -f mydb.o $(CACHE)/lib
	@cp -f api.h $(INCD)/caapi.h
	@cp -f aux.h $(INCD)
	@cp -f log.h $(INCD)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(SPECIAL) -c $<

clean:
	rm -f $(TARGET) core $(OBJS) # $(INCD)/caapi.h $(INCD)/aux.h $(INCD)/log.h

#
