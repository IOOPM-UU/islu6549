#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef union elem elem_t;

// Generalized union for variables
union elem
{
  int i;
  unsigned int u;
  bool b;
  float f;
  void *p;
  char *s;
};

// function type for two elem comparing values and see if equal
typedef bool ioopm_eq_function(elem_t a, elem_t b);

// function type for hashing an elem
typedef size_t ioopm_hash_function(elem_t key);

// create en elem_t containing an int
#define int_elem(x)   ((elem_t) { .i = (x) })
// create en elem_t containing a bool
#define bool_elem(x)  ((elem_t) { .b = (x) })
// create en elem_t containing a pointer
#define ptr_elem(x)   ((elem_t) { .p = (x) })
// create en elem_t containing a string
#define string_elem(x) ((elem_t) { .s = (x) })