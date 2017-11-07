#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "tm.h"
#include "type.h"
#include "log.h"
#include "util.h"
#include "list.h"
#include "hash.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "sec.h"
#include "sql.h"
#include "rs.h"
#include "put.h"
#include "query.h"
#include "his.h"
#include "load.h"
#include "lock.h"
#include "config.h"

#include "ext.h"
#include "aux.h"
#include "api.h"

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"

#include "nt.h"

/*******************************************************************
 *******************************************************************/

/* ================================================================= */

static int  dc_cache_api_remote_select(char * _sql, int _sql_len);
static int  dc_cache_api_pre_check_table_exist(char *_table_name);

#if ENABLE_HIS
static long dc_cache_api_remote_tm(char * _table_name);
#endif
/* ================================================================= */

static void *g_shm_addr = NULL;
static int   g_shm_id   = -1;
static char  g_sql[4096]= {0};

#define  DC_CLUSTER_LIFE_TIME  42
static int   g_cluster_life = DC_CLUSTER_LIFE_TIME;

/*
 *  Function:
 *      1. create IPC
 *      2. load all tables
 *
 *      Use lock
 *      WARN: invoke only one time!
 *      WARN: this is supervisor user, who will create the IPC.
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_admin_init(long _shm_key)
{
    int  rv = 0;
    int  err = 0;
    int  shm_id = -1;
    int  sem_id = -1;
    int  mutex = -1;
    int  sem_id_A = -1;
    int  sem_id_B = -1;
    int  max_segs = 0;
    long t2 = 0;
    long shm_flag = 0;
    long shm_size = 0;
    void *shm_addr = NULL;
    void *blk_addr = NULL;

    if (dc_load_init())
    {
        elog("error: dc_load_init");
        return -1;
    }

    /* calculate the size of all shared memory */
    shm_size = dc_load_calc_shm_size();
    if (shm_size <= 0)
    {
        elog("error: dc_load_calc_shm_size");
        return -1;
    }
#if DC_DEBUG
    olog("total shm size: %ld", shm_size);
#endif

    /* tree */
    rb_tree_register();

    /* get init shm addr */
    shm_addr = dc_aux_ipc_get_shm_init_addr();
#if DC_DEBUG
    olog("shm addr: %p", shm_addr);
#endif

    /* create IPC of SHM */
    shm_flag = SHM_CREATE;
    shm_id = dc_shm_init(_shm_key, shm_size, &shm_addr, shm_flag, &err);
    if (shm_id == FAILURE)
    {
        elog("error: dc_shm_init: %d", shm_id);
        return -1;
    }
    g_shm_id = shm_id;
    g_shm_addr = shm_addr;
#if DC_DEBUG
    olog("shm addr: %p", shm_addr);
#endif

    dc_lock_init();

    /* mutex */
    mutex = dc_lock_create_mutext_lock();
    if (mutex < 0)
    {
        elog("error: dc_lock_create_mutext_lock: %d", mutex);
        return -1;
    }
#if DC_DEBUG
    olog("mutex: %d", mutex);
#endif

    if (dc_shm_set_semid(shm_addr, mutex))
    {
        elog("error: dc_shm_set_semid");
        return -1;
    }

    /* blk */
    dc_blk_init(shm_addr);

    max_segs = SHM_MAX_SEGS;

#if DC_DEBUG
    olog("creating CRITICAL region");
#endif
    if (dc_mutex_lock(mutex))
    {
        elog("error: dc_mutex_lock: %d", mutex);
        return -1;
    }
#if DC_DEBUG
    olog("nice: created");
#endif

    /* ======================A========================== */

    /* load BLOCK-A */
    blk_addr = dc_shm_get_block_A_addr(shm_addr);
    if (blk_addr == NULL)
    {
        elog("error: dc_shm_get_block_A_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("blk-A addr: %p", blk_addr);
#endif

    /* BLOCK-A: sem/lock: create */
    sem_id = dc_lock_create(max_segs);
    if (sem_id < 0)
    {
        elog("error: dc_lock_create: %d", max_segs);
        rv = -1;
        goto _end;
    }
    sem_id_A = sem_id;
#if DC_DEBUG
    olog("blk-A sem_id: %d", sem_id);
#endif

    /* SEM-A */
    if (dc_blk_set_semid(blk_addr, sem_id))
    {
        elog("error: dc_blk_set_semid");
        rv = -1;
        goto _end;
    }

    /* 2017-11-3: unified timestamp  */
    t2 = get_time();

    /* DB => Cache */
    if (dc_load_all_tables(blk_addr, t2))
    {
        elog("error: dc_load_all_tables");
        rv = -1;
        goto _end;
    }

    /* ======================B========================== */

    /* BLOCK-B */
    blk_addr = dc_shm_get_block_B_addr(shm_addr);
    if (blk_addr == NULL)
    {
        elog("error: dc_shm_get_block_B_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("blk-B addr: %p", blk_addr);
#endif

    /* BLOCK-B: sem/lock: create */
    sem_id = dc_lock_create(max_segs);
    if (sem_id < 0)
    {
        elog("error: dc_lock_create: %d", max_segs);
        rv = -1;
        goto _end;
    }
    sem_id_B = sem_id;
#if DC_DEBUG
    olog("blk-B sem_id: %d", sem_id);
#endif

    /* SEM-B */
    if (dc_blk_set_semid(blk_addr, sem_id))
    {
        elog("error: dc_blk_set_semid");
        rv = -1;
        goto _end;
    }

    /* DB => Cache */
    if (dc_load_all_tables(blk_addr,  t2))
    {
        elog("error: dc_load_all_tables: B");
        rv = -1;
        goto _end;
    }

    /* switch to A */
    if (dc_blk_switch())
    {
        elog("error: dc_blk_switch");
        rv = -1;
        goto _end;
    }

    if (dc_shm_set_ready(shm_addr))
    {
        elog("error: dc_shm_set_ready");
        rv = -1;
        goto _end;
    }

#if DC_DEBUG
    olog("releasing CRITICAL region");
#endif
    if (dc_mutex_unlock(mutex))
    {
        elog("error: dc_mutex_unlock");
        rv = -1;
        goto _end;
    }
    else
    {
        /* clear */
        mutex = -1;
#if DC_DEBUG
        olog("nice: release");
#endif
    }

#if ENABLE_HIS
    if (dc_his_init())
    {
        elog("error: dc_his_init");
        rv = -1;
        goto _end;
    }
#endif

    if (dc_query_init())
    {
        elog("error: dc_query_init");
        rv = -1;
        goto _end;
    }

_end:

    if (rv)
    {
        olog("handle exception...");

        if (mutex != -1)
        {
            olog("release mutex lock");
            if (dc_mutex_unlock(mutex))
            {
                elog("error: fatal: dc_mutex_unlock");
            }
        }

        /* release all resources */
        if (sem_id_A >= 0)
        {
            olog("release SEM-ID A");
            if (dc_lock_destroy(sem_id_A))
            {
                elog("error: dc_lock_destroy: sem-A failure: %d", sem_id_A);
            }
        }

        if (sem_id_B >= 0)
        {
            olog("release SEM-ID B");
            if (dc_lock_destroy(sem_id_B))
            {
                elog("error: dc_lock_destroy: sem-B failure: %d", sem_id_B);
            }
        }

        if (mutex >= 0)
        {
            olog("release MUTEX");
            if (dc_lock_destroy(mutex))
            {
                elog("error: dc_lock_destroy: mutex failure: %d", mutex);
            }
        }

        if (shm_id >= 0 && shm_addr != NULL)
        {
            olog("release SHARED memory");
            if (dc_shm_term(shm_id, shm_addr))
            {
                elog("error: dc_shm_term: %d, %p", shm_id, shm_addr);
            }
        }
    }
    else
    {
#if DC_DEBUG
        olog("function '%s' succeeds", __func__);
#endif
    }

    return rv;
}


/*
 *  Function:
 *      detach from IPC and delete it
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_admin_close()
{
    int  rv = 0;
    int  sem_id = 0;
    void *blk_addr = NULL;


    if (g_shm_id < 0 || g_shm_addr == NULL)
    {
        elog("not attached");
        return 1;
    }


    /* TODO: release other resources */

    /* close WRITE lock */
    blk_addr = dc_blk_get_write_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_write_addr is NULL");
        return -1;
    }

    sem_id = dc_blk_get_semid(blk_addr);
    if (dc_lock_destroy(sem_id))
    {
        elog("error: dc_lock_destroy writer: %d", sem_id);
    }

    /* close READ lock */
    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr is NULL");
        return -1;
    }
    sem_id = dc_blk_get_semid(blk_addr);
    if (dc_lock_destroy(sem_id))
    {
        elog("error: dc_lock_destroy reader: %d", sem_id);
    }

    /* close MUTEX lock */
    sem_id = dc_shm_get_semid(g_shm_addr);
    if (dc_lock_destroy(sem_id))
    {
        elog("error: dc_lock_destroy mutex: %d", sem_id);
    }

    /* detach and delete */
    if (dc_shm_term(g_shm_id, g_shm_addr))
    {
        elog("error: dc_shm_term: %d, %p", g_shm_id, g_shm_addr);
        return -1;
    }

    g_shm_id = -1;
    g_shm_addr = NULL;

    return rv;
}


/*
 *  Function:
 *      attach to IPC
 *      for business process like, ALA, Switch
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_open(long _shm_key)
{
    int err = 0;
    int shm_id = 0;
    int ready = 0;
    long shm_flag = 0;
    long shm_size = 0;
    void *shm_addr = NULL;

    dc_lock_init();

    /* register tree for SHM */
    rb_tree_register();

    /* get init shm addr */
    shm_addr = dc_aux_ipc_get_shm_init_addr();
#if DC_DEBUG
    olog("shm addr: %p", shm_addr);
#endif

    /* attach to SHM */
    shm_flag = 0;
    shm_id = dc_shm_init(_shm_key, shm_size, &shm_addr, shm_flag, &err);
    if (shm_id == FAILURE)
    {
        if (shm_addr != NULL)
        {
#if DC_DEBUG
            olog("use unified shm-addr: %p", shm_addr);
#endif
            shm_id = dc_shm_init(_shm_key, shm_size, &shm_addr, shm_flag, &err);
            if (shm_id == FAILURE)
            {
                elog("error: dc_shm_init2: %d", shm_id);
                return -1;
            }
        }
        else
        {
            elog("error: dc_shm_init1: %d", shm_id);
            return -1;
        }
    }
    g_shm_id = shm_id;
    g_shm_addr = shm_addr;
    /*
    olog("attach at: %p", shm_addr);
    */

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

#if ENABLE_HIS
    /* history cache */
    if (dc_his_init())
    {
        elog("error: dc_his_init");
        return -1;
    }
#endif

    /* local DS */
    if (dc_query_init())
    {
        elog("error: dc_query_init");
        return -1;
    }

    return 0;
}


/*
 *  Function:
 *      detach from IPC
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_close()
{
    int rv = 0;

    /* TODO: release other resources */


    if (g_shm_id < 0 || g_shm_addr == NULL)
    {
        elog("not attached");
        return 1;
    }

    if (dc_shm_term(-1, g_shm_addr))
    {
        elog("error: dc_shm_term: %d, %p", -1, g_shm_addr);
        return -1;
    }

    g_shm_id = -1;
    g_shm_addr = NULL;

    return rv;
}


/*
 *  Function:
 *      allocate resource for query
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_alloc_area()
{
    int rv  = 0;

    return rv;
}


/*
 *  Function:
 *      free resource from 'ALLOC'
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_free_area()
{
    int rv  = 0;

    return rv;
}


/**
 *
 */
static int dc_cache_api_discarded()
{
    int  ready = 0;
    void *shm_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }

    ready = dc_shm_get_ready(shm_addr);
    if (DC_SHM_IS_DISCARD(ready))
    {
#if DC_DEBUG
        olog("warn: directly DB mode");
#endif
        return 1;
    }

    return 0;
}


/*
 *  Function:
 *      set to enable or discard mode
 *      shm->ready
 *
 *  Parameters:
 *      _mode -  0 not ready
 *      _mode -  1 cache mode
 *      _mode -  2 db mode
 *
 *  Return Value:
 *      SUCCESS - N>0: get N rows
 *                  0: not found
 *      FAILURE - -1
 */
int dc_cache_api_set_mode(int _mode)
{
    int  rv = 0;
    void *shm_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }

    if (_mode == 1)
    {
        rv = dc_shm_set_ready(shm_addr);
    }
    else if (_mode == 2)
    {
        rv = dc_shm_set_discard(shm_addr);
    }
    else
    {
        elog("sorry: not support: %d", _mode);
    }

    return rv;
}


/*
 *  Function:
 *      Query by SQL, for 1 row case
 *      Use lock
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - N>0: get N rows
 *                  0: not found
 *      FAILURE - -1
 */
int dc_cache_api_select(char *_sql)
{
    int  rv = 0;
    int  len = 0;

#if DC_DEBUG
    olog("++++++++++++++++++++++++++++++++++++++++");
#endif

    memset(g_sql, 0, sizeof(g_sql));
    if (dc_query_resolve_sql(_sql, strlen(_sql), g_sql, sizeof(g_sql)))
    {
        elog("error: invalid sql: [%s]", _sql);
        return -1;
    }

#if ENABLE_HIS
    int  row_len = 0;
    int  remote = 0;
    long remote_tm = 0;
    void *row = NULL;
    void *seg_header = NULL;
    void *select = NULL;
    void *p = NULL;
    char *name = NULL;

    if (g_shm_addr == NULL)
    {
        elog("error: not connect: please call dc_cache_api_open first");
        return -1;
    }

    rv = dc_his_query_pre(g_sql, &p, &name);
    if (rv < 0)
    {
        elog("error: dc_his_query_pre: %s", g_sql);
        return -1;
    }
    else if (rv)
    {
        /* from HIS not find */
#if DC_DEBUG
        olog("not find from HIS");
#endif

        remote = 1;
    }
    else
    {
        /* find from HIS */
#if DC_DEBUG
        olog("nice: query HIS succeeds");
#endif

        /* get remote tm with lock */
        remote_tm = dc_cache_api_remote_tm(name);
        if (remote_tm <= 0)
        {
            elog("error: dc_cache_api_remote_tm: %s", name);
            return -1;
        }
#if DC_DEBUG
        olog("remote tm: %ld", remote_tm);
#endif

        /* check remote keeps, then get result */
        rv = dc_his_query_post(p, remote_tm, &row, &row_len, &seg_header, &select, &name);
        if (rv < 0)
        {
            elog("error: dc_his_query_post: [%s]", g_sql);
            return -1;
        }
        else if (rv)
        {
            /* obsolete */
            remote = 1;
#if DC_DEBUG
            olog("obsolete");
#endif
        }
        else
        {
            /*
            olog("nice: still fresh");
            */
        }
    }


    if (remote)
    {
        /* from HIS not find */

        /* +: do remote query */
#if DC_DEBUG
        olog("do remote query");
#endif
        len = strlen(g_sql);
        rv = dc_cache_api_remote_select(g_sql, len);
        if (rv < 0)
        {
            elog("error: dc_cache_api_remote_select: %d, [%s]", rv, g_sql);
            return -1;
        }
        else if (rv == 0)
        {
#if DC_DEBUG
            olog("sorry, can't find data");
#endif
        }
        else 
        {
#if DC_DEBUG
            olog("query get %d rows", rv);
            olog("----------------------------------------");
#endif

            /* +: add to his */
            if (rv == 1)    /* only for one row case */
            {
                if (dc_rs_get_row2(&row, &row_len))
                {
                    elog("error: dc_rs_get_row2");
                    return -1;
                }

                seg_header = dc_rs_get_seg_header();

                select = dc_sql_get_select();

                name = dc_sql_get_table_name();

                rv = dc_his_add(g_sql, row, row_len, seg_header, select, name);
                if (rv)
                {
                    elog("error: dc_his_add: [%s]", g_sql);
                    return -1;
                }
                else
                {
#if DC_DEBUG
                    olog("dc_his_add succeeds");
#endif
                }

                rv = 1;
            }
            else
            {
#if DC_DEBUG
                olog("%d: more than one row, don't save", rv);
#endif
            }
        }
    }
    else
    {
        /* find from HIS */
#if DC_DEBUG
        olog("got from HIS");
#endif

        /* restore to sql && rs */
        dc_sql_set_select(select);
        dc_sql_set_table_name(name);

        if (dc_rs_restore_context(row, row_len, seg_header))
        {
            elog("error: dc_rs_restore_context");
            return -1;
        }

#if DC_DEBUG
        olog("restore context");
#endif

        rv = 1;
    }
#else
    /* +: do remote query */

    if (g_shm_addr == NULL)
    {
        elog("error: not connect: please call dc_cache_api_open first");
        return -1;
    }

#if DC_DEBUG
    olog("do remote query");
#endif
    len = strlen(g_sql);
    rv = dc_cache_api_remote_select(g_sql, len);
    if (rv < 0)
    {
        elog("error: dc_cache_api_remote_select: %d", rv);
        return -1;
    }
    else if (rv == 0)
    {
#if DC_DEBUG
        olog("sorry, can't find data");
#endif
    }
    else 
    {
#if DC_DEBUG
        olog("query get %d rows", rv);

        olog("----------------------------------------");
#endif
    }
#endif

    return rv;
}


/*
 *  Function:
 *      do remote query with lock
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - N>0: get N rows
 *                  0: not found
 *      FAILURE - -1
 */
static int dc_cache_api_remote_select(char * _sql, int _sql_len)
{
    int  rv   = 0;
    int  sem_id = -1;
    int  seg_idx = -1;
    void *blk_addr = NULL;

    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr is NULL");
        return -1;
    }
#if DC_DEBUG
    olog("blk addr: %p", blk_addr);
#endif

    seg_idx = dc_query_sql_select_pre(blk_addr, _sql, _sql_len);
    if (seg_idx < 0)
    {
        elog("error: dc_query_sql_select_pre");
        return -1;
    }

    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid: %p", blk_addr);
        return -1;
    }

    /* read(shared) lock */
    if (dc_shared_lock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_lock: %d, %d", sem_id, seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("read locked");
#endif

    rv = dc_query_sql_select_post(blk_addr, seg_idx);

    /* unlock */
    if (dc_shared_unlock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_unlock: %d, %d", sem_id, seg_idx);
        return -1;
    }

#if DC_DEBUG
    olog("read unlocked");
#endif

    if (rv < 0)
    {
        elog("error: dc_query_sql_select_post: %d", rv);
    }
    else
    {
#if DC_DEBUG
        olog("dc_query_sql_select_post: %d", rv);
#endif
    }

    return rv;
}


#if ENABLE_HIS
/*
 *  Function:
 *      get remote time with lock
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - N>0: get N rows
 *                  0: not found
 *      FAILURE - -1
 */
static long dc_cache_api_remote_tm(char * _table_name)
{
    int  sem_id = -1;
    int  seg_idx = -1;
    long timestamp = 0;
    void *seg_addr = NULL;
    void *blk_addr = NULL;

    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr is NULL");
        return -1;
    }
#if DC_DEBUG
    olog("blk addr: %p", blk_addr);
#endif

    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid: %p", blk_addr);
        return -1;
    }

    seg_idx = dc_blk_get_segment_index(blk_addr, _table_name);
    if (seg_idx == -1)
    {
        elog("error: dc_blk_get_segment_index: %s", _table_name);
        return -1;
    }
#if DC_DEBUG
    olog("%s => [%02d]", _table_name, seg_idx);
#endif

    seg_addr = dc_blk_get_segment_addr(blk_addr, seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr: not found '%d'", seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("segment addr is <%p>", seg_addr);
#endif

    /* read(shared) lock */
    if (dc_shared_lock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_lock: %d, %d", sem_id, seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("tm locked");
#endif

    timestamp = dc_seg_get_timestamp(seg_addr);
#if DC_DEBUG
    olog("tm:%ld", timestamp);
#endif

    /* unlock */
    if (dc_shared_unlock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_unlock: %d, %d", sem_id, seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("tm unlocked");
#endif

    return timestamp;
}
#endif


/*
 *  Function:
 *      Query by SQL, for more than 1 row case
 *      Use lock
 *
 *      should with: dc_cache_api_fetch()
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - N>0: get N rows
 *                  0: not found
 *      FAILURE - -1
 */
int dc_cache_api_cursor(char *_sql)
{
    int rv = 0;

    rv = dc_cache_api_select(_sql);
    if (rv >= 1)
    {
#if DC_DEBUG
        olog("dc_rs_iterator_init");
#endif
        dc_rs_iterator_init();
    }

    return rv;
}


/*
 *  Function:
 *      fetch next row, for multiple rows result set
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - p, not NULL
 *      FAILURE - NULL, no more
 */
void *dc_cache_api_fetch()
{
    return dc_query_rs_next();
}


int dc_cache_api_dump_curr_row()
{
    return dc_query_dump_curr_row();
}


/*
 *  Function:
 *      get column data
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_get_data(int _idx, void *_buffer, int _buffer_size)
{
    int rv = 0;

    if ((rv = dc_query_get_data(_idx, _buffer, _buffer_size)) < 0)
    {
        elog("error: dc_query_get_data: %d", _idx);
        return -1;
    }

    return 0;
}


static int dc_cache_api_pre_check_table_exist(char *_table_name)
{
    int  rv  = 0;
    int  ready = 0;
    int  seg_idx = 0;
    void *shm_addr = NULL;
    void *blk_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

    /* block */
    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        return -1;
    }

    /* get index of segment*/
    seg_idx = dc_blk_get_segment_index(blk_addr, _table_name);
    if (seg_idx < 0)
    {
        elog("error: dc_blk_get_segment_index: not found '%s'", _table_name);
        return -1;
    }

    return rv;
}


/*
 *  Function:
 *      Reload table from DB to SHM
 *      Use lock
 *      For business process like, ALA, Switch
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_reload(char *_table_name)
{
    int  rv  = 0;
    int  ready = 0;
    int  mutex = -1;
    int  sem_id = -1;
    int  seg_idx = 0;
    long t2 = 0;
    void *shm_addr = NULL;
    void *blk_addr = NULL;

    /* XXX: MUST: get SHM/mutex lock first, to ensure concurrent write */

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }
#if DC_DEBUG
    olog("shm addr: %p", shm_addr);
#endif

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

    if (dc_cache_api_pre_check_table_exist(_table_name))
    {
        elog("error: dc_cache_api_pre_check_table_exist: %s", _table_name);
        return -1;
    }

    mutex = dc_shm_get_semid(shm_addr);
    if (mutex < 0)
    {
        elog("error: dc_shm_get_semid");
        return -1;
    }
#if DC_DEBUG
    olog("mutex: %d", mutex);
#endif

    dc_lock_init();

#if DC_DEBUG
    olog("creating CRITICAL region");
#endif
    if (dc_mutex_lock(mutex))
    {
        elog("error: dc_mutex_lock");
        return -1;
    }
#if DC_DEBUG
    olog("nice: safe");
#endif

    /* ASSUME CURRENT IS READING A */

    /* +++++++++++++++++++++++++++++++++++++++*/

    /* +: write block B */
    blk_addr = dc_blk_get_write_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_write_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("WRITER-1 block addr: %p", blk_addr);
#endif

    /* +: get index of segment*/
    seg_idx = dc_blk_get_segment_index(blk_addr, _table_name);
    if (seg_idx < 0)
    {
        elog("error: dc_blk_get_segment_index: not found '%s'", _table_name);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("segment index is <%d>", seg_idx);
#endif

    /* +: lock B */
    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid");
        rv = -1;
        goto _end;
    }

    /* lock the whole SEGMENT */
#if DC_DEBUG
    olog("try get exclusive lock: %d, %d", sem_id, seg_idx);
#endif
    if (dc_exclusive_lock(sem_id, seg_idx))
    {
        elog("error: dc_exclusive_lock: %d, %d", sem_id, seg_idx);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("nice: got");
#endif

    t2 = get_time();

    /* +: write B */
    if (dc_load_table_refresh(blk_addr, _table_name, t2))
    {
        elog("error: dc_load_table_refresh");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("reload-1: %s succeeds", _table_name);
#endif

    /* +: un-lock B */
    if (dc_exclusive_unlock(sem_id, seg_idx))
    {
        elog("error: dc_exclusive_unlock: %d, %d", sem_id, seg_idx);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("nice: lock-1 released");
#endif

    /* +++++++++++++++++++++++++++++++++++++++*/

    /* +: switch to read B */
    if (dc_blk_switch())
    {
        elog("error: dc_blk_switch");
        rv = -1;
        goto _end;
    }

    /* +++++++++++++++++++++++++++++++++++++++*/

    /* +: write block A */
    blk_addr = dc_blk_get_write_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_write_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("WRITER-2 block addr: %p", blk_addr);
#endif

    /* +: get index of segment*/
    seg_idx = dc_blk_get_segment_index(blk_addr, _table_name);
    if (seg_idx < 0)
    {
        elog("error: dc_blk_get_segment_index: %s", _table_name);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("segment index is <%d>", seg_idx);
#endif

    /* +: lock A */
    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("block sem_id: %d", sem_id);
#endif

    /* +: lock the whole SEGMENT */
#if DC_DEBUG
    olog("try get exclusive lock: %d, %d", sem_id, seg_idx);
#endif
    if (dc_exclusive_lock(sem_id, seg_idx))
    {
        elog("error: dc_exclusive_lock: %d, %d", sem_id, seg_idx);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("nice: got");
#endif

    /* +: write A */
    if (dc_load_table_refresh(blk_addr, _table_name, t2))
    {
        elog("error: dc_load_table_refresh");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("reload: %s succeeds", _table_name);
#endif

    /* +: un-lock A */
    if (dc_exclusive_unlock(sem_id, seg_idx))
    {
        elog("error: dc_exclusive_unlock: %d, %d", sem_id, seg_idx);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("nice: lock-2 released");
#endif

    /* releast the MUTEX lock */
#if DC_DEBUG
    olog("releasing CRITICAL region");
#endif
    if (dc_mutex_unlock(mutex))
    {
        elog("error: dc_mutex_unlock");
        rv = -1;
        goto _end;
    }
    else
    {
        mutex = -1;
#if DC_DEBUG
        olog("nice: CRITICAL region released");
#endif
    }

_end:

    if (rv)
    {
        olog("handle exception...");

        if (sem_id != -1)
        {
            olog("FAIL: to-release SEGMENT lock: %d, %d", sem_id, seg_idx);
            if (dc_exclusive_unlock(sem_id, seg_idx))
            {
                elog("FAIL: error: dc_exclusive_unlock: %d, %d", sem_id, seg_idx);
            }
            else
            {
                olog("++++: release SEGMENT lock succeeds");
            }
        }

        if (mutex != -1)
        {
            olog("FAIL: to-release mutex lock: %d", mutex);
            if (dc_mutex_unlock(mutex))
            {
                elog("FAIL: error: fatal: dc_mutex_unlock");
            }
            else
            {
                olog("++++: release MUTEX lock succeeds");
            }
        }
    }
    else
    {
        /*
        olog("function '%s' succeeds", __func__);
        */
    }


    return rv;
}


/*
 *  Function:
 *      execute some job in cluster mode
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_cluster(char *_table_name, char *_action)
{
    int  i   = 0;
    int  rv  = 0;
    int  num = 0;
    int  len = 0;
    int  sid = 0;
    int  size = 0;
    int  life = 10;
    long curr_time = 0;
    char section[64] = {0};
    char entry[64] = {0};
    char buffer[1024] = {0};
    char file_path[1024] = {0};

#define DC_ADDR_MAX    100
    dc_nt_addr addr_list[DC_ADDR_MAX];
    dc_nt_addr *p = NULL;

    dc_cache_api_svc_t svc_in;
    dc_cache_api_svc_t svc_out;

    char *sep = ":";
    char *ptr = NULL;

#if DC_DEBUG
    olog("code:  %s", _action);
    olog("table: %s", _table_name);
#endif

    /* 1. get config file path */
#if STARRING_INTEGRATE
    ptr = getenv("FAPWORKDIR");
    if (ptr == NULL)
    {
        elog("error: please set environment: export FAPWORKDIR=/path/");
        return -1;
    }
    snprintf(file_path, sizeof(file_path), "%s/etc/cache.cfg", ptr);
#else
    ptr = getenv("DC_CACHE_CONFIG");
    if (ptr == NULL)
    {
        elog("error: please set environment: export DC_CACHE_CONFIG=/path/a.txt");
        return -1;
    }
    snprintf(file_path, sizeof(file_path), "%s", ptr);
#endif
    olog("config-file-path: %s", file_path);

    /* 2. get ip:port list */
    strcpy(section, "CLUSTER");
    strcpy(entry, "CNT");
    memset(buffer, 0, sizeof(buffer));
    rv = dc_cfg_get_value(section, entry, file_path, buffer, sizeof(buffer));
    if (rv < 0)
    {
        elog("error: dc_cfg_get_value: %s:%s", section, entry);
        return -1;
    }
    num = atoi(buffer);
#if DC_DEBUG
    olog("host number: %d(%s)", num, buffer);
#endif

    if (num > DC_ADDR_MAX)
    {
        elog("too much data: %d > %d", num, DC_ADDR_MAX);
        return -1;
    }

    memset(&addr_list, 0, sizeof(addr_list));
    for (i = 0; i < num; i++)
    {
        snprintf(entry, sizeof(entry), "HOST_%d", i);
        memset(buffer, 0, sizeof(buffer));
        rv = dc_cfg_get_value(section, entry, file_path, buffer, sizeof(buffer));
        if (rv < 0)
        {
            elog("error: dc_cfg_get_value: %s:%s", section, entry);
            return -1;
        }
        olog("got: %s ==> %s", entry, buffer);

        /* IP */
        ptr = strtok(buffer, sep);
        if (ptr == NULL)
        {
            elog("error: invalid data: %s", buffer);
            return -1;
        }
        snprintf(addr_list[i].ip, sizeof(addr_list[i].ip), "%s", ptr);

        /* PORT */
        ptr = strtok(NULL, sep);
        if (ptr == NULL)
        {
            elog("error: invalid data: %s", buffer);
            return -1;
        }
        snprintf(addr_list[i].port, sizeof(addr_list[i].port), "%s", ptr);
        addr_list[i].sid = -1;
#if DC_DEBUG
        olog("add: %s:%s", addr_list[i].ip, addr_list[i].port);
#endif

    } /* for */


    /* 3. loop: connect */
    for (i = 0; i < num; i++)
    {
        p = &addr_list[i];
        if (strlen(p->ip) == 0)
        {
#if DC_DEBUG
            olog("[%d]th no data, means the last.", i);
#endif
            break;
        }
        else
        {
            olog("to-connect[%d, %s:%s]", i, p->ip, p->port);
        }


        sid = dc_net_simple_connect(p->ip, p->port);
        if (sid < 0)
        {
            olog("error: dc_net_simple_connect");
            rv = -1;
            goto _end;
        }
        p->sid = sid;

        olog("nice: connect[%s:%s] succeeds", p->ip, p->port);

    } /* for */


    /* 4. loop: request */
    for (i = 0; i < num; i++)
    {
        p = &addr_list[i];
        if (strlen(p->ip) == 0)
        {
#if DC_DEBUG
            olog("[%d]th no data, means the last.", i);
#endif
            break;
        }
        else
        {
            olog("----------------------------------------------");
            olog("to-request: [%d]th -- [%d][%s, %s]", i, p->sid, p->ip, p->port);
        }

        memset(&svc_in,  0, sizeof(svc_in));
        memset(&svc_out, 0, sizeof(svc_out));

        snprintf(svc_in.code,       sizeof(svc_in.code),  "%s", _action);
        snprintf(svc_in.table,      sizeof(svc_in.table), "%s", _table_name);
        curr_time = get_time();
        dctime_format_micro(curr_time, svc_in.timestamp0, sizeof(svc_in.timestamp0));

        len  = sizeof(svc_in);
        size = sizeof(svc_out);

        rv = dc_net_simple_request(p->sid, (char *)&svc_in, len, (char *)&svc_out, size, life);
        if (rv)
        {
            olog("error: dc_net_simple_request: %d", p->sid);
            rv = -1;
            goto _end;
        }
        /*
        olog("request [%s:%s] got reply", p->ip, p->port);
        */

        olog("got-reply: code: [%s]", svc_out.code);
        olog("got-reply: table:[%s]", svc_out.table);
        olog("got-reply: resp: [%s]", svc_out.resp);
        olog("got-reply: tm0:  [%s]", svc_out.timestamp0);
        olog("got-reply: tm1:  [%s]", svc_out.timestamp1);
        olog("got-reply: tm2:  [%s]", svc_out.timestamp2);

        if (memcmp(svc_out.resp, "00", 2) != 0)
        {
            elog("error: server[%s:%s] failure", p->ip, p->port);
            elog("error: job failure: %s:%s:%s",
                    svc_out.code, svc_out.table, svc_out.resp);
            rv = -1;
            goto _end;
        }
        else
        {
            olog("nice: job succeeds: %s:%s:%s",
                    svc_out.code, svc_out.table, svc_out.resp);
        }


    } /* for */

_end:

    olog("----------------------------------------------");
    for (i = 0; i < num; i++)
    {
        p = &addr_list[i];
        if (strlen(p->ip) == 0)
        {
#if DC_DEBUG
            olog("to-close: [%d]th no data, means the last.", i);
#endif
            break;
        }
        else
        {
        }

        if (p->sid >= 0)
        {
            olog("to-close: [%d]th -- %d, [%s:%s]", i, p->sid, p->ip, p->port);
            dc_net_closesock(p->sid);
        }
    }


    return rv;
}



/*
 *  Function:
 *          get ip:port list from config file
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_cluster_get_config(dc_nt_addr *_addr_list, int  _n)
{
    int  i   = 0;
    int  rv  = 0;
    int  num = 0;
    char section[64] = {0};
    char entry[64] = {0};
    char buffer[1024] = {0};
    char file_path[1024] = {0};

    char *sep = ":";
    char *ptr = NULL;

    /* 1. get config file path */
#if STARRING_INTEGRATE
    ptr = getenv("FAPWORKDIR");
    if (ptr == NULL)
    {
        elog("error: please set environment: export FAPWORKDIR=/path/");
        return -1;
    }
    snprintf(file_path, sizeof(file_path), "%s/etc/cache.cfg", ptr);
#else
    ptr = getenv("DC_CACHE_CONFIG");
    if (ptr == NULL)
    {
        elog("error: please set environment: export DC_CACHE_CONFIG=/path/a.txt");
        return -1;
    }
    snprintf(file_path, sizeof(file_path), "%s", ptr);
#endif
    olog("config-file-path: %s", file_path);

    /* 2. get ip:port list */
    strcpy(section, "CLUSTER");
    strcpy(entry, "CNT");
    memset(buffer, 0, sizeof(buffer));
    rv = dc_cfg_get_value(section, entry, file_path, buffer, sizeof(buffer));
    if (rv < 0)
    {
        elog("error: dc_cfg_get_value: %s:%s", section, entry);
        return -1;
    }
    num = atoi(buffer);
#if DC_DEBUG
    olog("host number: %d(%s)", num, buffer);
#endif

    if (num > _n)
    {
        elog("too much data: %d > %d", num, _n);
        return -1;
    }

    for (i = 0; i < num; i++)
    {
        snprintf(entry, sizeof(entry), "HOST_%d", i);
        memset(buffer, 0, sizeof(buffer));
        rv = dc_cfg_get_value(section, entry, file_path, buffer, sizeof(buffer));
        if (rv < 0)
        {
            elog("error: dc_cfg_get_value: %s:%s", section, entry);
            return -1;
        }
        olog("got: %s ==> %s", entry, buffer);

        /* IP */
        ptr = strtok(buffer, sep);
        if (ptr == NULL)
        {
            elog("error: invalid data: %s", buffer);
            return -1;
        }
        snprintf(_addr_list[i].ip, sizeof(_addr_list[i].ip), "%s", ptr);

        /* PORT */
        ptr = strtok(NULL, sep);
        if (ptr == NULL)
        {
            elog("error: invalid data: %s", buffer);
            return -1;
        }
        snprintf(_addr_list[i].port, sizeof(_addr_list[i].port), "%s", ptr);
        _addr_list[i].sid = -1;
#if DC_DEBUG
        olog("add: %s:%s", _addr_list[i].ip, _addr_list[i].port);
#endif

    } /* for */

    return num;
}

/*
 *  Function:
 *      execute some job in cluster mode
 *     +parallel request
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_cluster_parallel(char *_table_name, char *_action, int _life)
{
    int  i   = 0;
    int  n   = 0;
    int  rv  = 0;
    int  num = 0;
    int  len = 0;
    int  sid = 0;
    int  cnt  = 0;
    int  size = 0;
    int  life = 5;
    int  max_fd = 0;
    int  succ_num = 0;
    int  interval  = 1;
    long curr_time = 0;
    long begin_time = 0;
    int  diff_time = 0;

    fd_set read_fd;
    struct timeval tmout;

#define DC_ADDR_MAX    100
    dc_nt_addr addr_list[DC_ADDR_MAX];
    dc_nt_addr *p = NULL;

    dc_cache_api_svc_t svc_in;
    dc_cache_api_svc_t svc_out;

#if DC_DEBUG
    olog("code:  %s", _action);
    olog("table: %s", _table_name);
#endif

    /* get ip:port list from config file */
    memset(&addr_list, 0, sizeof(addr_list));
    num = dc_cache_api_cluster_get_config(addr_list, DC_ADDR_MAX);
    if (num < 0)
    {
        elog("error:  dc_cache_api_cluster_get_config: %d", num);
        return -1;
    }


    /* 3. loop: connect */
    for (i = 0; i < num; i++)
    {
        p = &addr_list[i];
        if (strlen(p->ip) == 0)
        {
#if DC_DEBUG
            olog("[%d]th no data, means the last.", i);
#endif
            break;
        }
        else
        {
            olog("----------------------------------------------");
            olog("to-connect[%d]th -- [%s:%s]", i, p->ip, p->port);
        }


        sid = dc_net_simple_connect(p->ip, p->port);
        if (sid < 0)
        {
            olog("error: dc_net_simple_connect: %s:%s",  p->ip, p->port);
            p->sid = -1;
        }
        else
        {
            p->sid = sid;

            olog("to-request[%d]th -- [%s:%s@%d]", i, p->ip, p->port, p->sid);

            memset(&svc_in,  0, sizeof(svc_in));
            snprintf(svc_in.code,       sizeof(svc_in.code),  "%s", _action);
            snprintf(svc_in.table,      sizeof(svc_in.table), "%s", _table_name);
            curr_time = get_time();
            dctime_format_micro(curr_time, svc_in.timestamp0, sizeof(svc_in.timestamp0));

            len  = sizeof(svc_in);

            /* send */
            cnt = dc_net_send_nbytes(p->sid, (char *)&svc_in, len);
            if (cnt != len)
            {
                elog("error: dc_net_send_nbytes: %d@%d", cnt, p->sid);
                dc_net_closesock(p->sid);
                p->sid = -1;
            }
            else
            {
                olog("sent %d bytes", cnt);
            }
        } /* connected */
    } /* loop: connect()+send() */

    olog("----------------------------------------------");

    begin_time = dc_get_time_of_seconds();
    while (1)
    {
        /* check total timeout */
        diff_time = dc_get_time_of_seconds() - begin_time;
        if (diff_time > _life)
        {
            olog("it has cose all time: %d > %d",  diff_time, _life);
            break;
        }

        /* set fd_set */
        {
            FD_ZERO( &read_fd );
            max_fd = -1;

            for (i = 0; i < num; i++)
            {
                p = &addr_list[i];
                if (p->sid >= 0)
                {
#if DC_DEBUG
                    olog("to-monitor:%dth [%s:%s@%d]", i, p->ip, p->port, p->sid);
#endif
                    FD_SET( p->sid, &read_fd );

                    if (p->sid > max_fd)
                    {
#if DC_DEBUG
                        olog("---refreshes: %d --> %d", max_fd, p->sid);
#endif
                        max_fd = p->sid;
                    }
                }
            } /* fd_set */
        } /* fd_set */

        /* check no more socket */
#if DC_DEBUG
        olog("max-fd: %d", max_fd);
#endif
        if (max_fd < 0)
        {
            olog("no more socket to wait");
            break;
        }

        tmout.tv_sec = interval;
        tmout.tv_usec = 0;

        n = 0;
        n = dc_net_select( max_fd+1, &read_fd, 0, 0, &tmout );
        switch(n)
        {
            case -1:
                if (errno == EINTR)
                {
                    elog("warn: interrupted, but continue");
                    continue;
                }
                else
                {
                    elog("error: dc_net_select: %d, %s", errno, strerror(errno));
                    rv = -1;
                    goto _end;
                }
            case 0:
                /* time out */
                olog("waken, no data coming in %d seconds", tmout.tv_sec);
                break;
            default:
                olog("nice, %d fd is avaiable", n);
                break;
        }

        /* some sockets are readable */
        if (n > 0)
        {
            for (i = 0; i < num; i++)
            {
                p = &addr_list[i];
                if (p->sid >= 0)    /*  which means has connected */
                {
                    olog("----------------------------------------------");
                    if (FD_ISSET(p->sid, &read_fd))
                    {
                        olog("socket[%d] is readable %s:%s", p->sid, p->ip, p->port);

                        if ((rv = dc_net_getsockerr( p->sid)) == 0)
                        {
                            size = sizeof(svc_out);
                            memset(&svc_out, 0, sizeof(svc_out));
                            cnt = dc_net_recv_nbytes(p->sid, (char *)&svc_out, size, life);
                            if (cnt <= 0)
                            {
                                elog("error: dc_net_recv_nbytes: %d", cnt);
                                dc_net_closesock(p->sid);
                                p->sid = -1;
                                continue;
                            }
                            else
                            {
#if DC_DEBUG
                                olog("received bytes: %d", cnt);
#endif
                                /* close socket anyway */
                                dc_net_closesock(p->sid);
                                p->sid = -1;

                                olog("got-reply: code: [%s]", svc_out.code);
                                olog("got-reply: table:[%s]", svc_out.table);
                                olog("got-reply: resp: [%s]", svc_out.resp);
                                olog("got-reply: tm0:  [%s]", svc_out.timestamp0);
                                olog("got-reply: tm1:  [%s]", svc_out.timestamp1);
                                olog("got-reply: tm2:  [%s]", svc_out.timestamp2);

                                if (memcmp(svc_out.resp, "00", 2) != 0)
                                {
                                    elog("error: server[%s:%s] failure", p->ip, p->port);
                                    elog("error: job failure: %s:%s:%s",
                                            svc_out.code, svc_out.table, svc_out.resp);
                                    rv = -1;
                                }
                                else
                                {
                                    succ_num++;
                                    olog("nice: job succeeds: %s:%s:%s, total(%d)",
                                            svc_out.code, svc_out.table, svc_out.resp,  succ_num);
                                }
                            } /* data received */
                        } /* no socket error */
                        else
                        {
                            olog("error: socket [%s:%s@%d] failure: [%d]", p->ip,  p->port, p->sid,  rv);
                            dc_net_closesock(p->sid);
                            p->sid = -1;
                        }
                    } /* readable */
                    else
                    {
                        olog("socket[%d] NOT ready %s:%s", p->sid, p->ip, p->port);
                    }

                } /* valid socket */
            } /* for all client */
        }

    } /* loop always to get result */


_end:

    olog("----------------------------------------------");
    for (i = 0; i < num; i++)
    {
        p = &addr_list[i];
        if (p->sid >= 0)
        {
            olog("to-close: [%d]th -- %d, [%s:%s]", i, p->sid, p->ip, p->port);
            dc_net_closesock(p->sid);
            p->sid = -1;
        }
    }

    if (succ_num == num)
    {
        rv = 0;
        olog("all  requests(%d/%d) have succeed!",  succ_num, num);
    }
    else
    {
        rv = -1;
        elog("part requests(%d/%d) had failed!",  num-succ_num, num);
    }


    return rv;
}


int dc_cache_api_cluster_set_life(int _life)
{
    g_cluster_life = _life;

    return 0;
}


int dc_cache_api_cluster_get_life()
{
    return g_cluster_life;
}


/*
 *  Function:
 *      Reload table in cluster mode
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_reload_cluster_serial(char *_table_name)
{
    int  rv  = 0;

    rv = dc_cache_api_cluster(_table_name, DC_CODE_RELOAD);
    if (rv)
    {
        olog("error: dc_cache_api_cluster: %s", _table_name);
        return -1;
    }

    return rv;
}


/*
 *  Function:
 *      Enable table in cluster mode
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_enable_table_cluster_serial(char *_table_name)
{
    int  rv  = 0;

    rv = dc_cache_api_cluster(_table_name, DC_CODE_ENABLE);
    if (rv)
    {
        olog("error: dc_cache_api_cluster: %s", _table_name);
        return -1;
    }

    return rv;
}


/*
 *  Function:
 *      Disable table in cluster mode
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_disable_table_cluster_serial(char *_table_name)
{
    int  rv  = 0;

    rv = dc_cache_api_cluster(_table_name, DC_CODE_DISABLE);
    if (rv)
    {
        olog("error: dc_cache_api_cluster: %s", _table_name);
        return -1;
    }


    return rv;
}



/*
 *  Function:
 *      Reload table in cluster mode
 *     +parallel request
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_reload_cluster(char *_table_name)
{
    int  rv  = 0;
    int  life = 0;
    long begin= 0;

    /* get life time */
    life = dc_cache_api_cluster_get_life();
    if (life <= 0)
    {
        life = DC_CLUSTER_LIFE_TIME;
    }

    begin = get_time();
    rv = dc_cache_api_cluster_parallel(_table_name, DC_CODE_RELOAD, life);
    olog("cluster [%s] cost %d us", DC_CODE_RELOAD, get_time()-begin);
    if (rv)
    {
        olog("error: dc_cache_api_cluster_parallel: RELOAD: %s", _table_name);
        return -1;
    }

    return rv;
}


/*
 *  Function:
 *      Enable table in cluster mode
 *     +parallel request
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_enable_table_cluster(char *_table_name)
{
    int  rv  = 0;
    int  life = 0;
    long begin= 0;

    /* get life time */
    life = dc_cache_api_cluster_get_life();
    if (life <= 0)
    {
        life = DC_CLUSTER_LIFE_TIME;
    }

    begin = get_time();
    rv = dc_cache_api_cluster_parallel(_table_name, DC_CODE_ENABLE, life);
    olog("cluster [%s] cost %d us", DC_CODE_ENABLE, get_time()-begin);
    if (rv)
    {
        olog("error: dc_cache_api_cluster_parallel: ENABLE: %s", _table_name);
        return -1;
    }

    return rv;
}


/*
 *  Function:
 *      Disable table in cluster mode
 *     +parallel request
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_disable_table_cluster(char *_table_name)
{
    int  rv  = 0;
    int  life = 0;
    long begin= 0;

    /* get life time */
    life = dc_cache_api_cluster_get_life();
    if (life <= 0)
    {
        life = DC_CLUSTER_LIFE_TIME;
    }

    begin = get_time();
    rv = dc_cache_api_cluster_parallel(_table_name, DC_CODE_DISABLE, life);
    olog("cluster [%s] cost %d us", DC_CODE_DISABLE, get_time()-begin);
    if (rv)
    {
        olog("error: dc_cache_api_cluster_parallel: DISABLE: %s", _table_name);
        return -1;
    }

    return rv;
}



/*
 *  Function:
 *      dump table
 *      Use lock
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_dump_table(char *_table_name)
{
    int  rv  = 0;
    int  ready = 0;
    int  sem_id = -1;
    int  seg_idx = 0;
    void *shm_addr = NULL;
    void *blk_addr = NULL;
    void *seg_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

    /* +: read block A */
    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("read block addr: %p", blk_addr);
#endif

    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid: %p", blk_addr);
        return -1;
    }

    /* +: get index of segment*/
    seg_idx = dc_blk_get_segment_index(blk_addr, _table_name);
    if (seg_idx < 0)
    {
        elog("error: dc_blk_get_segment_index: %s", _table_name);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("segment index is <%d>", seg_idx);
#endif

    seg_addr = dc_blk_get_segment_addr(blk_addr, seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr: not found '%d'", seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("segment addr is <%p>", seg_addr);
#endif

    /* read(shared) lock */
    if (dc_shared_lock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_lock: %d, %d", sem_id, seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("read locked");
#endif

    dc_query_dump_segment(seg_addr);

    /* unlock */
    if (dc_shared_unlock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_unlock: %d, %d", sem_id, seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("read unlocked");
#endif

_end:

    return rv;
}


/*
 *  Function:
 *      dump all tables
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_dump_block()
{
    int  rv  = 0;
    int  ready = 0;
    char *name = NULL;
    void *shm_addr = NULL;
    void *blk_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }
#if DC_DEBUG
    olog("shm addr: %p", shm_addr);
#endif

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

    /* +: read block A */
    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("read block addr: %p", blk_addr);
#endif

    dc_blk_iterator_init(blk_addr);

    while ((name = dc_blk_iterator_next()) != NULL)
    {
        olog("table: %s", name);
        dc_cache_api_dump_table(name);
    }

    dc_blk_iterator_end();

_end:

    return rv;
}


/*
 *  Function:
 *      dump block basic info
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_list_tables()
{
    int  rv  = 0;
    int  ready = 0;
    int  sem_id = 0;
    int  seg_idx = 0;
    char *name = NULL;
    void *shm_addr = NULL;
    void *blk_addr = NULL;
    void *seg_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }
#if DC_DEBUG
    olog("shm addr: %p", shm_addr);
#endif

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

    /* +: read block A */
    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("read block addr: %p", blk_addr);
#endif


    /* lock */
    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid: %p", blk_addr);
        return -1;
    }

    printf("=====================================\n");

    dc_blk_iterator_init(blk_addr);
    while ((name = dc_blk_iterator_next()) != NULL)
    {
        printf("%s\n", name);
        seg_idx = dc_blk_get_segment_index(blk_addr, name);
        if (seg_idx < 0)
        {
            elog("error: dc_blk_get_segment_index: %s", name);
            rv = -1;
            goto _end;
        }

        seg_addr = dc_blk_get_segment_addr(blk_addr, seg_idx);
        if (seg_addr == NULL)
        {
            elog("error: dc_blk_get_segment_addr: %d", seg_idx);
            rv = -1;
            goto _end;
        }

        /* read(shared) lock */
        if (dc_shared_lock(sem_id, seg_idx) < 0)
        {
            elog("error: dc_shared_lock: %d, %d", sem_id, seg_idx);
            return -1;
        }

        dc_query_dump_segment_info(seg_addr);
        printf("=====================================\n");

        /* unlock */
        if (dc_shared_unlock(sem_id, seg_idx) < 0)
        {
            elog("error: dc_shared_unlock: %d, %d", sem_id, seg_idx);
            return -1;
        }
    }
    fflush(stdout);

    dc_blk_iterator_end();

_end:

    return rv;
}


/*
 *  Function:
 *      describe a table
 *      Use lock
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_desc_table(char *_table_name)
{
    int  rv  = 0;
    int  ready = 0;
    int  sem_id = -1;
    int  seg_idx = 0;
    void *shm_addr = NULL;
    void *blk_addr = NULL;
    void *seg_addr = NULL;

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

    /* +: read block A */
    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("read block addr: %p", blk_addr);
#endif

    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid: %p", blk_addr);
        return -1;
    }

    /* +: get index of segment*/
    seg_idx = dc_blk_get_segment_index(blk_addr, _table_name);
    if (seg_idx < 0)
    {
        elog("error: dc_blk_get_segment_index: %s", _table_name);
        rv = -1;
        goto _end;
    }
#if DC_DEBUG
    olog("segment index is <%d>", seg_idx);
#endif

    seg_addr = dc_blk_get_segment_addr(blk_addr, seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr: not found '%d'", seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("segment addr is <%p>", seg_addr);
#endif

    /* read(shared) lock */
    if (dc_shared_lock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_lock: %d, %d", sem_id, seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("read locked");
#endif

    printf("=====================================\n");
    printf("%s\n", _table_name);
    dc_query_dump_segment_info(seg_addr);
    printf("=====================================\n");
    dc_query_dump_segment_column(seg_addr);
    printf("=====================================\n");

    /* unlock */
    if (dc_shared_unlock(sem_id, seg_idx) < 0)
    {
        elog("error: dc_shared_unlock: %d, %d", sem_id, seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("read unlocked");
#endif
    fflush(stdout);

_end:

    return rv;
}


/*
 *  Function:
 *      shm basic info
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_cache_api_shm_info()
{
    int  rv  = 0;
    int  num = 0;
    int  ready = 0;
    int  mutex = 0;
    int  sem_id = 0;
    long gate = 0;
    char *name = NULL;
    void *shm_addr = NULL;
    void *blk_addr = NULL;

    printf("=====================================\n");

    shm_addr = dc_shm_get_addr();
    if (shm_addr == NULL)
    {
        elog("error: dc_shm_get_addr");
        return -1;
    }
    printf("addr:  %p\n", shm_addr);


    if (dc_cache_api_discarded())
    {
        printf("cache: DISABLE\n");
    }
    else
    {
        printf("cache: ENABLE\n");
    }

    /* check READY */
    ready = dc_shm_get_ready(shm_addr);
    if (!ready)
    {
        elog("error: not ready: %d", ready);
        return -1;
    }

    mutex = dc_shm_get_semid(shm_addr);
    if (mutex < 0)
    {
        elog("error: dc_shm_get_semid");
        rv = -1;
        goto _end;
    }
    printf("mutex: %d\n", mutex);

    /* gate */
    gate = dc_shm_get_gate(shm_addr);
    if (gate == DC_SHM_REGION_A)
    {
        printf("using: A-REGION\n");
    }
    else if (gate == DC_SHM_REGION_B)
    {
        printf("using: B-REGION\n");
    }
    else
    {
        printf("FATAL: unknown gate: %ld\n", gate);
    }

    /* block-A */
    blk_addr = dc_shm_get_block_A_addr(shm_addr);
    if (blk_addr == NULL)
    {
        elog("error: dc_shm_get_block_A_addr");
        rv = -1;
        goto _end;
    }

    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid: %p", blk_addr);
        return -1;
    }
    printf("lockA: %d\n", sem_id);


    /* block-B */
    blk_addr = dc_shm_get_block_B_addr(shm_addr);
    if (blk_addr == NULL)
    {
        elog("error: dc_shm_get_block_B_addr");
        rv = -1;
        goto _end;
    }

    sem_id = dc_blk_get_semid(blk_addr);
    if (sem_id < 0)
    {
        elog("error: dc_blk_get_semid: %p", blk_addr);
        return -1;
    }
    printf("lockB: %d\n", sem_id);


    /* read block */
    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        rv = -1;
        goto _end;
    }


    num = dc_blk_get_segment_number(blk_addr);
    if (num < 0)
    {
        elog("error: dc_blk_get_segment_number");
        rv = -1;
        goto _end;
    }

    if (num == 0)
    {
        printf("no table\n");
    }
    else if (num == 1)
    {
        printf("has %d table, as:\n", num);
    }
    else
    {
        printf("has %d tables, as:\n", num);
    }

    dc_blk_iterator_init(blk_addr);
    while ((name = dc_blk_iterator_next()) != NULL)
    {
        printf("==> %s\n", name);
    }
    dc_blk_iterator_end();

    printf("=====================================\n");
    fflush(stdout);


_end:

    return rv;
}


/*
 *  Function:
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 1
 *      FAILURE - 0
 */
int dc_cache_api_is_already_open()
{
    if (dc_shm_get_addr() != NULL)
    {
        return 1;
    }

    return 0;
}


/*
 *  Function:
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS - 1
 *      FAILURE - 0
 */
int dc_cache_api_is_aggregation_count(int *_num)
{
    int  rv = 0;
    int  agg = 0;

    agg = dc_sql_is_aggregation();

    if (agg < 0)
    {
        rv = -1;
        elog("error: dc_sql_is_aggregation");
    }
    else if (agg == 1)
    {
        rv = 1;
        *_num = dc_rs_get_num();
    }

    return rv;
}


/****************************************************************************** 
  2017-8-1
  compatible cache&db
 ******************************************************************************/
static int g_data_from_cache = 0;
static int g_has_allocated   = 0;
static int g_is_first_time   = 0;

int dc_malloc_area(void)
{
    int area = 0;

    area = MallocArea();
    if (-1 == area)
    {
        elog("error: MallocArea()");
        return -1;
    }

    g_data_from_cache = 0;  /* default get data from DB */
    g_has_allocated = 1;
    g_is_first_time = 1;

    dc_put_init();

    return area;
}


int dc_free_area(void)
{
    g_has_allocated = 0;

    dc_put_reset();

    return FreeArea();
}


int dc_db_select(int _area , char *_statement, char _op_flag, long _offset)
{
    int rv = 0;

    if (!g_has_allocated)
    {
        elog("error: should call CAMallocArea() preceding!");
        return -1;
    }

    if (dc_cache_api_discarded())
    {
        if (g_is_first_time)
        {
            g_is_first_time = 0;
            olog("warn: CACHE mode disabled");
        }

        /* get data from DB */
        rv = DBSelect(_area, _statement, _op_flag, _offset);
        g_data_from_cache = 0;
#if DC_DEBUG
        olog("DB mode: force: %d", rv);
#endif
        return rv;
    }


    if (g_is_first_time)
    {
        g_is_first_time = 0;
#if DC_DEBUG
        olog("query first time");
#endif

        /* first time: do cache query anyhow */
        rv = dc_cache_api_select(_statement);
        if (rv == 0)
        {
            /* not found */
#if DC_DEBUG
            olog("CA mode: first: no data");
#endif
            g_data_from_cache = 1;
            rv = E_NOTFOUND;

            /* for count(1) case, return E_OK. 2017-10-10 */
            if (dc_sql_is_aggregation() == 1)
            {
                rv = E_OK;
            }
        }
        else if (rv > 0)
        {
            /* found */
#if DC_DEBUG
            olog("CA mode: first: rs: %d", rv);
#endif
            g_data_from_cache = 1;
            rv = E_OK;
        }
        else
        {
            /* error */
#if DC_DEBUG
            olog("error: dc_cache_api_select");
#endif

            /* get data from DB */
            rv = DBSelect(_area, _statement, _op_flag, _offset);
            g_data_from_cache = 0;
#if DC_DEBUG
            olog("DB mode: first: %d", rv);
#endif
        }
    }
    else
    {
        /* fetch next data */
        if (g_data_from_cache)
        {
            if (dc_cache_api_fetch() != NULL)
            {
                rv = E_OK;
#if DC_DEBUG
                olog("CA mode: fetch next");
#endif
            }
            else
            {
                /* error */
                rv = E_NOTFOUND;
#if DC_DEBUG
                olog("CA mode: no more data");
#endif
            }
        }
        else
        {
            rv = DBSelect(_area, _statement, _op_flag, _offset);
#if DC_DEBUG
            olog("DB mode: fetch rv: %d", rv);
#endif
        }
    }

    return rv;
}


int dc_db_put_data(int _data_id, int _data_type, void * _data)
{
    if (!g_has_allocated)
    {
        elog("error: should call CAMallocArea() preceding!");
        return -1;
    }

    if (dc_query_put_data(_data_id, _data_type , _data))
    {
        elog("error: dc_query_put_data: %d", _data_id);
        return -1;
    }
    else
    {
#if DC_DEBUG
        olog("CA mode: put one data");
#endif
    }

    if (DBPutData(_data_id, _data_type, _data) == E_FAIL)
    {
        elog("error: DBPutData: %d", _data_id);
        return -1;
    }
    else
    {
#if DC_DEBUG
        olog("DB mode: put one data");
#endif
    }

    return 0;
}


int dc_db_get_data(int _data_id , void *_data , int _size)
{
    if (!g_has_allocated)
    {
        elog("error: should call CAMallocArea() preceding!");
        return -1;
    }

    if (g_data_from_cache)
    {
#if DC_DEBUG
        olog("CA mode: get column: %d", _data_id);
#endif
        if (dc_cache_api_get_data(_data_id, _data, _size) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
    }
    else
    {
#if DC_DEBUG
        olog("DB mode: get column: %d", _data_id);
#endif
        if (DBGetData(_data_id, _data, _size) == E_FAIL)
        {
            elog("error: DBGetData: %d", _data_id);
            return -1;
        }
    }

    return 0;
}


int dc_db_get_stru(int _data_id , void *_data , int _size)
{
    if (!g_has_allocated)
    {
        elog("error: should call CAMallocArea() preceding!");
        return -1;
    }

    if (g_data_from_cache)
    {
#if DC_DEBUG
        olog("CA mode: get struct: %d", _data_id);
#endif
        if (dc_query_get_stru(_data, _size) < 0)
        {
            elog("error: dc_query_get_stru");
            return -1;
        }
    }
    else
    {
#if DC_DEBUG
        olog("DB mode: get struct: %d", _data_id);
#endif
        if (DBGetStru(_data_id, _data, _size) == E_FAIL)
        {
            elog("error: DBGetStru: %d", _data_id);
            return -1;
        }
    }

    return 0;
}


#if RUN_API
#if ADMIN
static int dc_cache_api_simulate_admin_input()
{
    int  max_rows = 0;
    key_t shm_key = 0;
    char table_name[32] = {0};

    dc_aux_init();

    /* external (user) invoke */
    snprintf(table_name, sizeof(table_name), "%s", "TBL_LOG_cOuNTER");
    max_rows = 10;
    if (dc_aux_seg_add(table_name, max_rows))
    {
        elog("error: dc_aux_seg_add: %s", table_name);
        return -1;
    }

    snprintf(table_name, sizeof(table_name), "%s", "tBL_CFG_TXN");
    max_rows = 100;
    if (dc_aux_seg_add(table_name, max_rows))
    {
        elog("error: dc_aux_seg_add: %s", table_name);
        return -1;
    }

    snprintf(table_name, sizeof(table_name), "%s", "TbL_CFG_LINE");
    max_rows = 100;
    if (dc_aux_seg_add(table_name, max_rows))
    {
        elog("error: dc_aux_seg_add: %s", table_name);
        return -1;
    }
#if 0
#endif

    return 0;
}
#endif


#if ADMIN
static int dc_cache_api_function_test1()
{
    int  rv = 0;
    int  idx = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }

    olog("select succeeds!");

    idx = 0;
    memset(v, 0, sizeof(v));
    if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
    {
        elog("error: dc_cache_api_get_data");
        return -1;
    }
    olog("column value:%s", v);

#if 0
    olog("----------------------------------------");
    olog("sql2: TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509' ");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    olog("sql3: TBL_CFG_LINE"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT REMOTE_ADDR, REMOTE_PORT  FROM TBL_CFG_LINE "
            "WHERE SRV_NAME ='CommCredit' AND REMOTE_SVC_NAME =  'YL03' "
            " AND PROTOCOL  = 'XML' ");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }


    olog("----------------------------------------");
    olog("sql4: TBL_CFG_TXN"); 
    snprintf(sql, sizeof(sql), "%s",
            "SELECT DSP, LIVE_TIME FROM TBL_CFG_TXN "
            "WHERE TXN_NUM =  '1073' ");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }


    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER, to hit!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }


    olog("----------------------------------------");
    olog("sql5: TBL_LOG_COUNTER, new!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'     ");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    olog("sql6: TBL_LOG_COUNTER, new and pop!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'      ");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }

    olog("----------------------------------------");

    /* XXX: remote updated */

    olog("----------------------------------------");
    olog("sql5: TBL_LOG_COUNTER, table obsolete!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'     ");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER, already deleted!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'");
    if (dc_cache_api_select(sql) < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }
#endif

    olog("----------------------------------------");

    /*********************************************************************/

    return rv;
}
#endif


#if USER
/**
 * multiple rows
 */
static int dc_cache_api_function_test2()
{
    int  rv = 0;
    int  idx = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE TXN_TYPE=  'ZHXXYZ' ");

    if (dc_cache_api_cursor(sql) < 0)
    {
        elog("error: dc_cache_api_cursor: %s", sql);
        return -1;
    }

    while (dc_cache_api_fetch() != NULL)
    {
        idx = 0;
        memset(v, 0, sizeof(v));
        if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
        olog("      ===>>: %s", v);
    }

    olog("----------------------------------------");

    /*********************************************************************/

    return rv;
}

/**
 * HIS
 */
static int dc_cache_api_function_test3()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509' ");

    rows = dc_cache_api_select(sql);
    if (rows < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }
    else if (rows == 0)
    {
        olog("not found");
    }
    else
    {
        idx = 0;
        memset(v, 0, sizeof(v));
        if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
        olog("      ===>>: %s", v);
    }

    olog("----------------------------------------");

    /*********************************************************************/

    return rv;
}

static int dc_cache_api_function_test4()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20171509' ");

    rows = dc_cache_api_select(sql);
    if (rows < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }
    else if (rows == 0)
    {
        olog("not found");
    }
    else
    {
        idx = 0;
        memset(v, 0, sizeof(v));
        if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
        olog("      ===>>: %s", v);
    }

    olog("----------------------------------------");

    /*********************************************************************/

    return rv;
}


static int dc_cache_api_function_test5()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    snprintf(sql, sizeof(sql), "%s", 
            "SElECT inst_DATE_TIME, SUCC_tOTAL_NUM  fROM tbl_Log_counter "
            "WHErE pan ='6216920000008264' AnD TXN_TYPE=  'ZHXXYZ' "
            " ANd TXn_DATE = '20170509' ");

    snprintf(sql, sizeof(sql), "%s", 
            "SElECT inst_DATE_TIME, SUCC_tOTAL_NUM  fROM tbl_Log_counter "
            "WHErE pan ='6216920000008264' AnD TXN_TYPE=  'ZHXXYZ' "
            " OR TXn_DATE = '20170509' ");

    rows = dc_cache_api_select(sql);
    if (rows < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }
    else if (rows == 0)
    {
        olog("not found");
    }
    else
    {
        idx = 0;
        memset(v, 0, sizeof(v));
        if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
        olog("      ===>>: %s", v);
    }

    olog("----------------------------------------");

    /*********************************************************************/

    return rv;
}


static int dc_cache_api_function_test6()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    snprintf(sql, sizeof(sql), "%s", "SELECT plat_date FROM t_plat_para");
    snprintf(sql, sizeof(sql), "%s", "SELECT * FROM t_plat_para");

    rows = dc_cache_api_select(sql);
    if (rows < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }
    else if (rows == 0)
    {
        olog("not found");
    }
    else
    {
        idx = 0;
        memset(v, 0, sizeof(v));
        if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
        olog("      ===>>: %s", v);
    }

    olog("----------------------------------------");

    /*********************************************************************/

    return rv;
}


/* unified API: ORACLE */
static int dc_cache_api_function_test7()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    int  area = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char v2[32] = {0};

    typedef struct _dept_t
    {
        char dept_no[8+1];
        char name[40+1];
    } dept_t;

    dept_t st;

    olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    snprintf(sql, sizeof(sql), "%s", "SELECT * FROM DEPT");
    snprintf(sql, sizeof(sql), "%s", "SELECT * FROM tbl_log_counter");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        elog("error: CAMallocArea");
        return -1;
    }

    while (1)
    {
        rv = CADBSelect(area, sql, 0, 0);

        if (E_FAIL == rv)
        {
            CAFreeArea();
            olog("error: CADBSelect");
            return E_FAIL;
        }

        if ( rv == E_NOTFOUND )
        {
            olog("no more data");
            rv = 0;
            break;
        }

        rows++;

        idx = 0;
        memset(v, 0, sizeof(v));
        if (CADBGetData(idx, v, sizeof(v)) < 0)
        {
            elog("error: CADBGetData");
            return -1;
        }

        /*
        idx = 1;
        memset(v2, 0, sizeof(v2));
        if (CADBGetData(idx, v2, sizeof(v2)) < 0)
        {
            elog("error: CADBGetData");
            return -1;
        }
        olog("      ===>> [%s, %s]", v, v2);

        memset(&st, 0, sizeof(st));
        if (CADBGetStru(0, &st, sizeof(st)) < 0)
        {
            elog("error: CADBGetStru");
            return -1;
        }
        olog("      +++>> [%s, %s]", st.dept_no, st.name);
        */
    }

    CAFreeArea();

    olog("rows: %d", rows);
    olog("----------------------------------------");

    return rv;
}


/* unified API: DB2 */
static int dc_cache_api_function_test8()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    int  area = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char v2[32] = {0};

    typedef struct _bin_t
    {
        char bin[6+1];
        char name[50+1];
    } bin_t;

    bin_t st;

    olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    snprintf(sql, sizeof(sql), "%s", "SELECT * FROM T_CREDIT_BIN ");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        elog("error: CAMallocArea");
        return -1;
    }

    while (1)
    {
        rv = CADBSelect(area, sql, 0, 0);

        if (E_FAIL == rv)
        {
            CAFreeArea();
            olog("error: CADBSelect");
            return E_FAIL;
        }

        if ( rv == E_NOTFOUND )
        {
            olog("no more data");
            rv = 0;
            break;
        }

        rows++;

        idx = 0;
        memset(v, 0, sizeof(v));
        if (CADBGetData(idx, v, sizeof(v)) < 0)
        {
            elog("error: CADBGetData");
            return -1;
        }

        idx = 1;
        memset(v2, 0, sizeof(v2));
        if (CADBGetData(idx, v2, sizeof(v2)) < 0)
        {
            elog("error: CADBGetData");
            return -1;
        }
        olog("      ===>> [%s, %s]", v, v2);

        memset(&st, 0, sizeof(st));
        if (CADBGetStru(0, &st, sizeof(st)) < 0)
        {
            elog("error: CADBGetStru");
            return -1;
        }
        olog("      +++>> [%s, %s]", st.bin, st.name);
    }

    CAFreeArea();

    olog("rows: %d", rows);
    olog("----------------------------------------");

    return rv;
}
#endif


int main(int argc, char *argv[])
{
    int  rv  = 0;
    long shm_key = 7788;

    (void)argc;
    (void)argv;

    olog("api begin");

#if ADMIN
    if (dc_cache_api_simulate_admin_input())
    {
        elog("error: dc_cache_api_simulate_admin_input");
        return -1;
    }

    /* connect to DB */
    if (dc_ext_connect_db() == -1)
    {
        elog("error: dc_ext_connect_db");
        return -1;
    }

    if (dc_cache_api_admin_init(shm_key))
    {
        elog("error: dc_cache_api_admin_init");
        return -1;
    }

    if (dc_cache_api_function_test1())
    {
        elog("error: dc_cache_api_function_test1");
        return -1;
    }

#endif

#if USER
    /* connect only for reload */
    if (dc_ext_connect_db() == -1)
    {
        elog("error: dc_ext_connect_db");
        return -1;
    }

    if (dc_cache_api_open(shm_key))
    {
        elog("error: dc_cache_api_open");
        return -1;
    }

    if (argc == 2)
    {
        olog("reload: %s", argv[1]);
        if (dc_cache_api_reload(argv[1]))
        {
            elog("error: dc_cache_api_reload: %s", argv[1]);
            return -1;
        }
        olog("reload succeeds");
    }
    else
    {

#if 0
        if (dc_cache_api_function_test3())
        {
            elog("error: dc_cache_api_function_test3");
            return -1;
        }

        if (dc_cache_api_function_test2())
        {
            elog("error: dc_cache_api_function_test2");
            return -1;
        }

        if (dc_cache_api_function_test3())
        {
            elog("error: dc_cache_api_function_test3");
            return -1;
        }

        if (dc_cache_api_reload("TBL_LOG_COUNTER"))
        {
            elog("error: dc_cache_api_reload: %s", argv[1]);
            return -1;
        }
        olog("reload succeeds");

        if (dc_cache_api_function_test3())
        {
            elog("error: dc_cache_api_function_test3");
            return -1;
        }

        if (dc_cache_api_function_test4())
        {
            elog("error: dc_cache_api_function_test4");
            return -1;
        }

        if (dc_cache_api_function_test5())
        {
            elog("error: dc_cache_api_function_test5");
            return -1;
        }

        if (dc_cache_api_dump_table("TBL_LOG_COUNTER"))
        {
            elog("error: dc_cache_api_dump_table");
            return -1;
        }

        if (dc_cache_api_dump_block())
        {
            elog("error: dc_cache_api_dump_block");
            return -1;
        }

        if (dc_cache_api_function_test6())
        {
            elog("error: dc_cache_api_function_test6");
            return -1;
        }
#endif

#if ORACLE
        if (dc_cache_api_function_test7())
        {
            elog("error: dc_cache_api_function_test7");
            return -1;
        }
#endif

#if DB2
        if (dc_cache_api_function_test8())
        {
            elog("error: dc_cache_api_function_test8");
        }
#endif

    }


#if 1
    int  action = 0;
    char table[32+1] = {0};

    strcpy(table, "tbl_log_counter");

    olog("cluster: %s", table);

    action = 3;
    if (action == 1)
    {
        olog("to reload table");
        rv = dc_cache_api_reload_cluster(table);
        if (rv)
        {
            elog("error: dc_cache_api_reload_cluster: %s", table);
            return -1;
        }
    }
    else if (action == 2)
    {
        olog("to enable table");
        rv = dc_cache_api_enable_table_cluster(table);
        if (rv)
        {
            elog("error: dc_cache_api_enable_table_cluster: %s", table);
            return -1;
        }
    }
    else if (action == 3)
    {
        olog("to disable table");
        rv = dc_cache_api_disable_table_cluster(table);
        if (rv)
        {
            elog("error: dc_cache_api_disable_table_cluster: %s", table);
            return -1;
        }
    }
    else
    {
        elog("error: ");
    }
    olog("cluster done: %s", table);
#endif


    if (dc_cache_api_close())
    {
        elog("error: dc_cache_api_close");
        return -1;
    }
#endif

    olog("api end");

    return rv;
}
#endif

/* api.c */
