#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    = -DRUN_PUT


INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

PUT    = put

TARGET = $(PUT)

all: $(TARGET)

OBJS = put.o sql.o seg.o sec.o tree.o blk.o shm.o util.o log.o hash.o list.o mydb.o

$(PUT):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
