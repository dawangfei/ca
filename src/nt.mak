#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak

DEV    = -DRUN_NET

INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

NET    = nt

TARGET = $(NET)

all: $(TARGET)

OBJS = nt.o put.o sql.o seg.o sec.o tree.o blk.o shm.o util.o log.o hash.o list.o mydb.o

$(NET):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
