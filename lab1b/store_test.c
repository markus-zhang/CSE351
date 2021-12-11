// Written by Porter Jones (pbjones@cs.washington.edu)
// inspired by the testing structure from CMU's bitwise operator lab.
// General structure is to test specific cases that are intended to
// provide good test coverage first, and then test a random range of
// cases to prevent students from hardcoding solutions for the specific cases.
// The random range is NOT intended to provide good test coverage.

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "store_client.h"
#include "test_util.h"

// If non-NULL, test only one function (-f)
static char* test_fname = NULL;
static bool grade_output = false;

#define NUM_AISLES 10
#define SECTIONS_PER_AISLE 4
#define NUM_ITEMS 64

#define NUM_STORE_TESTS 4


static unsigned long aisles_copy[NUM_AISLES];
static int stockroom_copy[NUM_ITEMS];

static unsigned long aisles_before[NUM_AISLES];
static int stockroom_before[NUM_ITEMS];

// prints state in aisles_before and stockroom_before
// Only prints stockroom if print_stockroom is nonzero
static void print_before_picture(int print_stockroom) {
  printf("\t\tAisles before function call:\n");
  for (int i = 0; i < NUM_AISLES; i++) {
    printf("\t\t    aisles[%d] (addr %p): 0x%016lx\n", i, &aisles[i], aisles_before[i]);
  }
  if (print_stockroom) {
    printf("\t\tStockroom before function call:\n");
    for (int i = 0; i < NUM_ITEMS; i += 4) {
      for (int j = 0; j < 4; j++) {
        printf("\t\t    stockroom[%d]: %d", i + j, stockroom_before[i + j]);
      }
      printf("\n");
    }
  }
}

// checks state in aisles and stockroom to see if
// it is the same as what is in aisles_copy and stockroom_copy
// Only checks stockroom if check_stockroom is nonzero
static int copies_equal_actual(int check_stockroom) {
  for (int i = 0; i < NUM_AISLES; i++) {
    if (aisles_copy[i] != aisles[i]) {
      printf("\t\terror: aisle value incorrect: aisles[%d]: actual 0x%016lx, expected 0x%016lx\n", i, aisles[i], aisles_copy[i]);
      return -1;
    }
  }
  if (check_stockroom) {
    for (int i = 0; i < NUM_ITEMS; i++) {
      if (stockroom_copy[i] != stockroom[i]) {
        printf("\t\terror: stockroom value incorrect: stockroom[%d]: actual %d, expected %d\n", i, stockroom[i], stockroom_copy[i]);
        return -1;
      }
    }
  }
  return 0;
}

// copies state in aisles and stockroom into aisles_copy and stockroom_copy
// Only copies stockroom if copy_stockroom is nonzero
static void copy_aisles_and_stockroom(int copy_stockroom) {
  for (int i = 0; i < NUM_AISLES; i++) {
    aisles_copy[i] = aisles[i];
  }

  if (copy_stockroom) {
    for (int i = 0; i < NUM_ITEMS; i++) {
      stockroom_copy[i] = stockroom[i];
    }
  }
}

// Copies state in aisles_before and stockroom_before
// back into aisles and stockroom
static void restore_before_aisles_and_stockroom() {
  for (int i = 0; i < NUM_AISLES; i++) {
    aisles[i] = aisles_before[i];
  }

  for (int i = 0; i < NUM_ITEMS; i++) {
    stockroom[i] = stockroom_before[i];
  }
}

// Sets up aisles and stockroom.
// If random is 0, then all aisles and stockroom are set to 0
// Otherwise aisles and stockroom set to random values
static void aisles_and_stockroom_setup(int random) {
  if (!random) {
    for (int i = 0; i < NUM_AISLES; i++) {
      aisles[i] = 0;
      aisles_before[i] = 0;
    }
    for (int i = 0; i < NUM_ITEMS; i++) {
      stockroom[i] = 0;
      stockroom_before[i] = 0;
    }
  } else {
    for (int i = 0; i < NUM_AISLES; i++) {
      aisles[i] = (unsigned) rand();
      aisles[i] = (aisles[i] << 32) | (unsigned) rand();
      aisles_before[i] = aisles[i];
    }
    for (int i = 0; i < NUM_ITEMS; i++) {
      stockroom[i] = rand();
      stockroom_before[i] = stockroom[i];
    }
  }
}

static int test_refill_helper(
  int stock2_before,
  int stock2_after,
  int stock3_before,
  int stock3_after,
  int aisle_index,
  unsigned long input_aisle,
  unsigned long after_aisle,
  int zero_out_aisles
) {

  aisles_and_stockroom_setup(0);
  stockroom[3] = stock3_before;
  stockroom[2] = stock2_before;
  aisles[4] = input_aisle;

  if (zero_out_aisles) {
    aisles_and_stockroom_setup(0);
    stockroom[2] = stock2_after;
    stockroom[3] = stock3_after;
    aisles[4] = after_aisle;
  } else {
    aisles_and_stockroom_setup(1);
    refill_from_stockroom_sol();
  }

  copy_aisles_and_stockroom(1);

  if (zero_out_aisles) {
    aisles_and_stockroom_setup(0);
    stockroom[2] = stock2_before;
    //stockroom_before[2] = stock2_before;
    stockroom[3] = stock3_before;
    //stockroom_before[3] = stock3_before;
    aisles[4] = input_aisle;
    //aisles_before[aisle_index] = input_aisle;
  } else {
    restore_before_aisles_and_stockroom();
  }

  refill_from_stockroom();

  if (copies_equal_actual(1) != 0) {
    if (zero_out_aisles) {
      printf("\t\t    aisles[%d] before: 0x%016lx\n", aisle_index, input_aisle);
      printf("\t\t    All other aisles were set to 0.\n");
      printf("\t\t    stockroom[2] before: %d\n", stock2_before);
      printf("\t\t    stockroom[3] before: %d\n", stock3_before);
      printf("\t\t    All other stockrooms were set to 0.\n");
    }
    return -1;
  }

  return 0;
}

// For testing refill from stockroom
// Tests cases when:
//    The ids are all full for a particular item
//    The inventory is empty for a particular item
//    Inventory count > empty spaces for an item
//    Inventory count < empty spaces for an item
static int refill_from_stockroom_tests() {
  int errors = 0, score = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");

  // stock2_before, stock2_after, stock3_before, stock3_after, aisle_index, input_aisle, after_aisle, zero_out_aisles
  errors += test_refill_helper(0, 0, 10, 10, 4, 0x080108000FFF0FFF, 0x080108000FFF0FFF, 1);
  errors += test_refill_helper(4, 0, 10, 6, 4, 0x080108000CFF0CFF, 0x0801080F0FFF0FFF, 1);

  if (errors == 0) score++;

  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  // bunch of random test cases to avoid hardcoding above cases
  for (int i = 0; i < 500; i++) {
    // If zero_out_aisles is 0 then none of the other params are used
    if (test_refill_helper(0, 0, 0, 0, 0, 0, 0, 0) != 0) {
      errors += -1;
      if (!grade_output) print_before_picture(1);
      break;
    }
  }

  if (errors == 0) score++;

  return score;
}

static int test_fulfill_helper(
  unsigned short id,
  int num,
  int stockroom_start,
  int stockroom_after,
  int aisle_index,
  unsigned long input_aisle,
  unsigned long after_aisle,
  int expected_ret,
  int zero_out_aisles
) {
  int result;

  if (zero_out_aisles) {
    aisles_and_stockroom_setup(0);
    stockroom[id] = stockroom_after;
    aisles[4] = after_aisle;
  } else {
    aisles_and_stockroom_setup(1);
    expected_ret = fulfill_order_sol(id, num);
  }

  copy_aisles_and_stockroom(1);

  if (zero_out_aisles) {
    aisles_and_stockroom_setup(0);
    stockroom[id] = stockroom_start;
    //stockroom_before[id] = stockroom_start;
    aisles[4] = input_aisle;
    //aisles_before[aisle_index] = input_aisle;
  } else {
    restore_before_aisles_and_stockroom();
  }

  result = fulfill_order(id, num);

  if (copies_equal_actual(1) != 0) {
    printf("\t\t    params: id=0x%04hx, num=%d\n", id, num);
    if (zero_out_aisles) {
      printf("\t\t    aisles[%d] before: 0x%016lx\n", aisle_index, input_aisle);
      printf("\t\t    All other aisles were set to 0.\n");
      printf("\t\t    stockroom[%d] before: %d\n", id, stockroom_start);
      printf("\t\t    All other stockrooms were set to 0.\n");
    }
    return -1;
  } else if (result != expected_ret) {
    printf("\t\terror: wrong return when id=0x%04hx, num=%d\n", id, num);
    printf("\t\t    actual return=%d, expected return=%d (aisles & stockroom were in the correct state)\n", result, expected_ret);
    if (zero_out_aisles) {
      printf("\t\t    aisles[%d] before: 0x%016lx\n", aisle_index, input_aisle);
      printf("\t\t    All other aisles were set to 0.\n");
      printf("\t\t    stockroom[%d] before: %d\n", id, stockroom_start);
      printf("\t\t    All other stockrooms were set to 0.\n");
    }
    return -1;
  }

  return 0;
}

// For testing fulfill order
// Tests cases when:
//    given id has no items
//    given id has more items than num in aisles
//    given id has less items than num in aisles
//    given id has more items than num in stockroom
//    given id has less items than num in stockroom
static int fulfill_order_tests() {
  int errors = 0, score = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");

  // id, num, stock_start, stock_after, aisle_index input_aisle, after_aisle, expected_ret, zero_out_aisles
  errors += test_fulfill_helper(2, 5, 0, 0, 4, 0x080008000CFF0CFF, 0x080008000CFF0CFF, 0, 1);
  errors += test_fulfill_helper(2, 5, 0, 0, 4, 0x080308000CFF0CFF, 0x080008000CFF0CFF, 2, 1);
  errors += test_fulfill_helper(2, 5, 0, 0, 4, 0x0803080F0CFF0CFF, 0x080208000CFF0CFF, 5, 1);
  errors += test_fulfill_helper(2, 5, 2, 0, 4, 0x080308000CFF0CFF, 0x080008000CFF0CFF, 4, 1);
  errors += test_fulfill_helper(2, 5, 5, 2, 4, 0x080308000CFF0CFF, 0x080008000CFF0CFF, 5, 1);

  if (errors == 0) score++;

  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  // bunch of random test cases to avoid hardcoding above cases
  for (int i = 0; i < 500; i++) {
    int num = rand();
    unsigned short id = (unsigned short) (rand() % NUM_ITEMS);

    // If zero_out_aisles is 0 then only id and num are used!
    if (test_fulfill_helper(id, num, 0, 0, 0, 0, 0, 0, 0) != 0) {
      errors += -1;
      if (!grade_output) print_before_picture(1);
      break;
    }

  }

  if (errors == 0) score++;

  return score;
}

static int test_empty_section_helper(
  unsigned short id,
  int aisle_index,
  unsigned long input_aisle,
  unsigned long expected_ret,
  int zero_out_aisles
) {
  unsigned long result;
  if (zero_out_aisles) aisles_and_stockroom_setup(0);

  aisles[aisle_index] = input_aisle;
  //aisles_before[aisle_index] = input_aisle;

  copy_aisles_and_stockroom(0);

  result = (unsigned long) empty_section_with_id(id);

  if (copies_equal_actual(0) != 0) {
    printf("\t\t    aisles changed when they shouldn't - param: id=0x%04hx\n", id);
    if (zero_out_aisles) {
      printf("\t\t    aisles[%d] before: 0x%lx\n", aisle_index, input_aisle);
      printf("\t\t    All other aisles were set to 0.\n");
    }
    return -1;
  }

  if (result != expected_ret) {
    printf("\t\terror: wrong return when id=0x%04hx -- actual return=%p, expected return=%p\n", id, (void*)result, (void*)expected_ret);
    if (zero_out_aisles) {
      printf("\t\t    &aisles[0]=%p, &aisles[%d]=%p\n", &aisles[0], aisle_index, &aisles[aisle_index]);
      printf("\t\t    aisles[%d] before: 0x%016lx\n", aisle_index, input_aisle);
      printf("\t\t    All other aisles were set to 0.\n");
    }
    return -1;
  }

  return 0;
}

// For testing empty_section_with_id
//
// Test cases where:
//    id has no empty sections
//    id has empty space after filled space
//    id has two empty spaces
static int empty_section_with_id_tests() {
  int errors = 0, score = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");

  // aisle_index
  // id
  // input_aisle
  // expected_ret
  // zero_out_aisles
  errors += test_empty_section_helper(
    3,
    4,
    0x080008000FFF0FFF,
    (unsigned long)NULL,
    1
  );

  errors += test_empty_section_helper(
    2,
    4,
    0x080008000C000FFF,
    (unsigned long)(((unsigned short*)&aisles[4]) + 2),
    1
  );

  errors += test_empty_section_helper(
    2,
    4,
    0x080008010C000FFF,
    (unsigned long)(((unsigned short*)&aisles[4]) + 3),
    1
  );

  if (errors == 0) score++;

  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  for (int i = 0; i < 500; i++) {
    unsigned short id = (unsigned short) (rand() % NUM_ITEMS);
    aisles_and_stockroom_setup(1);
    int index = rand() % 11;
    if (index < 10) {
      int section_index = rand() % 4;
      id = get_id_sol(&aisles[index], section_index);
      set_spaces_sol(&aisles[index], section_index, 0);
      aisles_before[index] = aisles[index];
    }

    unsigned long solution = (unsigned long) empty_section_with_id_sol(id);

    if (test_empty_section_helper(id, 0, aisles[0], solution, 0) != 0) {
      errors += -1;
      if (!grade_output) print_before_picture(0);
      break;
    }
  }

  if (errors == 0) score++;

  return score;
}

static int test_most_items_helper(
  int aisle_index,
  unsigned long input_aisle,
  unsigned long expected_ret,
  int zero_out_aisles
) {
  unsigned long result;
  if (zero_out_aisles) aisles_and_stockroom_setup(0);

  aisles[aisle_index] = input_aisle;
  //aisles_before[aisle_index] = input_aisle;

  copy_aisles_and_stockroom(0);

  result = (unsigned long) section_with_most_items();

  if (copies_equal_actual(0) != 0) {
    printf("\t\t    aisles changed when they shouldn't\n");
    if (zero_out_aisles) {
      printf("\t\t    aisles[%d] before: 0x%lx\n", aisle_index, input_aisle);
      printf("\t\t    All other aisles were set to 0.\n");
    }
    return -1;
  }

  if (result != expected_ret) {
    printf("\t\terror: wrong return -- actual return=%p, expected return=%p\n", (void*)result, (void*)expected_ret);
    if (zero_out_aisles) {
      printf("\t\t    &aisles[0]=%p, &aisles[%d]=%p\n", &aisles[0], aisle_index, &aisles[aisle_index]);
      printf("\t\t    aisles[%d] before: 0x%016lx\n", aisle_index, input_aisle);
      printf("\t\t    All other aisles were set to 0.\n");
    }
    return -1;
  }

  return 0;
}

// For testing section_with_most_items
//
// Test cases where:
//    All aisles are zero
//    There are no ties
//    There is a tie
static int section_with_most_items_tests() {
  int errors = 0, score = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");

  // aisle_index
  // input_aisle
  // expected_ret
  // zero_out_aisles
  errors += test_most_items_helper(
    4,
    0xFCFCFCFFFCFEFCDF,
    (unsigned long)(((unsigned short*)&aisles[4]) + 2),
    1
  );

  errors += test_most_items_helper(
    4,
    0xFCFCFCFFFCFFFCDF,
    (unsigned long)(((unsigned short*)&aisles[4]) + 1),
    1
  );

  errors += test_most_items_helper(
    1,
    0x0000000000000000,
    (unsigned long)(&aisles[0]),
    1
  );

  if (errors == 0) score++;

  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  for (int i = 0; i < 500; i++) {
    aisles_and_stockroom_setup(1);

    unsigned long solution = (unsigned long) section_with_most_items_sol();

    if (test_most_items_helper(0, aisles[0], solution, 0) != 0) {
      errors += -1;
      if (!grade_output) print_before_picture(0);
      break;
    }
  }

  if (errors == 0) score++;

  return score;
}

static test_info store_tests[] = {
  {"refill_from_stockroom", refill_from_stockroom_tests, 0, 2},
  {"fulfill_order", fulfill_order_tests, 0, 2},
  {"empty_section_with_id", empty_section_with_id_tests, 0, 2},
  {"section_with_most_items", section_with_most_items_tests, 0, 2}
};

// Run all tests
void run_tests() {
  int total = 0, expected_total = 0;

  if (!grade_output) {
		printf("Running store_client tests...\n\n");
    printf("-------------------------------------------------\n");
	} else {
		printf("[\n");
	}

  for (int i = 0; i < NUM_STORE_TESTS; i++) {
    test_info* test_info = &store_tests[i];
    if (!test_fname || strcmp(test_info->fname, test_fname) == 0) {
			if (grade_output) {
				printf("\t{\n");
				printf("\t\t\"errors\":\"");
			} else {
        printf("Testing %s...\n", test_info->fname);
      }
      test_info->score = test_info->run_tests();

      total += test_info->score;
      expected_total += test_info->points;

      if (!grade_output) {
        printf("\nFinished testing %s. Score: %d/%d\n", test_info->fname, test_info->score, test_info->points);
        printf("-------------------------------------------------\n");
			} else {
				printf("\",\n");
				printf("\t\t\"name\":\"%s\",\n\t\t\"score\":\"%d\"\n", test_info->fname, test_info->score);
				printf("\t}");
				if (i != NUM_STORE_TESTS - 1) printf(",");
				printf("\n");
			}
    }
  }

  if (expected_total == 0) {
    printf("ERROR: Bad function name given!\n");
  }

  if (!grade_output) {
    printf("Finished testing store_client!\n");
    printf("Summary:\n");
    for (int i = 0; i < NUM_STORE_TESTS; i++) {
      test_info* test_info = &store_tests[i];
      if (!test_fname || strcmp(test_info->fname, test_fname) == 0) {
        printf("\t%-24s\t%d/%d\n", test_info->fname, test_info->score, test_info->points);
      }
    }

		printf("\nTotal Score: %d/%d\n", total, expected_total);
	} else {
		printf("]\n");
	}
}

// Display usage info
static void usage() {
  printf("Usage: store-test [-f <name>]\n");
  printf("  -f <name> Test only the named function\n");
}

// Only one command line flag (-f test_name)
int main(int argc, char* argv[]) {
  int c;
  srand(1);
  if ((c = getopt(argc, argv, "gf:")) != -1) {
    if (c == 'f') {
      test_fname = strdup(optarg);
    } else if (c == 'g') {
			grade_output = true;
    } else {
      usage();
      exit(1);
    }
  }

  run_tests();

  return 0;
}
