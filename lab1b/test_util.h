// Written by Porter Jones (pbjones@cs.washington.edu)
// inspired by the testing structure from CMU's bitwise operator lab.

#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

// Various definitions to help test functionality

// generic test function type
typedef int (*test_funct) ();

// Function types for running aisle_manager tests
typedef unsigned short (*get_funct) (unsigned long*, int);
typedef void (*set_funct)(unsigned long*, int, unsigned short);
typedef void (*items_funct)(unsigned long*, int, int);

// Combine all the information about a test
typedef struct {
    char* fname;
    test_funct run_tests;
    int score;
    int points;
} test_info;

// for aisle_test.c
unsigned short get_section_sol(unsigned long* aisle, int index);
unsigned short get_id_sol(unsigned long* aisle, int index);
unsigned short get_spaces_sol(unsigned long* aisle, int index);
void set_section_sol(unsigned long* aisle, int index, unsigned short new_section);
void set_id_sol(unsigned long* aisle, int index, unsigned short new_id);
void set_spaces_sol(unsigned long* aisle, int index, unsigned short new_spaces);
unsigned short num_items_sol(unsigned long* aisle, int index);
void add_items_sol(unsigned long* aisle, int index, int n);
void remove_items_sol(unsigned long* aisle, int index, int n);
void toggle_space_sol(unsigned long* aisle, int section_index, int space_index);
void rotate_items_left_sol(unsigned long* aisle, int index, int n);
void rotate_items_right_sol(unsigned long* aisle, int index, int n);

#endif // _TEST_UTIL_H
