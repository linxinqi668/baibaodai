#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	struct watchpoint *pred; // 指向前一个节点

	/* TODO: Add more members if necessary */
	char * expr; // 保存表达式
	uint32_t now_value; // 保存当前的值
	uint32_t old_value;

	bool status; // 0表示空闲

} WP;

void init_wp_pool();
WP* new_wp();
void free_wp(WP*);
WP* get_head();
WP* get_pool();
int get_num();

#endif
