// Written by Porter Jones (pbjones@cs.washington.edu)
// inspired by print_binary function from CMU's bitwise operator lab
//
// Functions to print binary representations of values

#include <stdio.h>
#include "store_util.h"

// Prints binary version of given short value,
// msb -> lsb in groups of 4
void print_binary_short(unsigned short value) {
  int i;
  // start at MSB and go backwards (decrement)
  for (i = 15; i >= 0; --i) {
    // print the i-th bit (0 or 1)
    printf("%d", (value >> i) & 1);
    // add space for readability
    if (i % 8 == 0) printf(" ");
    if (i % 4 == 0) printf(" ");
  }
  printf("\n");
}

// Prints binary version of given short value,
// msb -> lsb in groups of 8
void print_binary_long(unsigned long value) {
  int i;
  // start at MSB and go backwards (decrement)
  for (i = 63; i >= 0; --i) {
    // print the i-th bit (0 or 1)
    printf("%lu", (value >> i) & 1);
    // add space for readability
    if (i % 16 == 0) printf(" ");
    if (i % 8 == 0) printf(" ");
  }
  printf("\n");
}
