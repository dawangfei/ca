#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    = -DRUN_LOAD


INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

DBEXT  = $(CACHE)/lib/dbext.o
LOAD    = load

TARGET = $(LOAD)

all: $(TARGET)

OBJS = load.o put.o ext.o aux.o mydb.o seg.o sec.o tree.o blk.o shm.o util.o log.o list.o tm.o

$(LOAD):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(DBEXT) $(INCS) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
