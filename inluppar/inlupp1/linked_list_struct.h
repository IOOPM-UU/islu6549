#pragma once

#include <stddef.h>
#include "common.h"

typedef struct node node_t;

struct node {
    elem_t value;
    node_t *next;
};

struct list {
    size_t size;
    node_t *head;
    node_t *tail;
};