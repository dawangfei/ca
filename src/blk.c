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
#include "blk.h"

/* ================================================================= */

typedef struct _segment_t {
    long offset;                            /* from block header */
    long size;                              /* segment size */
    char segment_name[SEGMENT_NAME_MAX+1];
} shm_segment_t;

typedef struct _block_header_t {
    long size_available;
    long size_allocated;
    long t1;                                /* block level */
    int  segment_num;                       /* allocated segment number */
    int  semid;                             /* as lock */
    shm_segment_t segments[SHM_MAX_SEGS];
} shm_block_header_t;

/* ================================================================= */

static void *g_blk_iterator_addr = NULL;
static int   g_blk_iterator_idx  = 0;


/*
 *  Function:  dc_blk_init
 *      init two block header: A, B
 *
 *  Parameters:
 *      _shm_addr   - address of shared memory
 *
 *  Return Value:
 *      SUCCESS - the index of segment
 *      FAILURE - failure
 */
int  dc_blk_init(void *_shm_addr)
{
    long                 block_size= 0;
    shm_block_header_t  *blkHeader = NULL;

    block_size= dc_shm_get_block_size(_shm_addr);

    /* block A */
    blkHeader = (shm_block_header_t *)dc_shm_get_block_A_addr(_shm_addr);
    blkHeader->size_available = block_size;

    /* block B */
    blkHeader = (shm_block_header_t *)dc_shm_get_block_B_addr(_shm_addr);
    blkHeader->size_available = block_size;

    return 0;
} /* dc_blk_init */


/*
 *  Function:  dc_blk_alloc_segment
 *      allocate a segment from block area
 *
 *  Parameters:
 *     _block_addr  - address of block
 *     _size        - the size of segments
 *
 *  Return Value:
 *      SUCCESS - the index of segment
 *      FAILURE - failure
 */
int dc_blk_alloc_segment(void *_block_addr, long _size, int *_errcode)
{
    int  i = 0;
    long size = 0;
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: _block_addr is NULL");
        return -1;
    }

    if (_size == 0)
    {
        elog("error: _size is 0");
        return -1;
    }

    if (_errcode == NULL)
    {
        elog("error: _errcode is NULL");
        return -1;
    }

    header = (shm_block_header_t *)_block_addr;

    size = DC_CEIL_ALIGN(_size, DC_WORD_WIDTH);
#if DC_DEBUG
    olog("segment size aligned: [%ld] -> [%ld]", _size, size);
#endif

    /* have a enough space */
    if (header->size_available < size)
    {
        elog("available[%d] < required[%d, %d(aligned)]", header->size_available, _size, size);
        *_errcode = -1;
        return FAILURE;
    }

    /* olog("current segment number <%d>", header->segment_num); */
    for (i = 0; i < SHM_MAX_SEGS; i++)
    {
        if (header->segments[i].size == 0)
        {
#if DC_DEBUG
            olog("nice: shm allocated <%d>", header->size_allocated);
#endif
            header->segments[i].offset = header->size_allocated;
            header->segments[i].size   = size;

            header->size_allocated    += size;
            header->size_available    -= size;
            header->segment_num       += 1;
#if DC_DEBUG
            olog("nice: shm allocated <%d>", header->size_allocated);
            olog("segment number [%d] -> [%d]", header->segment_num-1, header->segment_num);
#endif

            return i;
        }
    }

    *_errcode = -2;
    return FAILURE;
}



/*
 *  Function:  dc_blk_get_segment_addr
 *      get the address of a segment in shared memory
 *
 *  Parameters:
 *      _block_addr - address of block
 *      _idx        - the index of segments
 *
 *  Return Value:
 *      SUCCESS - the address for idx's segment
 *      FAILURE - NULL
 */
void *dc_blk_get_segment_addr(void *_block_addr, int _idx)
{
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: invalid input");
        return NULL;
    }

    header = (shm_block_header_t *)_block_addr;

    /*
    olog("block_addr: %p", header);
    olog("-- segment number: [%d]", header->segment_num);
    olog("-- idx: [%d]", _idx);
    */

    if (_idx < header->segment_num)
    {
        /*
        olog("offset: [%ld]", header->segments[_idx].offset);
        */
        return _block_addr + sizeof(shm_block_header_t) + header->segments[_idx].offset;
    }
    else
    {
        return NULL;
    }
}


int  dc_blk_set_segment_name(void *_block_addr, int _idx, char *_name)
{
    int  rv = 0;
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: invalid input");
        return -1;
    }

    header = (shm_block_header_t *)_block_addr;

    if (_idx < header->segment_num)
    {
        snprintf(header->segments[_idx].segment_name,
                sizeof(header->segments[_idx].segment_name),
                "%s", _name);
        rv = 0;
    }
    else
    {
        rv = -1;
    }

    return rv;
} /* dc_blk_set_segment_name */


int dc_blk_get_segment_index(void *_block_addr, char *_seg_name)
{
    int  i = 0;
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: invalid input");
        return -1;
    }

    header = (shm_block_header_t *)_block_addr;

    for (i = 0; i < header->segment_num; i++)
    {
        if (strcasecmp(header->segments[i].segment_name, _seg_name) == 0)
        {
            return i;
        }
    }

    return -1;
}


void *dc_blk_get_segment_addr_by_name(void *_block_addr, char *_seg_name)
{
    int  i = 0;
    void *addr = NULL;
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: invalid input");
        return NULL;
    }

    header = (shm_block_header_t *)_block_addr;

    /*
    olog("block_addr: %p", header);
    olog("-- segment number: [%d]", header->segment_num);
    */

    for (i = 0; i < header->segment_num; i++)
    {
        /*
        olog("[%s]", header->segments[i].segment_name);
        */
        if (strcasecmp(header->segments[i].segment_name, _seg_name) == 0)
        {
            /*
            olog("matched: %s => %d", _seg_name, i);
            */
            addr = _block_addr + sizeof(shm_block_header_t) + header->segments[i].offset;
            break;
        }
    }

    return addr;
}


/*
 *  Function:
 *      get the address of the READER block
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - the address
 *      FAILURE - NULL
 */
void *dc_blk_get_read_addr()
{
    long  reader = 0;
    void *shm_addr = NULL;
    void *blk_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr: NULL");
        return NULL;
    }

    reader = dc_shm_get_gate(shm_addr);

    if (reader == DC_SHM_REGION_A)
    {
#if DC_DEBUG
        olog("reader USE block A");
#endif
        blk_addr = dc_shm_get_block_A_addr(shm_addr);
    }
    else if (reader == DC_SHM_REGION_B)
    {
#if DC_DEBUG
        olog("reader USE block B");
#endif
        blk_addr = dc_shm_get_block_B_addr(shm_addr);
    }
    else
    {
        elog("gate: <%ld> not initialized", reader);
    }

    return blk_addr;
}


/*
 *  Function:
 *      get the address of the WRITER block
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - the address
 *      FAILURE - NULL
 */
void *dc_blk_get_write_addr()
{
    long  reader = 0;
    void *shm_addr = NULL;
    void *blk_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr: NULL");
        return NULL;
    }

    reader = dc_shm_get_gate(shm_addr);

    if (reader == DC_SHM_REGION_A)
    {
#if DC_DEBUG
        olog("writer USE block B");
#endif
        blk_addr = dc_shm_get_block_B_addr(shm_addr);
    }
    else if (reader == DC_SHM_REGION_B)
    {
#if DC_DEBUG
        olog("writer USE block A");
#endif
        blk_addr = dc_shm_get_block_A_addr(shm_addr);
    }
    else
    {
        elog("gate: <%ld> not initialized", reader);
    }

    return blk_addr;
}


/*
 *  Function:  dc_blk_switch
 *      switch to another side
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_blk_switch()
{
    void *shm_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr: NULL");
        return -1;
    }

    dc_shm_switch_gate(shm_addr);

    return 0;
}


int dc_blk_get_segment_number(void *_blk_addr)
{
    shm_block_header_t  *blkHeader = NULL;

    blkHeader = (shm_block_header_t *)_blk_addr;

    if (blkHeader == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        return -1;
    }

    return blkHeader->segment_num;
}


int dc_blk_calc_size(int _all_segment_size)
{
    return sizeof(shm_block_header_t) + _all_segment_size;
}


int dc_blk_set_timestamp(void *_block_addr, long _t)
{
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: _block_addr is NULL");
        return -1;
    }

    header = (shm_block_header_t *)_block_addr;

    header->t1 = _t;

    return 0;
}


long dc_blk_get_timestamp(void *_block_addr)
{
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: _block_addr is NULL");
        return -1;
    }

    header = (shm_block_header_t *)_block_addr;

    return header->t1;
}


int dc_blk_set_semid(void *_block_addr, int _id)
{
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: _block_addr is NULL");
        return -1;
    }

    header = (shm_block_header_t *)_block_addr;

    header->semid = _id;

    return 0;
}


int dc_blk_get_semid(void *_block_addr)
{
    shm_block_header_t *header = NULL;

    if (_block_addr == NULL)
    {
        elog("error: _block_addr is NULL");
        return -1;
    }

    header = (shm_block_header_t *)_block_addr;

    return header->semid;
}


void dc_blk_iterator_init(void *_block_addr)
{
    g_blk_iterator_addr = _block_addr;
    g_blk_iterator_idx  = 0;
}


char *dc_blk_iterator_next()
{
    shm_block_header_t *header = NULL;

    if (g_blk_iterator_addr == NULL)
    {
        elog("error: not inited");
        return NULL;
    }

    header = (shm_block_header_t *)g_blk_iterator_addr;

    if (g_blk_iterator_idx >= header->segment_num)
    {
        return NULL;
    }
    else
    {
        return header->segments[g_blk_iterator_idx++].segment_name;
    }
}


void dc_blk_iterator_end()
{
    g_blk_iterator_addr = NULL;
}


#if RUN_BLK
int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  err = 0;
    int  idx = 0;
    int  to_read   = 0;
    int  to_create = 0;
    int  to_write  = 0;
    int  shm_id   = 0;
    int  shm_key  = 1222;
    long seg_size = 1024;
    long shm_size = 10000;
    void *shm_addr = NULL;
    void *blk_a    = NULL;
    void *seg_addr = NULL;
    long flag = 0;

    olog("blk begin");

    /* shm */
    flag = SHM_CREATE;
    shm_id = dc_shm_init(shm_key, shm_size, &shm_addr, flag, &err);
    if (shm_id == FAILURE)
    {
        elog("error: dc_shm_init: %d", shm_id);
        return -1;
    }
    olog("shm addr  %p", shm_addr);
    dc_shm_debug(shm_addr);

    /* blk */
    dc_blk_init(shm_addr);

    /* blk-A */
    blk_a = dc_shm_get_block_A_addr(shm_addr);
    olog("blk addr is %p", blk_a);

    if (argc == 3)
    {
        idx = atoi(argv[1]);

        if (memcmp(argv[2], "R", 1) == 0)
        {
            olog("to_read");
            to_read = 1;
        }
        else if (memcmp(argv[2], "C", 1) == 0)
        {
            olog("to_create");
            to_create = 1;
        }
        else if (memcmp(argv[2], "W", 1) == 0)
        {
            olog("to_write");
            to_write = 1;
        }
        else
        {
            elog("error: unknown cmd: %s", argv[2]);
            return -1;
        }
    }
    else
    {
        idx = 0;
        to_read = 1;
    }

    /* segment */
    if (to_create)
    {
        olog("let's create a new segment");
        idx = dc_blk_alloc_segment(blk_a, seg_size, &err);
        if (idx == FAILURE)
        {
            elog("error: dc_blk_alloc_segment: %d", err);
            return -2;
        }
        olog("allocated index is <%d>", idx);
    }

    /* segment_addr */
    seg_addr = dc_blk_get_segment_addr(blk_a, idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr failure");
        return -3;
    }
    olog("segment address is <%p>", seg_addr);

    if (to_read)
    {
        olog("READER");
        olog("\t<%s>", seg_addr);
    }

    if (to_write)
    {
        snprintf(seg_addr, seg_size, "hello, %d", getpid());
        olog("WRITER");
        olog("<%s> is written", seg_addr);
    }

    olog("blk end");
    return rv;
}
#endif

/* blk.c */
