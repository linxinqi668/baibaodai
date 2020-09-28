#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 100

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i; // 监视点编号
		wp_pool[i].next = &wp_pool[i + 1]; // 指向下一个监视点的指针
		wp_pool[i].pred = &wp_pool[i - 1]; // 指向前一个监视点的指针
		wp_pool[i].now_value = 0;
		wp_pool[i].old_value = 0;
		wp_pool[i].expr = NULL;
		wp_pool[i].status = false; // 设置为空闲
	}
	wp_pool[NR_WP - 1].next = NULL;
	wp_pool[0].pred = NULL;

	head = NULL;
	free_ = wp_pool; // 整个池都是空闲监视点
}

/* TODO: Implement the functionality of watchpoint */

// 返回一个空闲监视点， 空闲监视点为新的占用监视点链表表头
WP* new_wp() {
	// 确保仍然有空闲监视点
	Assert(free_ != NULL, "无剩余空闲监视点.");

	WP* res;

	if (head == NULL) { // 若当前忙碌节点为空
		head = free_;
		free_ = free_->next;
		if (free_ != NULL) // 如果空闲链表不止一个节点
			free_->pred = NULL;
		head->next = NULL;
		head->pred = NULL;
		res = head;
	} else { // 若当前忙碌节点非空, 则返回的新节点作为表头
		WP* backup = free_;
		free_ = free_->next;
		if (free_ != NULL) // 如果空闲链表不止一个节点
			free_->pred = NULL;
		head->pred = backup;
		backup->next = head;
		head = backup;
		head->pred = NULL;
		res = head;
	}

	// 设置为忙碌
	res->status = true;

	return res;
}

// 归还空闲监视点
void free_wp(WP* wp) {

	// 确保传入的不是空指针
	Assert(wp != NULL, "free空指针\n");

	// 设置为空闲
	wp->status = false;

	// 首先将wp指向的节点退出忙碌链表
	if (wp == head) { // 当wp指向头结点时
		head = NULL;
	} else if (wp->next == NULL) { // 当wp指向最后一个节点时
		wp->pred->next = NULL;
	} else { // 当wp指向中间节点时
		wp->pred->next = wp->next;
		wp->next->pred = wp->pred;
	}

	// 然后将wp指向的节点插入空闲链表并且作为头结点
	if (free_ == NULL) { // 若当前空闲链表为空
		free_ = wp;
		free_->pred = NULL;
		free_->next = NULL;
	} else { // 若空闲链表非空
		wp->next = free_;
		free_->pred = wp;
		free_ = wp;
		free_->pred = NULL;
	}

	// 完成
}

WP* get_head() {
	return head;
}

WP* get_pool() {
	return wp_pool;
}

