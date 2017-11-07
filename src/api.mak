#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    = -DRUN_API -DUSER -DENABLE_HIS #-DDC_DEBUG


INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

DBEXT  = $(CACHE)/lib/dbext.o

CLI    = cli

TARGET = $(CLI)

all: $(TARGET)

OBJS = config.o nt.o put.o api.o ext.o lock.o load.o mydb.o his.o query.o rs.o sec.o \
         sql.o where.o seg.o aux.o stack.o tree.o blk.o shm.o  \
         tm.o util.o log.o hash.o list.o

$(CLI):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(DBEXT) $(INCS) $(FLAGS) $(DBLIB)
.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
