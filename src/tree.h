/*
 * Copyright (C) Igor Sysoev
 *
 * from Nignx
 *
 * refined for shared memory version
 *
 */
#ifndef _DC_RB_TREE_H_INCLUDED_
#define _DC_RB_TREE_H_INCLUDED_

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

/*
#include "stack.h"
#include "queue.h"
*/

typedef unsigned long rb_tree_key_t;

typedef struct rb_tree_node_s rb_tree_node_t;

struct rb_tree_node_s {
    rb_tree_key_t   key;

    rb_tree_node_t  *link;

    rb_tree_node_t  *left;
    rb_tree_node_t  *right;
    rb_tree_node_t  *parent;
    u_char          color;

    void            *data_unit;
    int             data_len;
    int             key_len;
    char            key_val[1];     /* must at end */
};


typedef struct rb_tree_s {
    rb_tree_node_t      *free_list;
    int                 free_num;


    rb_tree_node_t      *root;
    rb_tree_node_t      *sentinel;
    rb_tree_node_t      sentinel_instance;  /* shm version only */
    int                 used_num;

    int                 function_index;     /* shm version only */
    int                 pre_alloc_mode;     /* compatible mode */

    void                *head_addr;
    int                 key_max_len;
    int                 node_size;
} rb_tree_t;


/* insert function */
#define     RB_INSERT_FUNCTION_MAX      10
enum {
    RB_INSERT_FUNCTION_LONG = 0,
    RB_INSERT_FUNCTION_STRING
};

/* register all functions */
int rb_tree_register();

/* init a tree */
void  rb_tree_init(rb_tree_t *tree, rb_tree_node_t *sentinel, int idx);

/*  */
void rb_tree_set_pre_alloc_mode(rb_tree_t *_tree);

/* insert an allocated node to tree. */
void rb_tree_insert(rb_tree_t *tree, rb_tree_node_t *node);

/* extract a node from tree. Note: the node is NOT freed. */
void rb_tree_delete(rb_tree_t *tree, rb_tree_node_t *node);

/* destroy a tree */
int rb_tree_destroy(rb_tree_t *rb_tree);

/* ====================================================================== */ 
/* ====================================================================== */ 
/* ====================================================================== */ 

/* get min/max from tree */
rb_tree_node_t * rb_tree_get_min(rb_tree_t *tree);
rb_tree_node_t * rb_tree_get_max(rb_tree_t *tree);

/* iterate: left -> root -> right (min -> max) */
void rb_tree_iterate_middle(rb_tree_node_t *root, rb_tree_node_t *sentinel);

/* search by key */
rb_tree_node_t * rb_tree_find_by_key(rb_tree_t *tree, rb_tree_key_t key);
rb_tree_node_t * rb_tree_find_by_string(rb_tree_t *tree, char *str_key, int len);


#endif
