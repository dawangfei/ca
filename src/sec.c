#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>
#include <stddef.h>

#include "type.h"
#include "log.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "sec.h"
#include "list.h"



/* ================================================================= */
/* ================================================================= */

/* ================================================================= 
   ================================================================= */


/**
 *  set memory to zero
 */
int dc_sec_memset(void *_data_unit, int _len)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;

    p->len = _len;
    memset(p->row, 0, _len);

    return 0;
}


/**
 * copy data to data-unit
 */
int dc_sec_memcpy(void *_data_unit, void *_data, int _len)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;

    p->len = _len;
    memcpy(p->row, _data, _len);

    return 0;
}


int dc_sec_reversed_link(void *_data_unit, int _ind_idx, void *_index_node)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;

    if (_ind_idx >= TABLE_MAX_INDEXES)
    {
        elog("error: index exceeds: %d", _ind_idx);
        return -1;
    }

    p->index_node[_ind_idx] = _index_node;

    return 0;
}


/**
 *  extract the specified node from list
 *  for with index mode
 *  and could be invoked only the first index
 */
int dc_sec_delete(void *_data_unit)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;

    list_del(&p->link);

    return 0;
}


/**
 * insert specified node to list
 */
int dc_sec_insert(List *_list, void *_data_unit)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;

    list_add(_list, &p->link);

    return 0;
}


/**
 *  It's used for no index case.
 */
void *dc_sec_extract(List *_list)
{
    dc_sec_t *p = NULL;

    p = list_entry(_list->head->next, DC_TYPEOF(*p), link);
    if (&p->link == _list->head)
    {
        olog("no more data node");
        return NULL;
    }
    else
    {
        list_del(&p->link);
    }

    return p;
}



int dc_sec_calc_size(int _row_size)
{
    return _row_size + sizeof(dc_sec_t);
}


/**
 *  get unit row address
 */
void *dc_sec_get_row(void *_data_unit)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;
    /*
    olog("offset: [%ld]", offsetof(dc_sec_t, row));
    */

    return p->row;
}


/**
 * set identifier for the unit
 */
void dc_sec_set_id(void *_data_unit, int _id)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;

    p->id = _id;

    return;
}


/**
 *  get id of unit row
 */
int dc_sec_get_id(void *_data_unit)
{
    dc_sec_t *p = NULL;

    p = (dc_sec_t *)_data_unit;

    return p->id;
}


#if RUN_SEC
int main(int argc, char *argv[])
{
    int  rv  = 0;

    (void)argc;
    (void)argv;

    olog("sec init");

    olog("offset: [%ld]", offsetof(dc_sec_t, row));

    olog("sec end");

    return rv;
}
#endif

/* sec.c */
