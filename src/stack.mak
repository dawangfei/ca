#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    = -DRUN_STACK


INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

STACK  = stack

TARGET = $(STACK)

all: $(TARGET)

OBJS = stack.o

$(STACK):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
