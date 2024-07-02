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

// #define CHECK_EXPR
#define CHECK_EXPR_DEREF_REG

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

extern word_t expr(char *e, bool *success);

#ifdef CHECK_EXPR
void check_expr(int argc, char *argv[]) {
  char *nemu_home = getenv("NEMU_HOME");
  char expr_file[1024];
  strcpy(expr_file, nemu_home);
  strcat(expr_file, "/tools/gen-expr/expr.txt");
  FILE *fp = fopen(expr_file, "r");
  assert(fp != NULL);

  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int cnt = 0;
  while ((read = getline(&line, &len, fp)) != -1) {
    printf("Testing line %d\n", ++cnt);
    printf("Retrieved line of length %zu:\n", read);
    printf("%s", line);
    char *ref_val_str = strtok(line, " ");
#ifdef CONFIG_ISA64
    word_t ref_val = strtoull(ref_val_str, NULL, 0);
#else
    word_t ref_val = strtoul(ref_val_str, NULL, 0);
#endif
    char *op = strtok(NULL, "\n");
    bool success = false;
    printf("Expression: \"%s\"\n", op);
#ifdef CONFIG_ISA64
    printf("Reference value: %lu\n", ref_val);
#else
    printf("Reference value: %u\n", ref_val);
#endif
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
    printf("\n");
  }
  if (line)
    free(line);

  fclose(fp);
  printf("All tests for expr passed!\n");
  return;
}
#elif defined(CHECK_EXPR_DEREF_REG)
void check_expr(int argc, char *argv[]) {
  // Manually write some test cases for expr
  // to check implementation of memory and 
  // register dereference
}
#endif

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

#if defined(CHECK_EXPR) || defined(CHECK_EXPR_DEREF_REG)
  check_expr(argc, argv);
  return 0;
#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
