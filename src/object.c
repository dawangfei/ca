#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "log.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "load.h"
#include "query.h"

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"



/* ================================================================= */


/* ================================================================= 
   ================================================================= */




#if RUN_OBJECT
int main(int argc, char *argv[])
{
    int  rv  = 0;

    olog("object begin");


    olog("object end");
    return rv;
}
#endif

/* object.c */
