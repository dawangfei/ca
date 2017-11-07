#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak

DEV    =

INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
LIBP   = $(DCHOME)/lib 
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)
CALIB  = -L$(CACHE)/lib -lca

LEAK   = leak

TARGET = $(LEAK)

all: $(TARGET)

OBJS = leak.o $(CACHE)/lib/mydb.o

$(LEAK):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) -I$(CACHE)/incl/dbincl $(CALIB) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o ben*log

#
