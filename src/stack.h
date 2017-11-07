#ifndef _DC_STACK_H_INCLUDED
#define _DC_STACK_H_INCLUDED

#define  dc_stack_size(s)      (s->top+1)
#define  dc_stack_is_empty(s)  (dc_stack_size(s) == 0)
#define  dc_stack_is_full(s)   (dc_stack_size(s) == s->capacity)

typedef struct _dc_stack {
    char **buffer;
    unsigned int  top;
    unsigned int  capacity;
} dc_stack_t;

dc_stack_t *dc_stack_create(int capacity);

int  dc_stack_push(dc_stack_t *s, char *ptr);

char *dc_stack_pop(dc_stack_t *s);

char *dc_stack_top(dc_stack_t *s);

void dc_stack_reset(dc_stack_t *s);

void dc_stack_destroy(dc_stack_t *s);

#endif
