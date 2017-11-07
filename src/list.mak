#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    = -DRUN_LST


INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

LST    = lst

TARGET = $(LST)

all: $(TARGET)

OBJS = list.o tm.o util.o log.o hash.o

$(LST):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
