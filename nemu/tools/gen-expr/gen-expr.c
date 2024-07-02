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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static pos = 0;

static void put_in_buf(char *string) {
  strcpy(buf + pos, string);
  pos += strlen(string);
}

static void gen_rand_op() {
  int choose = rand() % 4;
  switch (choose) {
  case 0:
    put_in_buf("+");
    break;
  case 1:
    put_in_buf("-");
    break;
  case 2:
    put_in_buf("*");
    break;
  case 3:
    put_in_buf("/");
    break;
  default:
    break;
  }
}
static void gen_rand_value() {
  int choose = rand() % 2;
  char buffer[64];
  switch (choose) {
    case 0:
      sprintf(buffer, "%d", rand());
      put_in_buf(buffer);
      break;
    case 1:
      sprintf(buffer, "0x%x", rand());
      put_in_buf(buffer);
      break;
    default:
      break;
  }
}

static void gen_rand_expr() {
  int choose = rand() % 3;
  switch (choose) {
  case 0:
    gen_rand_value();
    break;
  case 1:
    put_in_buf("(");
    gen_rand_expr();
    put_in_buf(")");
    break;
  case 2:
    gen_rand_expr();
    gen_rand_op();
    gen_rand_expr();
    break;
  default:
    break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
