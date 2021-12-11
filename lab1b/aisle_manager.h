// Written by Porter Jones (pbjones@cs.washington.edu)

#ifndef _AISLE_MANAGER_H
#define _AISLE_MANAGER_H

// Function definitions for the aisle manager
unsigned short get_section(unsigned long* aisle, int index);
unsigned short get_id(unsigned long* aisle, int index);
unsigned short get_spaces(unsigned long* aisle, int index);
void set_section(unsigned long* aisle, int index, unsigned short new_section);
void set_id(unsigned long* aisle, int index, unsigned short new_id);
void set_spaces(unsigned long* aisle, int index, unsigned short new_spaces);
unsigned short num_items(unsigned long* aisle, int index);
void add_items(unsigned long* aisle, int index, int n);
void remove_items(unsigned long* aisle, int index, int n);
void toggle_space(unsigned long* aisle, int index, int space_index);
void rotate_items_left(unsigned long* aisle, int index, int n);
void rotate_items_right(unsigned long* aisle, int index, int n);

#endif // _AISLE_MANAGER_H
