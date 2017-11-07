#ifndef __DC_LIST_H
#define __DC_LIST_H

/* This file is from Linux Kernel (include/linux/list.h) 
 * and modified by simply removing hardware prefetching of list items. 
 * Here by copyright, credits attributed to wherever they belong.
 * Kulesh Shanmugasundaram (kulesh [squiggly] isis.poly.edu)
 */

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

/* This file is simplified for dc-platform at 2015/4/20.
 */

typedef struct _t_link__ {
    struct _t_link__ *next;
    struct _t_link__ *prev;
} Link;

typedef struct {
    Link *head;
} List;


#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


/**
 * list_entry - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 **/
/*
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
*/
#define list_entry(ptr, type, member) \
    (type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member))


/**
 * list_for_each_entry  -       iterate over list of given type
 * @pos:        the type * to use as a loop counter.
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)                          \
    for (pos = list_entry((head)->next, __typeof__(*pos), member);      \
            &pos->member != (head);                                    \
            pos = list_entry(pos->member.next, __typeof__(*pos), member))

#define list_add(list, link)    list_add_tail(list, link)

List *create_list(void);
void destroy_list(List *_list);
void list_add_head(List *_list, Link *_new);
void list_add_tail(List *_list, Link *_new);
void list_del(Link *entry);
int  list_empty(List *_list);

#endif
