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
static int total_wp_count = 0;

void init_wp_pool() {
#ifdef CONFIG_WATCHPOINT
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = 0;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
#endif
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(const char *expression) {
#ifdef CONFIG_WATCHPOINT
  if (free_ == NULL) {
    printf("No enough watchpoints.\n");
    return NULL;
  }
  if (strlen(expression) >= 32) {
    printf("Expression '%s' too long.\n", expression);
    return NULL;
  }
  bool success = false;
  word_t value = expr((char *)expression, &success);
  if (!success) {
    printf("Invalid expression '%s'.\n", expression);
    return NULL;
  }
  WP *wp = free_;
  free_ = free_->next;
  wp->next = head;
  wp->NO = ++total_wp_count;
  strcpy(wp->str, expression);
  wp->value = value;
  printf("Watchpoint %d set. Initial value of '%s' is " FMT_WORD "\n", wp->NO, wp->str, wp->value);
  head = wp;
  return wp;
#else
  printf("Watchpoint not enabled. Recompile NEMU with config `watchpoint' enabled in menuconfig.\n");
  return NULL;
#endif
}

void free_wp(int number) {
#ifdef CONFIG_WATCHPOINT
  if (number > total_wp_count) {
    printf("Watchpoint %d does not exist.\n", number);
    return;
  }
  WP *wp;
  for (wp = head; wp != NULL; wp = wp->next) {
    if (wp->NO == number) {
      break;
    }
  }
  if (wp == NULL) {
    printf("Watchpoint %d does not exist.\n", number);
    return;
  }

  if (head == wp) {
    head = wp->next;
  } else {
    for (WP *p = head; p != NULL; p = p->next) {
      if (p->next == wp) {
        p->next = wp->next;
        break;
      }
    }
  }
  wp->NO = 0;
  wp->next = free_;
  free_ = wp;
#else
  printf("Watchpoint not enabled. Recompile NEMU with config `watchpoint' enabled in menuconfig.\n");
#endif
}

bool watchpoint_check() {
#ifdef CONFIG_WATCHPOINT
  bool ret = false;
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    bool success = false;
    word_t val = expr(wp->str, &success);
    if (!success) {
      printf("Invalid expression '%s'.\n", wp->str);
      assert(0);  // Should not reach as we have checked the expression when adding watchpoint
    }
    if (val != wp->value) {
      printf("Watchpoint %d: Value of '%s' changed from " FMT_WORD " to " FMT_WORD "\n", wp->NO, wp->str, wp->value, val);
      wp->value = val;
      ret = true;
    }
  }
  return ret;
#else
  printf("Watchpoint not enabled. Recompile NEMU with config `watchpoint' enabled in menuconfig.\n");
  return false;
#endif
}

void print_wp() {
#ifdef CONFIG_WATCHPOINT
  printf("Num   Expression\n");
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    printf("%-5d %s\n", wp->NO, wp->str);
  }
#else
  printf("Watchpoint not enabled. Recompile NEMU with config `watchpoint' enabled in menuconfig.\n");
#endif
}
