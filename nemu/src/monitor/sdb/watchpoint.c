/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"
#include <cpu/watchpoint_check.h>

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(const char *expression) {
  WP *wp = free_;
  if (wp == NULL) {
    printf("No enough watchpoints.\n");
    return NULL;
  }
  free_ = free_->next;
  wp->next = head;
  if (strlen(expression) >= 32) {
    printf("Expression '%s' too long.\n", expression);
    return NULL;
  }
  bool success = false;
  expr((char *)expression, &success);
  if (!success) {
    printf("Invalid expression '%s'.\n", expression);
    return NULL;
  }
  strcpy(wp->str, expression);
  head = wp;
  return wp;
}

void free_wp(WP *wp) {
  WP *p;
  if (head == wp) {
    head = wp->next;
  } else {
    for (p = head; p != NULL; p = p->next) {
      if (p->next == wp) {
        p->next = wp->next;
        break;
      }
    }
  }
  wp->next = free_;
  free_ = wp;
}

bool watchpoint_check() {
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    bool success = true;
    word_t val = expr(wp->str, &success);
    if (!success) {
      printf("Invalid expression '%s'.\n", wp->str);
      assert(0);  // Should not reach as we have checked the expression when adding watchpoint
    }
    if (val) {
      printf("Hit watchpoint %d: '%s' = " FMT_WORD "\n", wp->NO, wp->str, val);
      return true;
    }
  }
  return false;
}
