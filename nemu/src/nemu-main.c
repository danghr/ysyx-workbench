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
  char *nemu_home = getenv("NEMU_HOME");
  char expr_file[1024];
  strcpy(expr_file, nemu_home);
  strcat(expr_file, "/tools/gen-expr/expr.txt");
  FILE *fp = fopen(expr_file, "r");
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
    char *ref_val_str = strtok(line, " ");
#ifdef CONFIG_ISA64
    word_t ref_val = strtoull(ref_val_str, NULL, 0);
    printf("Reference value: %lu\n", ref_val);
#else
    word_t ref_val = strtoul(ref_val_str, NULL, 0);
    printf("Reference value: %u\n", ref_val);
#endif
    char *op = strtok(NULL, "\n");
    bool success = false;
    printf("Expression: %s\n", op);
    word_t result = expr(op, &success);
    if (success) {
      printf("Result: %u\n", result);
      if (result == ref_val) {
        printf("Result correct!\n");
      } else {
        printf("Result incorrect!\n");
        assert(0);
      }
    } else {
      printf("Expression invalid!\n");
      assert(0);
    }
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
