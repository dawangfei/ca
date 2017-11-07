#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "log.h"
#include "util.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "load.h"
#include "query.h"
#include "hash.h"
#include "list.h"

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"

#include "put.h"

#define  PUT_CONT_SIZE      64



static int               g_put_num  = 0;
static int               g_put_curr = 0;
static int               g_put_max  = PUT_CONT_SIZE;
static dc_put_t          g_put_cont[PUT_CONT_SIZE];


/* ================================================================= */

/* ================================================================= 
   ================================================================= */


/*
 *  Function:  dc_put_init
 *      init the put data vector
 *
 *  Parameteput:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_put_init(void)
{
    g_put_max = PUT_CONT_SIZE;

    dc_put_reset();

    return 0;
} /* dc_put_init */


void dc_put_destroy(void)
{
    olog("-->>>>%s", __func__);

    return;
}


/*
 *  Function:
 *      clear all data
 *
 *  Parameteput:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
void dc_put_reset()
{
    int i = 0;
    dc_put_t *p = NULL;

    for (i = 0; i < g_put_max; i++)
    {
        p = &g_put_cont[i];

        if (p->data.c_value != NULL)
        {
            if (p->len > 0)
            {
                free(p->data.c_value);
            }
            p->data.c_value = NULL;
        }
    }

    g_put_num = 0;
    memset(&g_put_cont, 0, sizeof(g_put_cont));
}



/*
 *  Function:
 *
 *
 *  Parameteput:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
int  dc_put_one(int _data_id , int _data_type , void * _data)
{
    int   rv = 0;
    dc_put_t *p = NULL;

    if (_data_id >= g_put_max)
    {
        elog("error: can't support %d > %d", _data_id, g_put_max);
        return -1;
    }

    p = &g_put_cont[_data_id];

    p->type = _data_type;

    switch (_data_type)
    {
        case DFFAPCHAR:
            p->len = strlen(_data);

            if (p->len > 0)
            {
                p->data.c_value = calloc(p->len+1, sizeof(char));
                if (p->data.c_value == NULL)
                {
                    elog("error: calloc: %d", p->len+1);
                    return -1;
                }

                memcpy(p->data.c_value, _data, p->len);
            }
            else
            {
#if DC_DEBUG
                olog("warn: no value: %d", p->len);
#endif
                p->data.c_value = "";
            }

            /* in case of duplicated set value */
            if (p->used == 0)
            {
                p->used = 1;
                g_put_num += 1;
            }

            rv = 0;
            break;
        default:
            p->used = 0;
            p->fail = 1;
            elog("error: don't support: %d type", _data_type);
            rv = -1;
            break;
    }

    return rv;
}


void dc_put_iterator_init()
{
    g_put_curr = 0;
}


/*
 *  Function:
 *      get next data
 *
 *  Parameteput:
 *
 *  Return Value:
 *      SUCCESS - one put of <dc_put_t>
 *      FAILURE - NULL
 */
void *dc_put_iterator_next()
{
    dc_put_t *p = NULL;

    if (g_put_curr >= g_put_max)
    {
        return NULL;
    }

    p = &g_put_cont[g_put_curr];

    if (p->used == 0)
    {
        /* no value */
        return NULL;
    }
    else if (p->fail)
    {
        /* not support */
        return NULL;
    }

    g_put_curr += 1;

    return p;
}



/*
 *  Function:
 *      get the number of put list
 *
 *  Parameteput:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
int dc_put_get_num()
{
    return g_put_num;
}


void dc_put_dump()
{
    int  i = 0;
    dc_put_t *p = NULL;

    olog("-->>>>%s", __func__);

    olog("put number: %d", g_put_num);

    for (i = 0; i < g_put_max; i++)
    {
        p = &g_put_cont[i];
        if (p->used)
        {
            if (p->fail)
            {
                olog("not support type: %d", p->type);
            }
            else
            {
                olog("[%d ==> %s]", i, p->data.c_value);
            }
        }
        else
        {
            break;
        }
    }
}


#if RUN_PUT
int main(int argc, char *argv[])
{
    int  i   = 0;
    int  rv  = 0;
    int  len = 0;
    int  max = 0;
    int  type = 0;
    char buffer[128] = {0};
    dc_put_t *p = NULL;

    (void)argc;
    (void)argv;

    olog("put begin");

    if (dc_put_init())
    {
        elog("error: dc_put_init");
        return -1;
    }

    olog("=========================================");

    /* called every time */
    dc_put_reset();

    olog("-----------------------------------------");

    olog("iterator begin");
    dc_put_iterator_init();

    while ((p = dc_put_iterator_next()) != NULL)
    {
        olog("data: [%p: %s]", p, p);
    }
    olog("iterator end");

    olog("-----------------------------------------");

    max = 3;
    for (i = 0; i < max; i++)
    {
        len = snprintf(buffer, sizeof(buffer), "%s--%ld", "abc", get_time());
        type = FAPCHAR;
        if (dc_put_one(i, type, buffer))
        {
            elog("error: dc_put_one: %d, %s", len, buffer);
            return -1;
        }
        else
        {
            olog("add [%s, %d] succeeds", buffer, len);
        }
    }
    olog("-----------------------------------------");

    type = FAPCHAR;
    strcpy(buffer, "666");
    if (dc_put_one(3, type, buffer))
    {
        elog("error: dc_put_one: %d, %s", len, buffer);
        return -1;
    }
    else
    {
        olog("add [%s, %d] succeeds", buffer, len);
    }
    olog("-----------------------------------------");

    dc_put_dump();

    olog("-----------------------------------------");
    dc_put_iterator_init();

    while ((p = dc_put_iterator_next()) != NULL)
    {
        olog("data: [%p: %s]", p, p->data.c_value);
    }
    olog("-----------------------------------------");

    /****************************************************/
    /****************************************************/
    /****************************************************/

    dc_put_destroy();

    olog("put end");
    return rv;
}
#endif

/* put.c */
