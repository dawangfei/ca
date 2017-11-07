#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"


List *create_list()
{
    List *list = calloc(1, sizeof(List));
    if (list == NULL)
    {
        return NULL;
    }

    list->head = calloc(1, sizeof(Link));
    if (list->head == NULL)
    {
        free(list);
        return NULL;
    }


    INIT_LIST_HEAD(list->head);

    return list;
}


void destroy_list(List *_list)
{
    if (_list != NULL)
    {
        if (_list->head != NULL)
        {
            free(_list->head);
        }
        free(_list);
    }

    return ;
}


/*
 *  Insert a new entry between two known consecutive entries. 
 * 
 *  This is only for internal list manipulation where we know
 *  the prev/next entries already!
 **/
static void __list_add(Link *new,
        Link *prev,
        Link *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * Insert a new entry in the front.
 * This is good for implementing stacks.
 *
 * list_add_head - add a new entry
 * @_list: list to add
 * @_new: new entry to be added
 *
 **/
void list_add_head(List *_list, Link *_new)
{
    __list_add(_new, _list->head, _list->head->next);
}


/**
 * Insert a new entry in the end.
 * This is useful for implementing queues.
 *
 * list_add_tail - add a new entry
 * @_list: list to add
 * @_new: new entry to be added
 *
 */
void list_add_tail(List *_list, Link *_new)
{
    __list_add(_new, _list->head->prev, _list->head);
}


/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static void __list_del(Link *prev, Link *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
void list_del(Link *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (void *) 0;
    entry->prev = (void *) 0;
}

/**
 * list_empty - tests whether a list is empty
 * @_list: the list to check.
 */
int list_empty(List *_list)
{
    return _list->head->next == _list->head;
}


/**
 * list_for_each        -       iterate over a list
 * @list:       the list.
 */
void iterate_list(List *list)
{
    Link *head = NULL;
    Link *link = NULL;

    for (head=list->head, link=head->next;
            link != head;
            link  = link->next)
    {
        printf("+link: [%p]\n", link);
    }
}

#if RUN_LST

#include "log.h"

typedef struct _node
{
    int  v;
    Link link;
} node_t;

int main()
{
    int rv = 0;
    List *list = NULL;
    node_t *node = NULL;

    olog("hello, world");

    list = create_list();
    if (list == NULL)
    {
        elog("error: create_list");
        return -1;
    }
    olog("list created");

    node = calloc(1, sizeof(node_t));
    if (node == NULL)
    {
        elog("error: calloc");
        return -1;
    }

    node->v = 1;
    olog("%p", &node->link);

    list_add(list, &node->link);
    olog("node added");

    iterate_list(list);

    list_for_each_entry(node, list->head, link)
    {
        olog("node[%d]", node->v);
    }

    return rv;
}

#endif

/* list.c */
