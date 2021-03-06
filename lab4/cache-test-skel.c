/*
YOUR NAME HERE
CSE 351 - Autumn 2015
Lab 4 - Mystery Caches

Mystery Cache Geometries:
mystery0:
    block size =
    cache size =
    associativity =
mystery1:
    block size =
    cache size =
    associativity =
mystery2:
    block size =
    cache size =
    associativity =
mystery3:
    block size =
    cache size =
    associativity =
*/

#include <stdlib.h>
#include <stdio.h>

#include "mystery-cache.h"

/*
 * NOTE: When using access_cache() you do not need to provide a "real" memory
 * addresses. You can use any convenient integer value as a memory address.
 * You should not be able to cause a segmentation fault by providing a memory
 * address out of your programs address space as the argument to access_cache.
 */

/*
   Returns the size (in B) of each block in the cache.
*/
int get_block_size(void) {
  /* YOUR CODE GOES HERE */
  access_cache(0);
  int address = 0;

  while (access_cache(address++)) {

  }

  return address - 1;
}

/*
   Returns the size (in B) of the cache.
*/
int get_cache_size(int block_size) {
  /* YOUR CODE GOES HERE */
  // We can access_cache(0), given say 2 bits of block offset, then test for 0b100
  // If the set has say 4 bits, then 1000000b and 0000000b must be stored in the same set.
  // However still don't know how to find the tag bits... 
  return -1;
}

/*
   Returns the associativity of the cache.
*/
int get_cache_assoc(int cache_size) {
  /* YOUR CODE GOES HERE */
  return -1;
}

int main(void) {
  int size;
  int assoc;
  int block_size;

  cache_init(0, 0);

  block_size=get_block_size();
  size=get_cache_size(block_size);
  assoc=get_cache_assoc(size);

  printf("Cache block size: %d bytes\n", block_size);
  printf("Cache size: %d bytes\n", size);
  printf("Cache associativity: %d\n", assoc);

  return EXIT_SUCCESS;
}
