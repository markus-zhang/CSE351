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
#include "test_util.h"
#include "aisle_manager.h"

// If non-NULL, test only one function (-f)
static char* test_fname = NULL;
static bool grade_output = false;

#define NUM_AISLE_TESTS 12

int get_test_helper(get_funct get_function, unsigned long* aisle, int index, unsigned short expected_val) {
  unsigned short actual_val = get_function(aisle, index);

  if (actual_val != expected_val) {
    printf("\t\terror: *aisle=0x%016lx, index=%d, expected_val=0x%04hx, actual_val=0x%04hx\n", *aisle, index, expected_val, actual_val);
    return -1;
  }
  return 0;
}

int get_random_tests(get_funct get_function, get_funct get_solution) {
  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  for (int i = 0; i < 500; i++) {
    int index = rand() % 4;
    unsigned long aisle = rand();
    aisle = (aisle << 32) | rand();
    if (get_test_helper(get_function, &aisle, index, get_solution(&aisle, index)) != 0) {
      return -1;
    }
  }

  return 0;
}

static int get_section_tests() {
  unsigned long aisle;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA9876543210;
  errors += get_test_helper(get_section, &aisle, 0, 0x3210);
  errors += get_test_helper(get_section, &aisle, 1, 0x7654);
  errors += get_test_helper(get_section, &aisle, 2, 0xBA98);
  errors += get_test_helper(get_section, &aisle, 3, 0xFEDC);

  errors += get_random_tests(get_section, get_section_sol);

  return errors == 0;
}

static int get_id_tests() {
  unsigned long aisle;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA9876543210;
  errors += get_test_helper(get_id, &aisle, 0, 0x0C);
  errors += get_test_helper(get_id, &aisle, 2, 0x2E);

  errors += get_random_tests(get_id, get_id_sol);

  return errors == 0;
}

static int get_spaces_tests() {
  unsigned long aisle;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFFFFBA9876543210;
  errors += get_test_helper(get_spaces, &aisle, 0, 0x210);
  errors += get_test_helper(get_spaces, &aisle, 2, 0x298);
  errors += get_test_helper(get_spaces, &aisle, 3, 0x3FF);

  errors += get_random_tests(get_spaces, get_spaces_sol);

  return errors == 0;
}

static int num_items_tests() {
  unsigned long aisle;
  int score = 0;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xF000BA987FFF3210;
  errors += get_test_helper(num_items, &aisle, 0, 2);
  errors += get_test_helper(num_items, &aisle, 1, 10);
  errors += get_test_helper(num_items, &aisle, 2, 4);
  errors += get_test_helper(num_items, &aisle, 3, 0);

  if (errors == 0) score += 2;

  errors += get_random_tests(num_items, num_items_sol);

  if (errors == 0) score++;

  return score;
}

int set_test_helper(set_funct set_function, unsigned long* aisle, int index, unsigned short new_section, unsigned long expected_after) {
  unsigned long before_aisle = *aisle;

  set_function(aisle, index, new_section);

  if (*aisle != expected_after) {
    printf("\t\terror: index=%d, new_pattern=0x%04hx\n", index, new_section);

    printf("\t\t    before *aisle=0x%016lx -- after *aisle=0x%016lx, expected *aisle=0x%016lx\n", before_aisle, *aisle, expected_after);
    return -1;
  }
  return 0;
}

int set_random_tests(set_funct set_function, set_funct set_solution) {
  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  for (int i = 0; i < 500; i++) {
    int index = rand() % 4;
    unsigned short new_section = rand() % 65536;
    unsigned long aisle = rand();
    aisle = (aisle << 32) | rand();
    unsigned long aisle_sol = aisle;

    set_solution(&aisle_sol, index, new_section);

    if (set_test_helper(set_function, &aisle, index, new_section, aisle_sol) != 0) {
      return -1;
    }
  }

  return 0;
}

static int set_section_tests() {
  unsigned long aisle;
  int score = 0;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA9876543210;
  errors += set_test_helper(set_section, &aisle, 0, 0xABCD, 0xFEDCBA987654ABCD);
  aisle = 0xFEDCBA987654ABCD;
  errors += set_test_helper(set_section, &aisle, 1, 0x1111, 0xFEDCBA981111ABCD);
  aisle = 0xFEDCBA981111ABCD;
  errors += set_test_helper(set_section, &aisle, 2, 0xCAFE, 0xFEDCCAFE1111ABCD);
  aisle = 0xFEDCCAFE1111ABCD;
  errors += set_test_helper(set_section, &aisle, 3, 0x0E0C, 0x0E0CCAFE1111ABCD);

  if (errors == 0) score++;

  errors += set_random_tests(set_section, set_section_sol);

  if (errors == 0) score++;

  return score;
}

static int set_spaces_tests() {
  unsigned long aisle;
  int score = 0;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA9876543210;
  errors += set_test_helper(set_spaces, &aisle, 0, 0xABCD, 0xFEDCBA9876543210);
  aisle = 0xFEDCBA9876543210;
  errors += set_test_helper(set_spaces, &aisle, 2, 0xAC00, 0xFEDCBA9876543210);
  aisle = 0xFEDCBA9876543210;
  errors += set_test_helper(set_spaces, &aisle, 1, 0x0175, 0xFEDCBA9875753210);
  aisle = 0xFEDCBA9875753210;
  errors += set_test_helper(set_spaces, &aisle, 3, 0x0000, 0xFC00BA9875753210);
  aisle = 0xFC00BA9875753210;
  errors += set_test_helper(set_spaces, &aisle, 0, 0x03FF, 0xFC00BA98757533FF);

  if (errors == 0) score++;

  errors += set_random_tests(set_spaces, set_spaces_sol);

  if (errors == 0) score++;

  return score;
}

static int set_id_tests() {
  unsigned long aisle;
  int score = 0;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA9876543210;
  errors += set_test_helper(set_id, &aisle, 0, 0xABCD, 0xFEDCBA9876543210);
  aisle = 0xFEDCBA9876543210;
  errors += set_test_helper(set_id, &aisle, 2, 0xAFC0, 0xFEDCBA9876543210);

  aisle = 0xFEDCBA9876543210;
  errors += set_test_helper(set_id, &aisle, 1, 0x0017, 0xFEDCBA985E543210);
  aisle = 0xFEDCBA985E543210;
  errors += set_test_helper(set_id, &aisle, 3, 0x0000, 0x02DCBA985E543210);
  aisle = 0x02DCBA985E543210;
  errors += set_test_helper(set_id, &aisle, 0, 0x003F, 0x02DCBA985E54FE10);

  if (errors == 0) score++;

  errors += set_random_tests(set_id, set_id_sol);

  if (errors == 0) score++;

  return score;
}

int items_test_helper(items_funct items_function, unsigned long* aisle, int index, int n, unsigned long expected_after) {
  unsigned long before_aisle = *aisle;

  items_function(aisle, index, n);

  if (*aisle != expected_after) {
    printf("\t\terror: index=%d, n=%d\n", index, n);

    printf("\t\t    before *aisle=0x%016lx -- after *aisle=0x%016lx, expected *aisle=0x%016lx\n", before_aisle, *aisle, expected_after);
    return -1;
  }
  return 0;
}

int items_random_tests(items_funct items_function, items_funct items_solution) {
  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  for (int i = 0; i < 500; i++) {
    int index = rand() % 4;
    int n = rand() % 20;
    unsigned long aisle = rand();
    aisle = (aisle << 32) | rand();
    unsigned long aisle_sol = aisle;

    items_solution(&aisle_sol, index, n);

    if (items_test_helper(items_function, &aisle, index, n, aisle_sol) != 0) {
      return -1;
    }
  }

  return 0;
}

static int add_items_tests() {
  unsigned long aisle;
  int score = 0;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA977C003210;
  errors += items_test_helper(add_items, &aisle, 0, 0, 0xFEDCBA977C003210);
  aisle = 0xFEDCBA977C003210;
  errors += items_test_helper(add_items, &aisle, 1, 10, 0xFEDCBA977FFF3210);
  aisle = 0xFEDCBA977FFF3210;
  errors += items_test_helper(add_items, &aisle, 2, 6, 0xFEDCBBFF7FFF3210);
  aisle = 0xFEDCBBFF7FFF3210;
  errors += items_test_helper(add_items, &aisle, 3, 3, 0xFEFFBBFF7FFF3210);

  if (errors == 0) score += 2;

  errors += items_random_tests(add_items, add_items_sol);

  if (errors == 0) score++;

  return score;
}

static int remove_items_tests() {
  unsigned long aisle;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA977FFF3210;
  errors += items_test_helper(remove_items, &aisle, 0, 0, 0xFEDCBA977FFF3210);
  aisle = 0xFEDCBA977FFF3210;
  errors += items_test_helper(remove_items, &aisle, 1, 10, 0xFEDCBA977C003210);
  aisle = 0xFEDCBA977C003210;
  errors += items_test_helper(remove_items, &aisle, 2, 4, 0xFEDCBA807C003210);
  aisle = 0xFEDCBA807C003210;
  errors += items_test_helper(remove_items, &aisle, 3, 8, 0xFC00BA807C003210);

  errors += items_random_tests(remove_items, remove_items_sol);

  return errors == 0;
}

static int rotate_items_left_tests() {
  unsigned long aisle;
  int score = 0;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFB11998772553210;
  errors += items_test_helper(rotate_items_left, &aisle, 0, 0, 0xFB11998772553210);
  aisle = 0xFB11998772553210;
  errors += items_test_helper(rotate_items_left, &aisle, 0, 10, 0xFB11998772553210);

  aisle = 0xFB11998772553210;
  errors += items_test_helper(rotate_items_left, &aisle, 1, 1, 0xFB11998770AB3210);
  aisle = 0xFB11998770AB3210;
  errors += items_test_helper(rotate_items_left, &aisle, 2, 9, 0xFB119AC370AB3210);
  aisle = 0xFB119AC370AB3210;
  errors += items_test_helper(rotate_items_left, &aisle, 3, 3, 0xF88E9AC370AB3210);
  aisle = 0xF88E9AC370AB3210;
  errors += items_test_helper(rotate_items_left, &aisle, 3, 14, 0xF8E29AC370AB3210);

  if (errors == 0) score += 2;

  errors += items_random_tests(rotate_items_left, rotate_items_left_sol);

  if (errors == 0) score++;

  return score;
}

static int rotate_items_right_tests() {
  unsigned long aisle;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFB11998772553210;
  errors += items_test_helper(rotate_items_right, &aisle, 0, 0, 0xFB11998772553210);
  aisle = 0xFB11998772553210;
  errors += items_test_helper(rotate_items_right, &aisle, 0, 10, 0xFB11998772553210);

  aisle = 0xFB11998772553210;
  errors += items_test_helper(rotate_items_right, &aisle, 1, 1, 0xFB119987732A3210);
  aisle = 0xFB119987732A3210;
  errors += items_test_helper(rotate_items_right, &aisle, 2, 9, 0xFB119B0E732A3210);
  aisle = 0xFB119B0E732A3210;
  errors += items_test_helper(rotate_items_right, &aisle, 3, 3, 0xF8E29B0E732A3210);
  aisle = 0xF8E29B0E732A3210;
  errors += items_test_helper(rotate_items_right, &aisle, 3, 14, 0xF88E9B0E732A3210);

  errors += items_random_tests(rotate_items_right, rotate_items_right_sol);

  return errors == 0;
}

int toggle_test_helper(items_funct items_function, unsigned long* aisle, int index, int spaces_index, unsigned long expected_after) {
  unsigned long before_aisle = *aisle;

  items_function(aisle, index, spaces_index);

  if (*aisle != expected_after) {
    printf("\t\terror: index=%d, spaces_index=%d\n", index, spaces_index);

    printf("\t\t    before *aisle=0x%016lx -- after *aisle=0x%016lx, expected *aisle=0x%016lx\n", before_aisle, *aisle, expected_after);
    return -1;
  }
  return 0;
}

int toggle_random_tests(items_funct items_function, items_funct items_solution) {
  if (!grade_output) printf("\tTesting a variety of cases (only shows first failure)...\n");

  for (int i = 0; i < 500; i++) {
    int index = rand() % 4;
    int spaces_index = rand() % 10;
    unsigned long aisle = rand();
    aisle = (aisle << 32) | rand();
    unsigned long aisle_sol = aisle;

    items_solution(&aisle_sol, index, spaces_index);

    if (toggle_test_helper(items_function, &aisle, index, spaces_index, aisle_sol) != 0) {
      return -1;
    }
  }

  return 0;
}

static int toggle_space_tests() {
  unsigned long aisle;
  int score = 0;
  int errors = 0;

  if (!grade_output) printf("\tTesting specific cases...\n");
  aisle = 0xFEDCBA9876543210;
  errors += toggle_test_helper(toggle_space, &aisle, 0, 0, 0xFEDCBA9876543211);
  aisle = 0xFEDCBA9876543211;
  errors += toggle_test_helper(toggle_space, &aisle, 1, 9, 0xFEDCBA9874543211);
  aisle = 0xFEDCBA9874543211;
  errors += toggle_test_helper(toggle_space, &aisle, 2, 6, 0xFEDCBAD874543211);
  aisle = 0xFEDCBAD874543211;
  errors += toggle_test_helper(toggle_space, &aisle, 3, 3, 0xFED4BAD874543211);

  if (errors == 0) score++;

  errors += toggle_random_tests(toggle_space, toggle_space_sol);

  if (errors == 0) score++;

  return score;
}

static test_info aisle_tests[] = {
  {"get_section", get_section_tests, 0, 1},
  {"get_spaces", get_spaces_tests, 0, 1},
  {"get_id", get_id_tests, 0, 1},
  {"set_section", set_section_tests, 0, 2},
  {"set_spaces", set_spaces_tests, 0, 2},
  {"set_id", set_id_tests, 0, 2},
  {"toggle_space", toggle_space_tests, 0, 2},
  {"num_items", num_items_tests, 0, 3},
  {"add_items", add_items_tests, 0, 3},
  {"remove_items", remove_items_tests, 0, 1},
  {"rotate_items_left", rotate_items_left_tests, 0, 3},
  {"rotate_items_right", rotate_items_right_tests, 0, 1},
};

// Run all tests
void run_tests() {
  int total = 0, expected_total = 0;

  if (!grade_output) {
		printf("Running aisle_manager tests...\n\n");
    printf("-------------------------------------------------\n");
	} else {
		printf("[\n");
	}

  for (int i = 0; i < NUM_AISLE_TESTS; i++) {
    test_info* test_info = &aisle_tests[i];
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
				if (i != NUM_AISLE_TESTS - 1) printf(",");
				printf("\n");
			}
    }
  }

  if (expected_total == 0) {
    printf("ERROR: Bad function name given!\n");
  }

  if (!grade_output) {
    printf("Finished testing aisle_manager!\n");
    printf("Summary:\n");
    for (int i = 0; i < NUM_AISLE_TESTS; i++) {
      test_info* test_info = &aisle_tests[i];
      if (!test_fname || strcmp(test_info->fname, test_fname) == 0) {
        printf("\t%-16s\t%d/%d\n", test_info->fname, test_info->score, test_info->points);
      }
    }

		printf("\nTotal Score: %d/%d\n", total, expected_total);
	} else {
		printf("]\n");
	}
}

// Display usage info
static void usage() {
  printf("Usage: aisle-test [-f <name>]\n");
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
