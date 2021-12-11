// Written by Porter Jones (pbjones@cs.washington.edu)

#ifndef _STORE_CLIENT_H
#define _STORE_CLIENT_H

extern unsigned long aisles[];
extern int stockroom[];

// Function definitions for the store client
void refill_from_stockroom();
int fulfill_order(unsigned short id, int num);
unsigned short* empty_section_with_id(unsigned short id);
unsigned short* section_with_most_items();
void refill_from_stockroom_sol();
int fulfill_order_sol(unsigned short id, int num);
unsigned short* empty_section_with_id_sol(unsigned short id);
unsigned short* section_with_most_items_sol();

#endif // _STORE_CLIENT_H
