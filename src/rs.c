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

#include "rs.h"

#define  RS_CONT_SIZE   100000

/*  */
typedef struct {
    void    *row;
    int      row_len;
    int      id;
    Link     link;
} dc_rs_t;


static int               g_rs_list_num  = 0;
static List             *g_rs_list = NULL;
static dc_rs_t          *g_rs_curr = NULL;
static segment_header_t  g_rs_header;
static char              g_rs_cont[RS_CONT_SIZE];


/* ================================================================= */

/* ================================================================= 
   ================================================================= */


/*
 *  Function:  dc_rs_init
 *      init the result set
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_rs_init(void)
{

    if (g_rs_list == NULL)
    {
        g_rs_list = create_list();
        if (g_rs_list == NULL)
        {
            elog("error: create_list failure");
            return -1;
        }
    }

    return 0;
} /* dc_rs_init */


void dc_rs_destroy(void)
{
    olog("-->>>>%s", __func__);

    if (g_rs_list != NULL)
    {
        olog("let's destroy the list");

        if (g_rs_list_num > 0)
        {
            dc_rs_reset();
        }

        if (g_rs_list->head != NULL)
        {
            free(g_rs_list->head);
            g_rs_list->head = NULL;
        }

        free(g_rs_list);
        g_rs_list = NULL;
    }

    return;
}



/*
 *  Function:
 *      save segment header to global
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
void dc_rs_set_seg_header(void *_seg_header)
{
    memset(&g_rs_header, 0, sizeof(g_rs_header));

    memcpy(&g_rs_header, _seg_header, sizeof(g_rs_header));

}


/*
 *  Function:
 *      get segment header from global
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
void *dc_rs_get_seg_header()
{
    return &g_rs_header;
}


/*
 *  Function:
 *      clear all node in list
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
void dc_rs_reset()
{
    dc_rs_t *p = NULL;

    g_rs_curr = NULL;

    if (g_rs_list != NULL)
    {
        p = list_entry(g_rs_list->head->next, DC_TYPEOF(*p), link);
        while (&p->link != g_rs_list->head)
        {
            list_del(&p->link);

            /*
            olog("reset free node[%d => %p]", g_rs_list_num, p);
            */
            if (p->row != NULL)
            {
                /*
                olog("reset free row [%d => %p]", g_rs_list_num, p->row);
                */
                free(p->row);
                p->row = NULL;
            }

            free(p);
            g_rs_list_num--;

            p = list_entry(g_rs_list->head->next, DC_TYPEOF(*p), link);
        }
    }
}


/*
 *  Function:
 *      1. allocate an area;
 *      2. copy data;
 *      3. add to list;
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
int  dc_rs_add_row(void *_row, int _len, int _id)
{
    dc_rs_t *p = NULL;

    p = calloc(sizeof(dc_rs_t), 1);
    if (p == NULL)
    {
        elog("error: calloc1");
        return -1;
    }

    p->id      = _id;
    p->row_len = _len;
    p->row     = calloc(_len+1, 1);
    if (p->row == NULL)
    {
        elog("error: calloc2: %d", _len);
        free(p);
        return -1;
    }
    /*
    olog("row: %p", p->row);
    */

    memcpy(p->row, _row, _len);

    list_add(g_rs_list, &p->link);

    g_rs_list_num++;

    if (g_rs_list_num == 1)
    {
        g_rs_curr = p;
        /*
        olog("to curr: %p", g_rs_curr);
        */
    }

    return 0;
}


void dc_rs_iterator_init()
{
    g_rs_curr = list_entry(g_rs_list->head, DC_TYPEOF(*g_rs_curr), link);
}


/*
 *  Function:
 *      get next row
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - a row
 *      FAILURE - NULL
 */
void *dc_rs_iterator_next()
{
    dc_rs_t *p = NULL;

    if (g_rs_curr == NULL)
    {
        return NULL;
    }

    p = list_entry(g_rs_curr->link.next, DC_TYPEOF(*p), link);

    if (&p->link == g_rs_list->head)
    {
#if DC_DEBUG
        olog("no more: back to head");
#endif
        p = NULL;
    }
    else
    {
#if DC_DEBUG
        olog("id: [%d]", p->id);
#endif
        g_rs_curr = p;
        p = p->row;
    }

    return p;
}


/*
 *  Function:
 *      get first row data
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
void *dc_rs_get_head_row()
{
    dc_rs_t *p = NULL;

    p = list_entry(g_rs_list->head->next, DC_TYPEOF(*p), link);
    if (&p->link == g_rs_list->head)
    {
        olog("no data");
        p = NULL;
    }
    else
    {
        p = p->row;
    }

    return p;
}


/*
 *  Function:
 *      get curr row data
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
void *dc_rs_get_curr_row()
{
    dc_rs_t *p = NULL;

    /*
    olog("curr: %p", g_rs_curr);
    */

    if (g_rs_list_num > 0 && g_rs_curr != NULL)
    {
        if (&g_rs_curr->link == g_rs_list->head)
        {
            olog("curr is head");
            p = list_entry(g_rs_list->head->next, DC_TYPEOF(*p), link);
            return p->row;
        }
        else
        {
            return g_rs_curr->row;
        }
    }

    return NULL;
}


/*
 *  Function:
 *      get first row data and length
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
int dc_rs_get_row2(void **_row, int *_len)
{
    int rv = -1;
    dc_rs_t *p = NULL;

    p = list_entry(g_rs_list->head->next, DC_TYPEOF(*p), link);
    if (&p->link == g_rs_list->head)
    {
        olog("no data");
    }
    else
    {
        *_row = p->row;
        *_len = p->row_len;
        rv = 0;
    }

    return rv;
}


/*
 *  Function:
 *      get the number of result set
 *      used scene: for his.c
 *      used scene2: for count(*)
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
int dc_rs_get_num()
{
    return g_rs_list_num;
}


/*
 *  Function:
 *      set to the result set
 *      used scene: for his.c to restore context
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE -
 */
int dc_rs_restore_context(void *_row, int _row_len, void *_seg_header)
{
    dc_rs_reset();

    if (dc_rs_add_row(_row, _row_len, -1))
    {
        elog("error: dc_rs_add_row");
        return -1;
    }

    dc_rs_set_seg_header(_seg_header);

    return 0;
}


void dc_rs_dump()
{
    int  n = 0;
    dc_rs_t *p = NULL;

    olog("-->>>>%s", __func__);

    olog("row number: %d", g_rs_list_num);

    list_for_each_entry(p, g_rs_list->head, link)
    {
        olog("[%d => %p]", n++, p);

        if (p->row != NULL)
        {
            olog("--- row[%p: %d, %s]", p->row, p->row_len, p->row);
        }
        else
        {
            olog("--- null row data");
        }
    }
}



/**
 * 
 */
int dc_rs_uniq()
{
    int  data_id = 0;
    int  dynamic_mode = 0;
    dc_rs_t *p = NULL;
    dc_rs_t *q = NULL;  /* temporary */
    char *p_cont = NULL;

    if (g_rs_list_num <= 0)
    {
        return 0;
    }

    if (g_rs_list_num <= RS_CONT_SIZE)
    {
        /* use global container */
#if DC_DEBUG
        olog("static mode");
#endif
        memset(g_rs_cont, 0, sizeof(g_rs_cont));
        p_cont = g_rs_cont;
    }
    else
    {
#if DC_DEBUG
        olog("dynamic mode");
#endif
        dynamic_mode = 1;
        p_cont = calloc(g_rs_list_num, sizeof(char));
        if (p_cont == NULL)
        {
            elog("error: calloc: %d", g_rs_list_num);
            return -1;
        }
    }

    if (g_rs_list != NULL)
    {
        p = list_entry(g_rs_list->head->next, DC_TYPEOF(*p), link);
        while (&p->link != g_rs_list->head)
        {
            q = list_entry(p->link.next, DC_TYPEOF(*p), link);

            data_id = p->id;

#if DC_DEBUG
            olog("data-id[%d], [%p]", data_id, p);
#endif
            if (p_cont[data_id])
            {
                /* delete for has appeared */
                list_del(&p->link);
                g_rs_list_num--;
#if DC_DEBUG
                olog("[%d] has appeard!", data_id);
#endif
            }
            else
            {
                /* mark as new */
                p_cont[data_id] = 1;
            }

            p = q;
        } /* while */
    }

    if (dynamic_mode)
    {
        free(p_cont);
    }

    return g_rs_list_num;
}



#if RUN_RS
int main(int argc, char *argv[])
{
    int  i   = 0;
    int  n   = 0;
    int  rv  = 0;
    int  len = 0;
    int  max = 0;
    char buffer[128] = {0};
    char *p = NULL;

    (void)argc;
    (void)argv;

    olog("rs begin");

    if (dc_rs_init())
    {
        elog("error: dc_rs_init");
        return -1;
    }

    olog("=========================================");

    /* call every time */
    dc_rs_reset();

    olog("-----------------------------------------");
    p = dc_rs_get_head_row();
    if (p != NULL)
    {
        olog("row: %s", p);
    }
    else
    {
        olog("sorry, no data: dc_rs_get_head_row");
    }
    olog("-----------------------------------------");

    olog("iterator begin");
    dc_rs_iterator_init();

    while ((p = dc_rs_iterator_next()) != NULL)
    {
        olog("data: [%p: %s]", p, p);
    }
    olog("iterator end");

    olog("-----------------------------------------");

    max = 3;
    for (i = 0; i < max; i++)
    {
        len = snprintf(buffer, sizeof(buffer), "%s--%ld", "abc", get_time());
        if (dc_rs_add_row(buffer, len, i))
        {
            elog("error: dc_rs_add_row: %d, %s", len, buffer);
            return -1;
        }
        else
        {
            olog("add [%s, %d] succeeds", buffer, len);
        }
    }
    olog("-----------------------------------------");

    dc_rs_dump();

    olog("-----------------------------------------");
    dc_rs_iterator_init();

    while ((p = dc_rs_iterator_next()) != NULL)
    {
        olog("data: [%p: %s]", p, p);
    }
    olog("-----------------------------------------");

    /****************************************************/
    /****************************************************/
    /****************************************************/

    /* case2: uniq test */
    olog("+++++++++++++++++++++++++++++++++++++++++");
    olog("+++++++++++++++++++++++++++++++++++++++++");
    olog("+++++++++++++++++++++++++++++++++++++++++");
    dc_rs_reset();

    max = 5;
    for (i = 0; i < max; i++)
    {
        len = snprintf(buffer, sizeof(buffer), "%s--%ld", "abc", get_time());
        if (dc_rs_add_row(buffer, len, i%3))
        {
            elog("error: dc_rs_add_row: %d, %s", len, buffer);
            return -1;
        }
        else
        {
            olog("add [%s, %d] succeeds", buffer, len);
        }
    }
    olog("+++++++++++++++++++++++++++++++++++++++++");
    dc_rs_dump();
    olog("+++++++++++++++++++++++++++++++++++++++++");
    olog("before: %d", g_rs_list_num);
    n = dc_rs_uniq();
    olog("after: %d, %d", g_rs_list_num, n);
    olog("+++++++++++++++++++++++++++++++++++++++++");
    dc_rs_dump();

    dc_rs_destroy();

    olog("rs end");
    return rv;
}
#endif

/* rs.c */
