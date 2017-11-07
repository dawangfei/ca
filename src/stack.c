#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"


dc_stack_t *dc_stack_create(int capacity)
{
    dc_stack_t *s = NULL;

    if (capacity <= 0)
    {
        return NULL;
    }

    s = (dc_stack_t *)calloc(1, sizeof(dc_stack_t));
    if (s == NULL)
    {
        return NULL;
    }

    s->buffer = (char **)calloc(capacity, sizeof(char *));
    if (s->buffer == NULL)
    {
        free(s);
        return NULL;
    }

    s->top = -1;
    s->capacity = capacity;

    return s;
}


void dc_stack_destroy(dc_stack_t *_s)
{
    if (_s != NULL)
    {
        if (_s->buffer != NULL)
        {
            free(_s->buffer);
        }

        free(_s);
    }

    return;
}


void dc_stack_reset(dc_stack_t *s)
{
    s->top = -1;
    memset(s->buffer, 0, s->capacity*sizeof(char *));
}


char *dc_stack_top(dc_stack_t *s)
{
    char *p = NULL;

    if (!dc_stack_is_empty(s))
    {
        p = s->buffer[s->top];
    }

    return p;
}


int dc_stack_push(dc_stack_t *s, char *ptr)
{
    if (ptr == NULL)
    {
        return -1;
    }

    if (dc_stack_is_full(s))
    {
        return -1;
    }

    s->top++;
    s->buffer[s->top] = ptr;

    return 0;
}


char * dc_stack_pop(dc_stack_t *s)
{
    if (dc_stack_is_empty(s))
    {
        return NULL;
    }

    return s->buffer[s->top--];
}


/*****************************************************************************/

#ifdef RUN_STACK
static int test1()
{
    dc_stack_t *s;
    char *ptr;
    int counter = 0;

    printf("hello, %s\n", __func__);

    s = dc_stack_create(20);

    ptr = (char *)2;
    if (dc_stack_push(s, ptr))
    {
        printf("error: dc_stack_push: %p\n", ptr);
        return -1;
    }

    ptr = (char *)8;
    if (dc_stack_push(s, ptr))
    {
        printf("error: dc_stack_push: %p\n", ptr);
        return -1;
    }

    ptr = (char *)5;
    if (dc_stack_push(s, ptr))
    {
        printf("error: dc_stack_push: %p\n", ptr);
    }

    ptr = (char *)6;
    if (dc_stack_push(s, ptr))
    {
        printf("error: dc_stack_push: %p\n", ptr);
    }

    ptr = (char *)7;
    dc_stack_push(s, ptr);

    ptr = (char *)9;
    dc_stack_push(s, ptr);

    while (!dc_stack_is_empty(s))
    {
        ptr = dc_stack_pop(s);
        counter++;
        printf("pop %p\n", ptr);
    }

    printf("total %d are poped\n", counter);

    dc_stack_destroy(s);

    printf("bye, %s\n", __func__);

    return 0;
}


static int test2(int max)
{
    dc_stack_t *s;
    char *ptr;
    int counter = 0;
    int i;

    printf("hello, %s\n", __func__);

    s = dc_stack_create(2);
    for (i = 1; i < max; i++)
    {
        ptr = (char *)((long)i);
        dc_stack_push(s, ptr);
    }

    while (!dc_stack_is_empty(s))
    {
        ptr = dc_stack_pop(s);
        counter++;
        printf("pop %p\n", ptr);
    }

    printf("total %d are poped\n", counter);

    dc_stack_destroy(s);
    printf("after destory: %p\n", s);

    printf("bye, %s\n", __func__);

    return 0;
}

int main(int argc, char *argv[])
{
    printf("hello, dc_stack\n");

    test1();

    test2(100);

    printf("bye, dc_stack\n");

    return 0;
}
#endif
