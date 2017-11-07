#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "log.h"
#include "shm.h"


/* ================================================================= */

typedef struct _shm_header_t {
    long gate;                  /* control: 0, 1, 2 */
    int  ready;                 /* 0: not, 1: ready, 2: discard */
    int  sem_id;                /* mutext lock */
    long shm_size;              /* total size */
    long block_size;            /* single block size */
    long offset_a;              /* A offset of shared memory */
    long offset_b;              /* B offset of shared memory */
    void *init_addr;            /* head address, MUST be same to all attached process */
} shm_header_t;

static void *_g_shm_addr = NULL;

/* ================================================================= */


void dc_shm_debug(void *addr);

/*
 *  Function:  dc_shm_init
 *      To create a shared memory,
 *          or attach a shm
 *      Warn: a lock must be hold if to create!
 *
 *  Parameters:
 *      shm_key   - ipc key
 *      shm_size  - the size of full shared memory, without shm_header_t
 *      shm_addr  - the address of shared memory,
 *                  both input and output
 *      flag      - CREATE or 0
 *
 *  Return Value:
 *      SUCCESS - shmid
 *      FAILURE - failure
 */
int dc_shm_init(long shm_key, long shm_size, void **shm_addr, long flag, int *errcode)
{
    int shm_id;
    int new_create = 0;
    void *input_addr = NULL;
    shm_header_t  *shmHeader = NULL;

    _g_shm_addr = NULL;

    if ((shm_id = shmget((key_t)shm_key, 0, 0)) == -1)
    {
        /*
        olog("to create a shm[%d]", shm_key);
        */
        /* Fail to get shared memory identifier */
        if (errno != ENOENT) {
            elog("error: shmget: <%d>,<%s>", errno, strerror(errno));
            *errcode = -4;
            return FAILURE;
        }

        /* whether to create */
        if (!(flag & SHM_CREATE)) {
            elog("error: key %ld not exists", shm_key);
            *errcode = -5;
            return FAILURE;
        }

        /* olog("to create a shm[%d]", shm_key); */
        /* Create shared memory */
        if ((shm_id = shmget((key_t)shm_key, 
                        sizeof(shm_header_t) + shm_size,
                        0644 | IPC_CREAT)) == -1) {
            *errcode = -6;
            return FAILURE;
        }
        new_create = 1;
    }
    else
    {
        /* to create but already exist */
        if (flag & SHM_CREATE)
        {
            elog("CREATE flag specified but already exist!");
            *errcode = -8;
            return FAILURE;
        }
    }

    input_addr = *shm_addr;
    if ((*shm_addr = shmat(shm_id, *shm_addr, 0)) == (void *)-1)
    {
        *errcode = -7;
        elog("error: shmat: <%d>,<%s>,<%p>", errno, strerror(errno), input_addr);
        return FAILURE;
    }

    _g_shm_addr = *shm_addr;

    if (new_create)
    {
#if DC_DEBUG
        olog("is new created, let's initialize size <%d>", shm_size);
#endif
        /* initialize new create shared memory block */
        shmHeader = (shm_header_t *)*shm_addr;
        memset(shmHeader, 0, sizeof(shm_header_t)+shm_size);
        shmHeader->gate = 0;
        shmHeader->ready = 0;
        shmHeader->shm_size   = shm_size;
        shmHeader->block_size = DC_FLOOR_ALIGN(shm_size, (DC_WORD_WIDTH*2))/2;
        shmHeader->init_addr  = *shm_addr;
        shmHeader->offset_a   = sizeof(shm_header_t);
        shmHeader->offset_b   = shmHeader->offset_a + shmHeader->block_size;
    }
    else
    {
        /* XXX: check 'init_addr' equals 'shm_addr' */
        shmHeader = (shm_header_t *)*shm_addr;
        if (shmHeader->init_addr != *shm_addr)
        {
#if DC_DEBUG
            elog("warn: the address expect:[%p], but got:[%p]!", shmHeader->init_addr, *shm_addr);
#endif
            *shm_addr = shmHeader->init_addr;
            return -1;
        }
        else
        {
#if DC_DEBUG
            olog("nice: head address is the same as creator.");
#endif
        }
    }

    return shm_id;
} /* dc_shm_init */


/*
 *  Function:  
 *      To detach and delete a shared memory,
 *      the shared-memory is already created!
 *
 *  Parameters:
 *      id      - the id of shared memory
 *      addr    - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - the index of segments
 *      FAILURE - failure
 */
int dc_shm_term(int shm_id, void *shm_addr)
{
    if (shm_addr != NULL)
    {
        if (shmdt(shm_addr) != 0) 
        {
            elog("error: shmdt: <%d>,<%s>", errno, strerror(errno));
            return FAILURE;
        }
        _g_shm_addr = NULL;
    }

    if (shm_id >= 0)
    {
        if (shmctl(shm_id, IPC_RMID, NULL) != 0)
        {
            elog("error: shmctl: <%d>,<%s>", errno, strerror(errno));
            return FAILURE;
        }
    }

    return SUCCESS;
} /* dc_shm_term */


/*
 *  Function:  
 *      get the gate of shm
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - gate
 *      FAILURE - 
 */
long dc_shm_get_gate(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    shmHeader = (shm_header_t *)_shm_addr;

    return shmHeader->gate;
}


/*
 *  Function:  
 *      switch the gate of shm
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - gate
 *      FAILURE - 
 */
void dc_shm_switch_gate(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    shmHeader = (shm_header_t *)_shm_addr;

    if (shmHeader->gate == DC_SHM_REGION_DEFAULT || shmHeader->gate == DC_SHM_REGION_B)
    {
        shmHeader->gate = DC_SHM_REGION_A;
    }
    else if (shmHeader->gate == DC_SHM_REGION_A)
    {
        shmHeader->gate = DC_SHM_REGION_B;
    }
}


/*
 *  Function:  
 *      get the address of block-A
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - address
 *      FAILURE - NULL
 */
void *dc_shm_get_block_A_addr(void *_shm_addr)
{
    void *blk_addr = NULL;
    shm_header_t *shmHeader = NULL;

    shmHeader = _shm_addr;

    blk_addr = _shm_addr + shmHeader->offset_a;

    return blk_addr;
} /* dc_shm_get_block_A_addr */


/*
 *  Function:  
 *      get the address of block-B
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - address
 *      FAILURE - NULL
 */
void *dc_shm_get_block_B_addr(void *_shm_addr)
{
    void *blk_addr = NULL;
    shm_header_t *shmHeader = NULL;

    shmHeader = _shm_addr;

    blk_addr = _shm_addr + shmHeader->offset_b;

    return blk_addr;
} /* dc_shm_get_block_B_addr */


/*
 *  Function:  
 *      get the address of shm
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - address
 *      FAILURE - NULL
 */
void *dc_shm_get_addr()
{
    return _g_shm_addr;
} /* dc_shm_get_addr */


void dc_shm_set_addr()
{
    _g_shm_addr = NULL;
}


/*
 *  Function:  
 *      get block size
 *
 *  Parameters:
 *      _shm_addr - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - address
 *      FAILURE - NULL
 */
long dc_shm_get_block_size(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    if (_shm_addr == NULL)
    {
        elog("error: %s: invalid argument", __func__);
        return -1;
    }

    shmHeader = (shm_header_t *)_shm_addr;

    return shmHeader->block_size;
} /* dc_shm_get_block_size */


long dc_shm_calc_size(int _block_size)
{
    return sizeof(shm_header_t) + _block_size*2;
}


int  dc_shm_set_ready(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    if (_shm_addr == NULL)
    {
        elog("error: %s: invalid argument", __func__);
        return -1;
    }

    shmHeader = (shm_header_t *)_shm_addr;
    shmHeader->ready = 1;

    return 0;
}


int dc_shm_get_ready(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    if (_shm_addr == NULL)
    {
        elog("error: %s: invalid argument", __func__);
        return -1;
    }

    shmHeader = (shm_header_t *)_shm_addr;

    return shmHeader->ready;
}


int  dc_shm_set_discard(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    if (_shm_addr == NULL)
    {
        elog("error: %s: invalid argument", __func__);
        return -1;
    }

    shmHeader = (shm_header_t *)_shm_addr;
    shmHeader->ready = 2;

    return 0;
}


int  dc_shm_set_semid(void *_shm_addr, int _sem_id)
{
    shm_header_t *shmHeader = NULL;

    if (_shm_addr == NULL)
    {
        elog("error: %s: invalid argument", __func__);
        return -1;
    }

    shmHeader = (shm_header_t *)_shm_addr;
    shmHeader->sem_id= _sem_id;

    return 0;
}


int dc_shm_get_semid(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    if (_shm_addr == NULL)
    {
        elog("error: %s: invalid argument", __func__);
        return -1;
    }

    shmHeader = (shm_header_t *)_shm_addr;

    return shmHeader->sem_id;
}

void dc_shm_debug(void *_shm_addr)
{
    shm_header_t *shmHeader = NULL;

    assert(_shm_addr != NULL);
    shmHeader = _shm_addr;

    olog("+++++++++++++++++++++++++++++++++++++++++");

    olog("para_addr:[%p]",  _shm_addr);
    olog("init_addr:[%p]",  shmHeader->init_addr);

    olog("gate:     [%ld]", shmHeader->gate);
    olog("blk_size: [%ld]", shmHeader->block_size);
    olog("shm_size: [%ld]", shmHeader->shm_size);
    olog("offsetA:  [%ld]", shmHeader->offset_a);
    olog("offsetB:  [%ld]", shmHeader->offset_b);

    olog("+++++++++++++++++++++++++++++++++++++++++");

    return;
} /* dc_shm_debug */


#if RUN_SHM
int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  err = 0;
    int  shm_id   = 0;
    int  shm_key  = 1222;
    long shm_size = 10000;
    void *shm_addr = NULL;
    void * blk_a  = NULL;
    long flag = 0;

    olog("shm begin");

    (void)argc;
    (void)argv;

#if STARRING_INTEGRATE
    shm_addr = 0x700000160000000;
#endif

    /* shm */
    flag = SHM_CREATE;
    shm_id = dc_shm_init(shm_key, shm_size, &shm_addr, flag, &err);
    if (shm_id == FAILURE)
    {
        elog("error: dc_shm_init: %d", shm_id);
        return -1;
    }
    olog("shm addr:  %p", shm_addr);
    dc_shm_debug(shm_addr);

    /* blk */
    blk_a = dc_shm_get_block_A_addr(shm_addr);
    olog("blk addr is %p", blk_a);

    olog("shm end");
    return rv;
}
#endif

/* shm.c */
