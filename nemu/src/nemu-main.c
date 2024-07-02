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
#include <stdio.h>
#include <stdlib.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

extern word_t expr(char *e, bool *success);

void check_expr(int argc, char *argv[]) {
  FILE *fp = fopen("$(NEMU_HOME)/tools/gen-expr/expr.txt", "r");
  assert(fp != NULL);

  // While loop, read a line from the file, 
  // extract the first number as the reference number,
  // then call expr() to evaluate the expression,
  // and finally compare the value returned by expr() with the reference number.
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, fp)) != -1) {
    printf("Retrieved line of length %zu:\n", read);
    printf("%s", line);
  }
  if (line)
    free(line);
  

  fclose(fp);
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
