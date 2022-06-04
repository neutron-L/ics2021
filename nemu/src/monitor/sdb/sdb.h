#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);
word_t vaddr_read(vaddr_t addr, int len);

void show_watchpoints();
bool check_watchpoints();
int new_wp(const char * str);
int free_wp(int NO);
#endif
