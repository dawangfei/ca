#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    =

BINP   = $(CACHE)/bin
INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)
CALIB  = -L$(CACHE)/lib -lca

ADM    = $(BINP)/caadm
SYNC   = $(BINP)/casync
LSEN   = $(BINP)/listen
WORK   = $(BINP)/caworker

TARGET = $(ADM) $(SYNC) $(LSEN) $(WORK)

all: $(TARGET)

OBJS = caadmin.o $(CACHE)/lib/mydb.o

$(ADM):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) $(CALIB) $(FLAGS) $(DBLIB)

SYNC_OBJS = casync.o $(CACHE)/lib/mydb.o
$(SYNC): $(SYNC_OBJS)
	$(CC) -o $@ $(SYNC_OBJS) $(INCS) $(CALIB) $(FLAGS) $(DBLIB)

LSEN_OBJS = calisten.o $(CACHE)/lib/mydb.o
$(LSEN): $(LSEN_OBJS)
	$(CC) -o $@ $(LSEN_OBJS) $(INCS) $(CALIB) $(FLAGS) $(DBLIB)

WORK_OBJS = caworker.o $(CACHE)/lib/mydb.o
$(WORK): $(WORK_OBJS)
	$(CC) -o $@ $(WORK_OBJS) $(INCS) $(CALIB) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
