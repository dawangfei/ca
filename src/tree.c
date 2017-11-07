/*
 * Copyright (C) Igor Sysoev
 */
#include "tree.h"


/*
 * The red-black tree code is based on the algorithm described in
 * the "Introduction to Algorithms" by Cormen, Leiserson and Rivest.
 *
 */

#define rb_t_red(node)               ((node)->color = 1)
#define rb_t_black(node)             ((node)->color = 0)
#define rb_t_is_red(node)            ((node)->color)
#define rb_t_is_black(node)          (!rb_t_is_red(node))
#define rb_t_copy_color(n1, n2)      (n1->color = n2->color)



/* a sentinel must be black */
#define rb_tree_sentinel_init(node)  rb_t_black(node)


typedef void (*rb_tree_insert_rb) (rb_tree_node_t *_root,
        rb_tree_node_t *_node,
        rb_tree_node_t *_sentinel);

static rb_tree_insert_rb    g_insert_function_list[RB_INSERT_FUNCTION_MAX];


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


static void rb_tree_left_rotate(rb_tree_node_t **root,
        rb_tree_node_t *sentinel,
        rb_tree_node_t *node);

static void rb_tree_right_rotate(rb_tree_node_t **root,
        rb_tree_node_t *sentinel,
        rb_tree_node_t *node);

static rb_tree_node_t * rb_tree_min(rb_tree_node_t *node, rb_tree_node_t *sentinel);
static rb_tree_node_t * rb_tree_max(rb_tree_node_t *node, rb_tree_node_t *sentinel);


/* customized function pointer */
static void rb_tree_insert_value(rb_tree_node_t *root,
        rb_tree_node_t *node,
        rb_tree_node_t *sentinel);

static void rb_tree_insert_string(rb_tree_node_t *root,
        rb_tree_node_t *node,
        rb_tree_node_t *sentinel);


/* init a tree */
void  rb_tree_init(rb_tree_t *tree, rb_tree_node_t *sentinel, int idx)
{
    rb_tree_sentinel_init(sentinel);
    tree->root = sentinel;
    tree->sentinel = sentinel;

    tree->used_num = 0;

    tree->function_index = idx;

}


/* set insert function */
static int rb_tree_set_func(int _idx, rb_tree_insert_rb _insert)
{
    if (_idx >= RB_INSERT_FUNCTION_MAX)
    {
        return -1;
    }

    g_insert_function_list[_idx] = _insert;

    return 0;
}


/**
 *
 *  XXX: This function must be called before init a tree.
 *
 */
int rb_tree_register()
{
    /* key is long */
    if (rb_tree_set_func(RB_INSERT_FUNCTION_LONG,   rb_tree_insert_value))
    {
        return -1;
    }

    /* key is string */
    if (rb_tree_set_func(RB_INSERT_FUNCTION_STRING, rb_tree_insert_string))
    {
        return -1;
    }

    return 0;
}


void rb_tree_set_pre_alloc_mode(rb_tree_t *_tree)
{
    _tree->pre_alloc_mode = 1;
}


void rb_tree_insert(rb_tree_t *tree, rb_tree_node_t *node)
{
    rb_tree_node_t  **root, *temp, *sentinel;

    /* a binary tree insert */

    root = (rb_tree_node_t **) &tree->root;
    sentinel = tree->sentinel;
    tree->used_num++;

    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        rb_t_black(node);
        *root = node;

        return;
    }

    /***
      tree->insert(*root, node, sentinel);
      rb_tree_insert_string(*root, node, sentinel);
      _g_tree_insert(*root, node, sentinel);
    ***/
    g_insert_function_list[tree->function_index](*root, node, sentinel);

    /* re-balance tree */

    while (node != *root && rb_t_is_red(node->parent)) {

        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;

            if (rb_t_is_red(temp)) {
                rb_t_black(node->parent);
                rb_t_black(temp);
                rb_t_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rb_tree_left_rotate(root, sentinel, node);
                }

                rb_t_black(node->parent);
                rb_t_red(node->parent->parent);
                rb_tree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else {
            temp = node->parent->parent->left;

            if (rb_t_is_red(temp)) {
                rb_t_black(node->parent);
                rb_t_black(temp);
                rb_t_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rb_tree_right_rotate(root, sentinel, node);
                }

                rb_t_black(node->parent);
                rb_t_red(node->parent->parent);
                rb_tree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    rb_t_black(*root);
} /* rb_tree_insert */


void rb_tree_delete(rb_tree_t *tree,
        rb_tree_node_t *node)
{
    unsigned int red;
    rb_tree_node_t  **root, *sentinel, *subst, *temp, *w;

    /* a binary tree delete */

    root = (rb_tree_node_t **) &tree->root;
    sentinel = tree->sentinel;
    tree->used_num--;

    if (node->left == sentinel) {
        temp = node->right;
        subst = node;

    } else if (node->right == sentinel) {
        temp = node->left;
        subst = node;

    } else {
        subst = rb_tree_min(node->right, sentinel);

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    if (subst == *root) {
        *root = temp;
        rb_t_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    red = rb_t_is_red(subst);

    if (subst == subst->parent->left) {
        subst->parent->left = temp;

    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {

        temp->parent = subst->parent;

    } else {

        if (subst->parent == node) {
            temp->parent = subst;

        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        rb_t_copy_color(subst, node);

        if (node == *root) {
            *root = subst;

        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) {
        return;
    }

    /* a delete fixup */

    while (temp != *root && rb_t_is_black(temp)) {

        if (temp == temp->parent->left) {
            w = temp->parent->right;

            if (rb_t_is_red(w)) {
                rb_t_black(w);
                rb_t_red(temp->parent);
                rb_tree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            if (rb_t_is_black(w->left) && rb_t_is_black(w->right)) {
                rb_t_red(w);
                temp = temp->parent;

            } else {
                if (rb_t_is_black(w->right)) {
                    rb_t_black(w->left);
                    rb_t_red(w);
                    rb_tree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                rb_t_copy_color(w, temp->parent);
                rb_t_black(temp->parent);
                rb_t_black(w->right);
                rb_tree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }

        } else {
            w = temp->parent->left;

            if (rb_t_is_red(w)) {
                rb_t_black(w);
                rb_t_red(temp->parent);
                rb_tree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (rb_t_is_black(w->left) && rb_t_is_black(w->right)) {
                rb_t_red(w);
                temp = temp->parent;

            } else {
                if (rb_t_is_black(w->left)) {
                    rb_t_black(w->right);
                    rb_t_red(w);
                    rb_tree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                rb_t_copy_color(w, temp->parent);
                rb_t_black(temp->parent);
                rb_t_black(w->left);
                rb_tree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    rb_t_black(temp);
} /* rb_tree_delete */


/**
 * destroy a tree
 */
int rb_tree_destroy(rb_tree_t *tree)
{
    rb_tree_node_t *node = NULL;

    if (tree == NULL) {
        return 0;
    }

    if (tree->pre_alloc_mode)
    {
        memset(tree->head_addr, 0, tree->node_size*(tree->free_num+tree->used_num));
        tree->head_addr = NULL;
        tree->free_list = NULL;
    }
    else
    {
        node = tree->root;
        while (node != tree->sentinel) {
            rb_tree_delete(tree, node);
            free(node);
            node = tree->root;
        }
        node = NULL;
    }

    return 0;
} /* rb_tree_destroy */


static void rb_tree_insert_value(rb_tree_node_t *temp,
        rb_tree_node_t *node,
        rb_tree_node_t *sentinel)
{
    rb_tree_node_t  **p;

    for ( ;; ) {

        p = (node->key < temp->key) ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    rb_t_red(node);
} /* rb_tree_insert_value */


static void rb_tree_insert_string(rb_tree_node_t *temp,
        rb_tree_node_t *node,
        rb_tree_node_t *sentinel)
{
    int cmp_len = 0;
    rb_tree_node_t  **p;


    for ( ;; ) {

        cmp_len = ((node->key_len > temp->key_len) ? node->key_len : temp->key_len);

        p = (memcmp(node->key_val, temp->key_val, cmp_len) < 0) ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    rb_t_red(node);
} /* rb_tree_insert_string */



static void rb_tree_left_rotate(rb_tree_node_t **root,
        rb_tree_node_t *sentinel,
        rb_tree_node_t *node)
{
    rb_tree_node_t  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
} /* rb_tree_left_rotate */


static void rb_tree_right_rotate(rb_tree_node_t **root,
        rb_tree_node_t *sentinel,
        rb_tree_node_t *node)
{
    rb_tree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->right) {
        node->parent->right = temp;

    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
} /* rb_tree_right_rotate */


static rb_tree_node_t *rb_tree_min(rb_tree_node_t *node, rb_tree_node_t *sentinel)
{
    while (node->left != sentinel) {
        node = node->left;
    }

    return node;
}


static rb_tree_node_t *rb_tree_max(rb_tree_node_t *node, rb_tree_node_t *sentinel)
{
    while (node->right != sentinel) {
        node = node->right;
    }

    return node;
}

rb_tree_node_t * rb_tree_get_min(rb_tree_t *tree)
{
    return rb_tree_min(tree->root, tree->sentinel);
}

rb_tree_node_t * rb_tree_get_max(rb_tree_t *tree)
{
    return rb_tree_max(tree->root, tree->sentinel);
}


/**
 *  recursive mode: left -> root -> right
 */
void rb_tree_iterate_middle(rb_tree_node_t *root,
        rb_tree_node_t *sentinel)
{
    static int counter = 0;

    if (!root || root == sentinel) {
        return ;
    }


    rb_tree_iterate_middle(root->left, sentinel);

    counter++;
    printf("key [%s] => [%s] node %03dth [%p]\n",
            root->key_val, (char *)root->data_unit+64, counter, root);

    rb_tree_iterate_middle(root->right, sentinel);

    return ;
}


/**
 * find by rb_tree_key_t: key
 */
rb_tree_node_t * rb_tree_find_by_key(rb_tree_t *tree, rb_tree_key_t key)
{
    rb_tree_node_t *node = NULL;

    if (tree == NULL)
    {
        return NULL;
    }

    node = tree->root;

    if (!node || node == tree->sentinel)
    {
        return NULL;
    }

    while (node != NULL)
    {
        if (key == node->key) {
            break;
        }
        else if (key < node->key) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    return node;
}


/**
 * find by node.key_val
 */
rb_tree_node_t * rb_tree_find_by_string(rb_tree_t *tree, char *_key, int _key_len)
{
    int rv = 0;
    int key_cmp_len = 0;
    rb_tree_node_t *node = NULL;
#define RB_TREE_MAX_KEY_LEN     256
    char buffer[RB_TREE_MAX_KEY_LEN+1] = {0};

    if (tree == NULL)
    {
        return NULL;
    }

    if (_key_len <= 0 || _key_len > RB_TREE_MAX_KEY_LEN || _key_len > tree->key_max_len)
    {
        printf("error: ...\n");
        return NULL;
    }

    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, _key, _key_len);

    node = tree->root;
    if (!node || node == tree->sentinel)
    {
        return NULL;
    }

    while (node != NULL)
    {
        key_cmp_len = ((node->key_len > _key_len) ? node->key_len : _key_len);
        rv = memcmp(buffer, node->key_val, key_cmp_len);
        if (rv == 0)
        {
            break;
        }
        else if (rv < 0)
        {
            node = node->left;
        }
        else
        {
            node = node->right;
        }
    }

    return node;
}


#if RUN_RBTREE
int  main(int argc, char *argv[])
{
    int   i    = 0;
    int   max  = 10;
    int   cap  = 0;
    int   size = 0;
    int   key_len = 100;
    void *addr = NULL;
    rb_tree_node_t *node = NULL;
    rb_tree_t tree;

    (void)argc;
    (void)argv;

    cap  = 1000;
    size = cap * 1024;

    addr = malloc(size);
    assert(addr != NULL);

    rb_tree_register();

    memset(&tree, 0, sizeof(tree));
    rb_tree_init(&tree, &tree.sentinel_instance, RB_INSERT_FUNCTION_STRING);
    tree.key_max_len = 100;
    tree.node_size   = 200;

    for (i = 0; i < max; i++)
    {
        /* allocate a node */
        {
            node = calloc(1, sizeof(rb_tree_node_t)+key_len);
            assert(node != NULL);

            node->key_len = snprintf(node->key_val, key_len+1, "%d", i);
            node->data_unit = (void *)(long)(max-i);
        }

        rb_tree_insert(&tree, node);
        printf("root1: [%s]\n", tree.root->key_val);
    }

    node = rb_tree_get_min(&tree);
    printf("min: [%s][%p]\n", node->key_val, node->data_unit);

    node = rb_tree_get_max(&tree);
    printf("max: [%s][%p]\n", node->key_val, node->data_unit);

    node = rb_tree_find_by_string(&tree, "8", 1);
    if (node != NULL)
    {
        printf("sea: [%s][%p]\n", node->key_val, node->data_unit);
    }
    else
    {
        printf("sea: not found\n");
    }

    return 0;
}
#endif

/* rb_tree.c  */
