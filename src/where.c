#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "util.h"
#include "hash.h"
#include "log.h"
#include "list.h"
#include "stack.h"

#include "where.h"



#define DC_WHERE_AND_SEP                "AND"
#define DC_WHERE_AND_SEP_LEN            3

#define DC_WHERE_OR_SEP                 "OR"
#define DC_WHERE_OR_SEP_LEN             2

#define DC_WHERE_STACK_MAX              100
#define DC_WHERE_SEC_MAX                1024


typedef struct {
    int     len;
    char    buf[DC_WHERE_SEC_MAX];
    Link    link;
} dc_where_node_t;


typedef struct _dc_where_list_t {
    List   *list;
    int     num;    /* number */
} dc_where_list_t;



/* ================================================================= */

void *dc_where_create_list();

int  dc_where_destroy_list(dc_where_list_t *_wl);

int  dc_where_add_list(dc_where_list_t *_wl, char *_str, int _len);
/* int  dc_where_add_list2(dc_where_list_t *_wl, char *_str, int _len); */

void dc_where_dump_list(dc_where_list_t *_wl);

int  dc_where_clear_outer_parentheses(char *_sql, int _len, int *_start, int *_len2);

void *dc_where_get_OPR_list(char *_sql, int _len, char *_opr, int _opr_len);

void *dc_where_get_OR_list(char *_sql, int _len);
void *dc_where_get_AND_list(char *_sql, int _len);
int  dc_where_list_multiply(dc_where_list_t *_mul_list, dc_where_list_t *_new_list);

static int dc_where_get_leftest_p(char *_sql, int _len, int *_pos);
static int dc_where_get_rightest_p(char *_sql, int _len, int *_pos);

/* ================================================================= 
   ================================================================= */


/*
 *  Function:
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_where_init(void)
{

    return 0;
}


int dc_where_destroy()
{

    return 0;
}



/*
 *  Function:
 *       b='67' AND (f='10' OR f='12')
 *       f='10' OR f='12'
 *       (b='67' or b='66' or b='65') AND (f='10' OR f='12')
 *       
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - list
 *      FAILURE - NULL
 */
void *dc_where_analyze_recur(char *_sql, int _len)
{
    int   rv = 0;
    static int level = 0;

    dc_where_list_t *list0 = NULL;  /* final result list */

    dc_where_node_t *node1 = NULL;  /* OR */
    dc_where_list_t *list1 = NULL;  /* OR */

    dc_where_node_t *node2 = NULL;  /* AND */
    dc_where_list_t *list2 = NULL;  /* AND */

    dc_where_list_t *list3 = NULL;  /* RECURSIVE */

    dc_where_list_t *mul_list = NULL;   /* AND: multiply */
    dc_where_node_t *nodeM = NULL;

    /* create result list  */
    list0 = dc_where_create_list();
    if (list0 == NULL)
    {
        elog("error: dc_where_create_list");
        return NULL;
    }

    list1 = (dc_where_list_t *)dc_where_get_OR_list(_sql, _len);
    if (list1 == NULL)
    {
        elog("error: dc_where_get_OR_list");
        dc_where_destroy_list(list0);
        list0 = NULL;
        return NULL;
    }

    level++;

#if DC_DEBUG
    olog("[%d]@_@[%.*s]", level, _len, _sql);
#endif

#if DC_DEBUG
    olog("[%d]+++++++++++++++++++++++++++++++++++++++++", level);
    olog("[%d]get OR list: %d", level, list1->num);
    dc_where_dump_list(list1);
    olog("[%d]+++++++++++++++++++++++++++++++++++++++++", level);
#endif

    /* iterate OR list */
    list_for_each_entry(node1, list1->list->head, link)
    {
#if DC_DEBUG
        olog("[%d]#########################################", level);
        olog("[%d]node#OR: (%d)[%.*s]", level, node1->len, node1->len, node1->buf);
#endif

        list2 = (dc_where_list_t *)dc_where_get_AND_list(node1->buf, node1->len);
        if (list2 == NULL)
        {
            elog("error: dc_where_get_AND_list");
            rv = -1;
            goto _end;
        }

#if DC_DEBUG
        olog("[%d]-----------------------------------------", level);
        olog("[%d]-->get AND list with [%d] nodes", level, list2->num);
        dc_where_dump_list(list2);
        olog("[%d]-----------------------------------------", level);
#endif

        if (list2->num == 1)
        {
#if DC_DEBUG
            olog("[%d]REACH end: [%.*s], add to list0", level, node1->len, node1->buf);
#endif
            if (dc_where_add_list(list0, node1->buf, node1->len))
            {
                elog("error: dc_where_add_list");
                rv = -1;
                goto _end;
            }
        }
        else
        {
#if DC_DEBUG
            olog("[%d]AND: more than 1", level);
#endif

            /* iterate AND list */
            mul_list = dc_where_create_list();
            if (mul_list == NULL)
            {
                elog("error: dc_where_create_list: mul");
                rv = -1;
                goto _end;
            }

            list_for_each_entry(node2, list2->list->head, link)
            {
#if DC_DEBUG
                olog("[%d]node#AND: (%d)[%.*s]", level, node2->len, node2->len, node2->buf);
#endif
                /* recursive */
                list3 = dc_where_analyze_recur(node2->buf, node2->len);
                if (list3 == NULL)
                {
                    elog("error: dc_where_analyze_recur -- recur");
                    rv = -1;
                    goto _end;
                }

#if DC_DEBUG
                olog("[%d]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", level);
                olog("[%d]get RET list: %d", level, list3->num);
                /*
                dc_where_dump_list(list3);
                */
                olog("[%d]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", level);
#endif

                /* do multiply: (mul_list * list3 => mul_list) */
                if (dc_where_list_multiply(mul_list, list3))
                {
                    elog("error: dc_where_list_multiply");
                    rv = -1;
                    goto _end;
                }

                /*
                olog("fre list3");
                */
                dc_where_destroy_list(list3);
                list3 = NULL;
            }

            /* append list3 => list0 */
            list_for_each_entry(nodeM, mul_list->list->head, link)
            {
                if (dc_where_add_list(list0, nodeM->buf, nodeM->len))
                {
                    elog("error: dc_where_add_list: M");
                    rv = -1;
                    goto _end;
                }
            }

            /*
            olog("fre list mul");
            */
            dc_where_destroy_list(mul_list);
            mul_list = NULL;
        }

        /* free list2 */
        dc_where_destroy_list(list2);
        list2 = NULL;
    }

    /*
    olog("2+++++++++++++++++++++++++++++++++++++++++");
    olog("2get OR list: %d", list1->num);
    dc_where_dump_list(list1);
    olog("2+++++++++++++++++++++++++++++++++++++++++");
    */

    dc_where_destroy_list(list1);
    list1 = NULL;

_end:
    level--;

    /* OR list */
    if (list1)
    {
        dc_where_destroy_list(list1);
        list1 = NULL;
    }

    /* AND list */
    if (list2)
    {
        dc_where_destroy_list(list2);
        list2 = NULL;
    }

    /* RECUR list */
    if (list3)
    {
        dc_where_destroy_list(list3);
        list3 = NULL;
    }

    /* MUL list */
    if (mul_list)
    {
        dc_where_destroy_list(mul_list);
        mul_list = NULL;
    }

    if (rv)
    {
        elog("error: '%s' failure", __func__);
        dc_where_destroy_list(list0);
        list0 = NULL;
    }

    return list0;
}


/*
 *  Function:
 *       b='67' AND (f='10' OR f='12')
 *       f='10' OR f='12'
 *       (b='67' or b='66' or b='65') AND (f='10' OR f='12')
 *       
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - list
 *      FAILURE - NULL
 */
void *dc_where_analyze(char *_sql, int _len)
{
    int   rv = 0;
    int   len = 0;
    int   offset = 0;
    int   where_len = 0;
    char *where_region = NULL;
    void *list = NULL;

    offset = 0;
    len    = 0;
    rv = dc_where_clear_outer_parentheses(_sql, _len, &offset, &len);
    if (rv < 0)
    {
        elog("error: dc_where_clear_outer_parentheses: \n%s", _sql);
        return NULL;
    }
    else if (rv > 0)
    {
        where_region = _sql + offset;
        where_len    = len;
#if DC_DEBUG
        olog("shrinked: %d: %.*s", where_len, where_len, where_region);
#endif
    }
    else
    {
        where_region = _sql;
        where_len    = _len;
#if DC_DEBUG
        olog("keeping:  %d: %.*s", where_len, where_len, where_region);
#endif
    }

    list = dc_where_analyze_recur(where_region, where_len);

    return list;
}


/*
 *  Function:
 *       _mul_list = _mul_list * _new_list
 *       
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - multipled-list
 *      FAILURE - NULL
 */
int dc_where_list_multiply(dc_where_list_t *_mul_list, dc_where_list_t *_new_list)
{
    int  i  = 0;
    int  num = 0;
    int  len = 0;
    char buf[DC_WHERE_SEC_MAX] = {0};
    dc_where_node_t *node1 = NULL;
    dc_where_node_t *node2 = NULL;
    List *list = NULL;

    if (_mul_list == NULL || _new_list == NULL)
    {
        elog("error: invalid input");
        return -1;
    }

    num  = _mul_list->num;
    list = _mul_list->list;

    if (num > 0)
    {
        /*
        olog("mul-curr-num: %d", num);
        */

        for (i = 0; i < num; i++)
        {
            /* get the first node */
            node1 = list_entry(list->head->next, DC_TYPEOF(*node1), link);
            if (&node1->link == list->head)
            {
                olog("error: no more node");
                return -1;
            }

            /* 1. extract from list */
            list_del(&node1->link);
            _mul_list->num--;

            list_for_each_entry(node2, _new_list->list->head, link)
            {
                /*
                olog("node2: [%.*s]", node2->len, node2->buf);
                */

                /*
                olog("DI-0: [%.*s AND %.*s]", node1->len, node1->buf, node2->len, node2->buf);
                */
                len = node1->len + node2->len + DC_WHERE_AND_SEP_LEN + 3;
                if (len > DC_WHERE_SEC_MAX)
                {
                    elog("error: too long: %d", len);
                    return -1;
                }

                memset(buf, 0, sizeof(buf));
                len = snprintf(buf, sizeof(buf), "%.*s %s %.*s",
                        node1->len, node1->buf,
                        DC_WHERE_AND_SEP,
                        node2->len, node2->buf);
#if DC_DEBUG
                olog("DI-1: [%s](%d)", buf, len);
#endif

                if (dc_where_add_list(_mul_list, buf, len))
                {
                    elog("error: dc_where_add_list");
                    return -1;
                }
            } /* iterator */

            free(node1);
        } /* for */
    }
    else
    {
        /*
        olog("first time, copy only");
        */
        list_for_each_entry(node2, _new_list->list->head, link)
        {
#if DC_DEBUG
            olog("node2-Z: [%.*s]", node2->len, node2->buf);
#endif

            if (dc_where_add_list(_mul_list, node2->buf, node2->len))
            {
                elog("error: dc_where_add_list");
                return -1;
            }
        }
    }

    return 0;
}


/*
 *  Function:
 *        b='67'
 *        f='10' OR (f='12')
 *        (f='10') OR f='12'
 *       ( (f='10') OR f='12' )
 *       (b='67' AND (f='10' OR f='12')) 
 *       (b='67' AND (f='10' OR f='12') 
 *       (b='67' AND (f='10' OR f='12')))
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - 0: good with ()
 *              - 1: good without ()
 *      FAILURE - -1
 */
int dc_where_pre_check_parentheses(char *_sql, int _len)
{
    char  c = 0;
    int   k = 0;
    int   rv = 0;
    int   has_p = 0;
    int   last_start = 0;
    char *p0 = NULL;
    char *p1 = NULL;
    dc_stack_t *s = NULL;

    /*
    olog("[%.*s]", _len, _sql);
    */

    s = dc_stack_create(DC_WHERE_STACK_MAX);
    if (s == NULL)
    {
        elog("error: dc_stack_create");
        return -1;
    }

    last_start = 0;

    for (k = 0; k < _len; k++)
    {
        c  = _sql[k];
        p0 = _sql + k;

        if (c == '(')
        {
            /* push stack */
            if (dc_stack_push(s, p0))
            {
                elog("error: dc_stack_push");
                rv = -1;
                goto _end;
            }
            /*
            olog("push [%d] to stack", k);
            */

            has_p = 1;

        }
        else if (c == ')')
        {
            /* pop stack */
            if ((p1 = dc_stack_pop(s)) == NULL)
            {
                /*
                elog("error: dc_stack_pop");
                */
                rv = -1;
                goto _end;
            }
            /*
            olog("pop [%ld] from stack", (long)(p1-_sql));
            */
            has_p = 1;
        }
        else if (c == ' ')
        {
            /* space */
        }
        else
        {
            /* word */
        }
    }

    if (!dc_stack_is_empty(s))
    {
        rv = -1;
        elog("less ')'");
    }

_end:

    if (s)
    {
        dc_stack_destroy(s);
    }

    if (rv == 0)
    {
        if (has_p)
        {
            rv = 1;
        }
    }


    return rv;
}



/*
 *  Function:
 *        b='67'
 *        b='67' or b = '66'
 *        b='67' or b = '66' or b ='77'
 *        (f='10') OR (f='12')
 *        (f='10' AND e='20') OR a ='1'
 *        (f='10' OR e='20')  OR a ='1'
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - list
 *      FAILURE - NULL
 */
void *dc_where_get_OR_list(char *_sql, int _len)
{
    return dc_where_get_OPR_list(_sql, _len, DC_WHERE_OR_SEP, DC_WHERE_OR_SEP_LEN);
}


/*
 *  Function:
 *        e = '20'
 *        e = '20' and  f='10'
 *        e = '20' and (f='10' OR f='20')
 *        e = '40' and (f='10' OR f='20') and d = '30'
 *        e = '20' and (f='10')
 *        1. check whether has '()'
 *        2. check whether sub has 'OR'
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - list
 *      FAILURE - NULL
 */
void *dc_where_get_AND_list(char *_sql, int _len)
{
    return dc_where_get_OPR_list(_sql, _len, DC_WHERE_AND_SEP, DC_WHERE_AND_SEP_LEN);
}


/*
 *  Function:
 *        for inner use
 *        e = '20'
 *        e = '20' OPR  f='10'
 *        e = '20' OPR (f='10' X f='20')
 *        e = '40' OPR (f='10' X f='20') OPR d = '30'
 *        e = '20' OPR (f='10')
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - list
 *      FAILURE - NULL
 */
void *dc_where_get_OPR_list(char *_sql, int _len, char *_opr, int _opr_len)
{
    char c = 0;
    int  k = 0;
    int  rv = 0;
    int  len = 0;
    int  new_len = 0;
    int  str_len = 0;
    int  rel_start = 0;
    int  last_start = 0;
    int  token_started = 0;
    int  token_start_pos = 0;
    char *p0 = NULL;
    char *p1 = NULL;
    char *pt = NULL;
    char *str = NULL;
    dc_stack_t *s = NULL;
    dc_where_list_t *wl = NULL;

    /*
    olog("[%.*s]", _len, _sql);
    */

    wl = dc_where_create_list();
    if (wl == NULL)
    {
        elog("error: dc_where_create_list");
        return NULL;
    }

    s = dc_stack_create(DC_WHERE_STACK_MAX);
    if (s == NULL)
    {
        elog("error: dc_stack_create");
        dc_where_destroy_list(wl);
        return NULL;
    }

    last_start = 0;

    for (k = 0; k < _len; k++)
    {
        c  = _sql[k];
        p0 = _sql + k;

        /* printf("%c", c); */

        if (c == '(')
        {
            /* push stack */
            if (dc_stack_push(s, p0))
            {
                elog("error: dc_stack_push");
                rv = -1;
                goto _end;
            }
            /*
            olog("push [%d] to stack", k);
            */
        }
        else if (c == ')')
        {
            /* pop stack */
            if ((p1 = dc_stack_pop(s)) == NULL)
            {
                elog("error: dc_stack_pop");
                rv = -1;
                goto _end;
            }
            /*
            olog("pop [%ld] from stack", (long)(p1-_sql));
            */
        }
        else if (c == ' ')
        {
            if (token_started)
            {
                token_started = 0;
                len = k - token_start_pos;

                if (dc_stack_is_empty(s))
                {
                    pt = _sql+token_start_pos;
                    /*
                    olog("++token: (%d)[%.*s]", len, len, _sql+token_start_pos);
                    */
                    if (len == _opr_len
                            && strncasecmp(pt, _opr, len) == 0)
                    {
                        /*
                        olog("got [%.*s]", len, pt);
                        */
                        len = k - last_start - _opr_len;
                        /*
                        olog("{EXPR-%s}[%.*s](%d)", _opr, len, _sql+last_start, len);
                        */

                        rel_start = 0;
                        new_len = len;
                        rv = dc_where_clear_outer_parentheses(_sql+last_start, len, &rel_start, &new_len);
                        if (rv < 0)
                        {
                            elog("error: dc_where_clear_outer_parentheses: %d, %.*s", len, len, _sql+last_start);
                            rv = -1;
                            goto _end;
                        }
                        else if (rv > 0)
                        {
                            rv = 0;
                            /*
                            olog("nice: redundant () is cleared");
                            */
                            str = _sql+last_start + rel_start;
                            str_len = new_len;
                            /*
                            olog("===refined: (%d)[%.*s]", str_len, str_len, str);
                            */
                        }
                        else
                        {
                            str = _sql+last_start;
                            str_len = len;
                            /*
                            olog("===[%.*s]: not exists outer ()", str_len, str);
                            */
                        }

                        /* put to list */
                        dc_where_add_list(wl, str, str_len);

                        last_start = k;
                    }
                }
                else
                {
                    /*
                    olog("--token: (%d)[%.*s]", len, len, _sql+token_start_pos);
                    */
                }
            }
        }
        else
        {
            /* token start */
            if (!token_started)
            {
                token_started = 1;
                token_start_pos = k;
            }
        }
    }

    /* the last area */
    len = _len - last_start;

    rel_start = 0;
    new_len = len;
    rv = dc_where_clear_outer_parentheses(_sql+last_start, len, &rel_start, &new_len);
    if (rv < 0)
    {
        elog("error: dc_where_clear_outer_parentheses: %d, %.*s", len, len, _sql+last_start);
        rv = -1;
        goto _end;
    }
    else if (rv > 0)
    {
        rv = 0;
        /*
        olog("nice: redundant () is cleared");
        */
        str = _sql+last_start + rel_start;
        str_len = new_len;
        /*
        olog("refined: (%d)[%.*s]", str_len, str_len, str);
        */
    }
    else
    {
        str = _sql+last_start;
        str_len = len;
        /*
        olog("[%.*s]: not exists outer ()", str_len, str);
        */
    }
    /*
    olog("{EXPR-END}[%.*s](%d)", str_len, str, str_len);
    */

    /* put to list */
    dc_where_add_list(wl, str, str_len);

_end:
    if (s)
    {
        dc_stack_destroy(s);
    }

    if (rv)
    {
        elog("error: get '%s' list failure", _opr);
        dc_where_destroy_list(wl);
        wl = NULL;
    }

    return wl;
}


void *dc_where_create_list()
{
    dc_where_list_t *wl = calloc(1, sizeof(dc_where_list_t));
    if (wl == NULL)
    {
        elog("error: calloc");
        return NULL;
    }

    wl->list = create_list();
    if (wl->list == NULL)
    {
        free(wl);
        return NULL;
    }

    wl->num  = 0;

    return wl;
}


int dc_where_add_list(dc_where_list_t *_wl, char *_str, int _len)
{
    dc_where_node_t *node = NULL;

    node = calloc(1, sizeof(dc_where_node_t));
    if (node == NULL)
    {
        elog("error: calloc");
        return -1;
    }

    if (_len >= sizeof(node->buf))
    {
        elog("error: too long: %d", _len);
        free(node);
        return -1;
    }

    memcpy(node->buf, _str, _len);
    node->buf[_len] = 0;
    node->len = _len;

    list_add(_wl->list, &node->link);
    _wl->num++;

    return 0;
}


int dc_where_destroy_list(dc_where_list_t *_wl)
{
    int  i  = 0;
    int  rv = 0;
    int  num = 0;
    dc_where_node_t *node = NULL;
    List *list = NULL;

    if (_wl != NULL)
    {
        num  = _wl->num;
        list = _wl->list;
        /*
        olog("des num: %d", num);
        */

        for (i = 0; i < num; i++)
        {
            /* get the first node */
            node = list_entry(list->head->next, DC_TYPEOF(*node), link);
            if (&node->link == list->head)
            {
                olog("error: no more node");
                return -1;
            }

            /*
            olog("destroy loop del: [%p][%.*s]", node, node->len, node->buf);
            */

            /* extract from list */
            list_del(&node->link);
            /*
            olog("destroy after list_del");
            */

            free(node);
            node = NULL;
            /*
            olog("destroy after free");
            */
        }

        destroy_list(list);
        _wl->list = NULL;

        free(_wl);
    }

    return rv;
}


/*
 *  Function:
 *        e = '20'
 *        (e = '20')    => e = '20' 
 *        (e = '20' and (f='10' OR f='20')) => e = '20' and (f='10' OR f='20')
 *        (e = '20' and (f='10' OR f='20') and d =3) => e = '20' and (f='10' OR f='20') and d =3
 *        ((e = '40' )) => e = '40' 
 *        (e = '40') or (e='50') =>  orig
 *        ((e = '40') or (e='50')) => (e = '40') or (e='50')
 *
 *
 *  Parameters:
 *      _start - MUST be set to 0
 *      _len2  - MUST be set to _len
 *
 *
 *  Return Value:
 *      SUCCESS - 0: no redundant ()
 *              - 1: exist
 *      FAILURE - failure
 */
int dc_where_clear_outer_parentheses(char *_sql, int _len, int *_start, int *_len2)
{
    int   a = 0;
    int   b = 0;
    int   rv = 0;
    int   len = 0;
    int   len2 = 0;
    int   start = 0;
    char *str = NULL;

    /*
    olog("+++[%.*s]", _len, _sql);
    */

    /* leftest ( */
    rv = dc_where_get_leftest_p(_sql, _len, &a);
    if (rv < 0)
    {
        elog("error: dc_where_get_leftest_p");
        return -1;
    }
    else if (rv == 0) 
    {
#if DC_DEBUG
        olog("no need: 1");
#endif
        return 0;
    }
    else
    {
        /*
        olog("left '(': %d", a);
        */
    }

    /*  rightest ) */
    rv = dc_where_get_rightest_p(_sql, _len, &b);
    if (rv < 0)
    {
        elog("error: dc_where_get_rightest_p");
        return -1;
    }
    else if (rv == 0) 
    {
#if DC_DEBUG
        olog("no need: 2");
#endif
        return 0;
    }
    else
    {
        /*
        olog("right '(': %d", b);
        */
    }

    str = _sql + a + 1;
    len = b - a - 1;
    /*
    olog("next-to-check:[%.*s]", len, str);
    */

    rv = dc_where_pre_check_parentheses(str, len);
    if (rv < 0)
    {
#if DC_DEBUG
        olog("no need: 3");
#endif
        return 0;
    }
    else if (rv == 0)
    {
#if DC_DEBUG
        olog("[%.*s]: shrinked2", len, str);
#endif
        *_start += a+1;
        *_len2  = len;
    }
    else
    {
#if DC_DEBUG
        olog("[%.*s]: shrinked1", len, str);
#endif

        start = a+1;
        len2  = len;

        /*
        olog("recursive");
        */
        rv = dc_where_clear_outer_parentheses(str, len, &start, &len2);
        if (rv < 0)
        {
            elog("error: dc_where_clear_outer_parentheses");
            return -1;
        }
        else if (rv > 0)
        {
            *_start += start;
            *_len2  = len2;
#if DC_DEBUG
            olog("[%.*s]: again", len, str);
#endif
        }
        else
        {
#if DC_DEBUG
            olog("[%.*s]: xxx", len, str);
#endif
            *_start += start;
            *_len2  = len2;
        }
    }

    if (*_len2 != _len)
    {
        rv = 1;
    }

    return rv;
}


/*
 *  Function:
 *        e = '20'
 *        (e = '20')
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - 0: not found
 *              - 1: exist
 *      FAILURE - failure
 */
static int dc_where_get_leftest_p(char *_sql, int _len, int *_pos)
{
    char c = 0;
    int  k = 0;
    int  rv = 0;

    /* get first '(' from left to right */
    for (k = 0; k < _len; k++)
    {
        c  = _sql[k];

        if (c == '(')
        {
            /*
            olog("find leftest[%c]", c);
            */
            rv = 1;
            *_pos = k;
            break;
        }
        else if (c == ')')
        {
            elog("error: leftest: ')'");
            return -1;
        }
        else if (c == ' ')
        {
            /* space */
        }
        else
        {
            /* word */
            rv = 0;
            break;
        }
    }

    return rv;
}


/*
 *  Function:
 *        e = '20'
 *        (e = '20')
 *
 *  Parameters:
 *
 *
 *  Return Value:
 *      SUCCESS - 0: not found
 *              - 1: exist
 *      FAILURE - failure
 */
static int dc_where_get_rightest_p(char *_sql, int _len, int *_pos)
{
    char c = 0;
    int  k = 0;
    int  rv = 0;

    /* get last ')', from rigth to left */
    for (k = _len-1; k >= 0; k--)
    {
        c  = _sql[k];

        if (c == '(')
        {
            elog("error: rightest: '('");
            return -1;
        }
        else if (c == ')')
        {
            /*
            olog("got [%c]", c);
            */
            rv = 1;
            *_pos = k;
            break;
        }
        else if (c == ' ')
        {
            /* space */
        }
        else
        {
            /* word */
            rv = 0;
            break;
        }
    }

    return rv;
}


static dc_where_node_t *g_where_curr = NULL;
static dc_where_list_t *g_where_list = NULL;

int dc_where_iterator_init(void *_addr)
{
    int   num  = 0;
    List *list = NULL;

    g_where_list = (dc_where_list_t *)_addr;
    list = g_where_list->list;
    num  = g_where_list->num;

    g_where_curr = list_entry(list->head, DC_TYPEOF(*g_where_curr), link);

    return num;
}


/*
 *  Function:
 *      get next, from start
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - a row
 *      FAILURE - NULL
 */
void *dc_where_iterator_next()
{
    void *p = NULL;
    dc_where_node_t *node = NULL;
    List *list = NULL;

    if (g_where_list == NULL)
    {
        elog("error: iterator not init");
        return NULL;
    }

    list = g_where_list->list;

    node = list_entry(g_where_curr->link.next, DC_TYPEOF(*node), link);

    if (&node->link == list->head)
    {
        olog("no more: back to head");
        node = NULL;
    }
    else
    {
        g_where_curr = node;
        p = node->buf;
    }

    return p;
}


int dc_where_iterator_end(void *_list)
{
    int rv = 0;
    
    if (_list != NULL)
    {
        rv = dc_where_destroy_list(_list);
    }

    return rv;
}



void dc_where_dump_list(dc_where_list_t *_wl)
{
    int i = 0;
    dc_where_node_t *node = NULL;

    list_for_each_entry(node, _wl->list->head, link)
    {
        olog("[%03d]: [%.*s](%d)", i, node->len, node->buf, node->len);
        i++;
    }

    return;
}


#if RUN_WHERE
int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  len = 0;
    char where[1024] = {0};
    dc_where_list_t *list = NULL;

    (void)argc;
    (void)argv;
    (void)len;
    (void)list;
    (void)where;

    olog("where begin");

    if (dc_where_init())
    {
        elog("error: dc_where_init");
        return -1;
    }

#if 0
    olog("=========================================");
    len = snprintf(where, sizeof(where), "%s", "(b='67' AND (f='10' OR f='12')))"); /* 7 */
    len = snprintf(where, sizeof(where), "%s", "(b='67' AND (f='10' OR f='12')");   /* 6 */
    len = snprintf(where, sizeof(where), "%s", "b='67'");       /* 1 */
    len = snprintf(where, sizeof(where), "%s", " f='10' OR (f='12')");  /* 2 */
    len = snprintf(where, sizeof(where), "%s", " (f='10') OR f='12' "); /* 3 */
    len = snprintf(where, sizeof(where), "%s", "( (f='10') OR f='12' )");   /* 4 */
    len = snprintf(where, sizeof(where), "%s", "(b='67' AND (f='10' OR f='12'))");  /* 5 */
    rv = dc_where_pre_check_parentheses(where, len);
    if (rv < 0)
    {
        elog("error: dc_where_pre_check_parentheses");
        elog("==> invalid SQL: [%s]", where);
        return -1;
    }
    else
    {
        olog("good SQL!");
    }
#endif

#if 0
    olog("=========================================");
    len = snprintf(where, sizeof(where), "%s", " (f='10' OR e='20')  OR a ='1' "); /* 6 */
    len = snprintf(where, sizeof(where), "%s", " (f='10') OR (f='12') and e = 9   "); /* 7 */
    len = snprintf(where, sizeof(where), "%s", " (f='10') AND (f='12') and e = 9 "); /* 8 */
    len = snprintf(where, sizeof(where), "%s", " (f='10') OR (f='12') or e = 9 "); /* 8 */
    len = snprintf(where, sizeof(where), "%s", "b='67'"); /* 1 */
    len = snprintf(where, sizeof(where), "%s", "b='67' or b = '66'"); /* 2 */
    len = snprintf(where, sizeof(where), "%s", "b='67' or b = '66' or b ='77'"); /* 3 */
    len = snprintf(where, sizeof(where), "%s", " (f='10') OR (f='12')"  ); /* 4 */
    len = snprintf(where, sizeof(where), "%s", " (f='10' AND e='20') OR a ='1'"); /* 5 */
    list = (dc_where_list_t *)dc_where_get_OR_list(where, len);
    if (list == NULL)
    {
        elog("error: dc_where_get_OR_list");
        return -1;
    }
    else
    {
        olog("+++++++++++++++++++++++++++++++++++++++++");
        olog("get OR list: %d", list->num);
        dc_where_dump_list(list);
    }

#endif

#if 0
    olog("=========================================");
    len = snprintf(where, sizeof(where), "%s", "e = '20'"); /* 1 */
    len = snprintf(where, sizeof(where), "%s", "e = '20' and  f='10'");     /* 2 */
    len = snprintf(where, sizeof(where), "%s", "e = '20' and (f='10' OR f='20')"); /* 3 */
    len = snprintf(where, sizeof(where), "%s", " e ='40' and (  f='10' OR f='20') and d= '30' "); /* 4 */
    len = snprintf(where, sizeof(where), "%s", "e = '20' and (f='10')");    /* 5 */
    list = (dc_where_list_t *)dc_where_get_AND_list(where, len);
    if (list == NULL)
    {
        elog("error: dc_where_get_AND_list");
        return -1;
    }
    else
    {
        olog("+++++++++++++++++++++++++++++++++++++++++");
        olog("AND list: %d", list->num);
        dc_where_dump_list(list);
    }
#endif

#if 0
    int  len2 = 0;
    int  start = 0;

    olog("=========================================");
    /**************************************************************/
    len = snprintf(where, sizeof(where), "%s", "e = '20'");     /* 1 */
    len = snprintf(where, sizeof(where), "%s", "(e = '20')");   /* 2 */
    len = snprintf(where, sizeof(where), "%s", "(e = '20' and (f='10' OR f='20'))"); /* 3 */
    len = snprintf(where, sizeof(where), "%s", " (e = '20' and (f='10' OR f='20') and d =3)");   /* 4 */
    len = snprintf(where, sizeof(where), "%s", "((e = '40' ))");    /* 5 */
    len = snprintf(where, sizeof(where), "%s", " (f='10' OR f='20') ");    /* 6 */
    len = snprintf(where, sizeof(where), "%s", " (f='12') and e = 9 ");     /* 7 */
    len = snprintf(where, sizeof(where), "%s", " (f='10') OR (f='20') ");    /* 8 */
    len = snprintf(where, sizeof(where), "%s", "( (((e = '40' ) )))");    /* 9 */

    olog("input: ---%.*s---", len, where);

    start = 0;
    len2 = len;
    rv = dc_where_clear_outer_parentheses(where, len, &start, &len2);
    if (rv < 0)
    {
        elog("error: dc_where_clear_outer_parentheses");
        return -1;
    }
    else if (rv > 0)
    {
        olog("output:---%.*s---", len2, where+start);
    }
    else
    {
        olog("keeps: [%.*s]", len, where);
    }
#endif

#if 0
    dc_where_list_t *ml = NULL;
    dc_where_list_t *new = NULL;

    olog("=========================================");
    ml = dc_where_create_list();
    if (ml == NULL)
    {
        elog("error: dc_where_create_list: mul");
        return -1;
    }
    /*
    dc_where_add_list(ml, "AD", 2);
    dc_where_add_list(ml, "BCE", 3);
    dc_where_add_list(ml, "HJK", 3);
    */

    new = dc_where_create_list();
    if (new == NULL)
    {
        elog("error: dc_where_create_list: new");
        return -1;
    }
    dc_where_add_list(new, "xx",  2);
    dc_where_add_list(new, "yyy", 3);

    rv = dc_where_list_multiply(ml, new);
    if (rv)
    {
        elog("error: dc_where_list_multiply");
        return -1;
    }
    else
    {
        olog("+++++++++++++++++++++++++++++++++++++++++");
        olog("get RS list: %d", ml->num);
        dc_where_dump_list(ml);
    }

#endif


#if 1
    int  i   = 0;
    int  num = 0;
    char *p = NULL;

    olog("=========================================");



    len = snprintf(where, sizeof(where), "%s", " f='10' OR f='12' ");   /* 1 */
    len = snprintf(where, sizeof(where), "%s", " b='67' AND (f='10' OR f='12') ");  /* 2 */
    len = snprintf(where, sizeof(where), "%s", " b='67' OR (f='10' AND f='12') ");  /* 3 */
    len = snprintf(where, sizeof(where), "%s", " a = 20 or b='67' AND (f='10' OR f='12') AND c='1' ");  /* 5 */

    len = snprintf(where, sizeof(where), "%s", " ((sys_no='544' AND drawee_operate_type='AAAAAA' AND drawee_product_id='AAAAAA') OR (drawee_operate_type='123456789' AND drawee_product_id='test') OR (drawee_operate_type='123456789' AND drawee_product_id='AAAAAA') OR (drawee_operate_type='AAAAAA' AND drawee_product_id='test')) ");  /* X */

    len = snprintf(where, sizeof(where), "%s", " ((sys_no='544' AND drawee_operate_type='AAAAAA' AND drawee_product_id='AAAAAA') OR (drawee_operate_type='123456789' AND drawee_product_id='test') OR (drawee_operate_type='123456789' AND drawee_product_id='AAAAAA') OR (drawee_operate_type='AAAAAA' AND drawee_product_id='test')) and (type ='01' or type='02') ");  /* Y */

    len = snprintf(where, sizeof(where), "%s", " b='67' OR (f='10' AND f='12') ");  /* 6 */
    len = snprintf(where, sizeof(where), "%s", "(b='67' or b='66' or b='65') AND (f='10' OR f='12')");  /* 7 */

    len = snprintf(where, sizeof(where), "%s", "(b='67' or b='66' or b='65') AND (f='10' OR f='12')"
            " and (c= '22' or c='33' )");   /* 8 */
    len = snprintf(where, sizeof(where), "%s", " b='67' AND (f='10' OR f='12' and c = '22') ");  /* 9 */

    len = snprintf(where, sizeof(where), "%s", " (sys_no='544' AND drawee_operate_type='AAAAAA' AND drawee_product_id='AAAAAA') OR (drawee_operate_type='123456789' AND drawee_product_id='test') OR (drawee_operate_type='123456789' AND drawee_product_id='AAAAAA') OR (drawee_operate_type='AAAAAA' AND drawee_product_id='test') "); /* Z */

    olog("%s", where);
    list = (dc_where_list_t *)dc_where_analyze(where, len);
    if (list == NULL)
    {
        elog("error: dc_where_analyze");
        return -1;
    }
    else
    {
        /*
        olog("+++++++++++++++++++++++++++++++++++++++++");
        olog("get RS list: %d", list->num);
        dc_where_dump_list(list);
        */

        num = dc_where_iterator_init(list);
        for (i = 0; i < num; i++)
        {
            p = dc_where_iterator_next();
            if (p == NULL)
            {
                elog("error: dc_where_iterator_next");
                return -1;
            }
            olog("sub[%d]: [%s]", i, p);
        }

        if (dc_where_iterator_end(list))
        {
            elog("error: dc_where_iterator_end");
            return -1;
        }
    }


#endif

    if (dc_where_destroy(list))
    {
        elog("error: dc_where_destroy");
        return -1;
    }


    olog("where end");
    return rv;
}
#endif

/* where.c */
