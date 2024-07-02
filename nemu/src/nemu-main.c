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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

extern word_t expr(char *e, bool *success);

void check_expr(int argc, char *argv[]) {
  bool success = false;
  word_t result = expr("*(0x80000000 + (-(\t -1\n+ -3)*(1 - -1)))\n", &success);
  printf("%u\n", result);
  printf("0x%08x\n", result);
  assert(success);
  return;
}

#define CHECK_EXPR

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

#ifdef CHECK_EXPR
  check_expr(argc, argv);
  return 0;
#else
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
#endif
}
