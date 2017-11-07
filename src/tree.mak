#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    = -DRUN_RBTREE


INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

TREE    = tree

TARGET = $(TREE)

all: $(TARGET)

OBJS = tree.o util.o log.o

$(TREE):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
