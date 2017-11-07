#
include $(CACHE)/mak/platform.mak
include $(CACHE)/mak/database.mak


DEV    = -DRUN_SHM # -DDSTARRING_INTEGRATE


INCS   = -I$(CACHE)/incl -I$(CACHE)/incl/dbincl
DBLIB  = -L$(CACHE)/lib -ldbapi $(DBFLGS)
FLAGS += $(DBCPL)

SHM    = shm

TARGET = $(SHM)

all: $(TARGET)

OBJS = shm.o util.o log.o

$(SHM):  $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCS) $(FLAGS) $(DBLIB)

.c.o:
	$(CC) -o $@ $(INCS) $(FLAGS) $(DEV) -c $<

clean:
	rm -f $(TARGET) core *stackdump *.lis a.out *.o

#
