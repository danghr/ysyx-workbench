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
#include <unistd.h>
#include <stdbool.h>

// this should be enough
static char buf[65536];
static char buf_for_output[65536];
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static int pos = 0;
static int pos_output = 0;

static void put_in_buf(char *string) {
  strcpy(buf + pos, string);
  pos += strlen(string);
}
static void put_in_buf_for_output(char *string) {
  strcpy(buf_for_output + pos_output, string);
  pos_output += strlen(string);
}

static void gen_rand_op() {
  int choose = rand() % 8;
  switch (choose) {
  case 0:
    put_in_buf(" + ");
    put_in_buf_for_output(" + ");
    break;
  case 1:
    put_in_buf(" - ");
    put_in_buf_for_output(" - ");
    break;
  case 2:
    put_in_buf(" * ");
    put_in_buf_for_output(" * ");
    break;
  case 3:
    put_in_buf(" / ");
    put_in_buf_for_output(" / ");
    break;
  case 4:
    put_in_buf(" && ");
    put_in_buf_for_output(" && ");
    break;
  case 5:
    put_in_buf(" || ");
    put_in_buf_for_output(" || ");
    break;
  case 6:
    put_in_buf(" == ");
    put_in_buf_for_output(" == ");
    break;
  case 7:
    put_in_buf(" != ");
    put_in_buf_for_output(" != ");
    break;
  default:
    break;
  }
}

static void gen_rand_value() {
  int choose = rand() % 2;
  char buffer[64];
  uint32_t value = rand() % (2 << 10);
  if (rand() % 2) {
    // Negative
    put_in_buf("-");
    put_in_buf_for_output("-");
  }
  switch (choose) {
    case 0:
      sprintf(buffer, "%du", value);
      put_in_buf(buffer);
      sprintf(buffer, "%d", value);
      put_in_buf_for_output(buffer);
      break;
    case 1:
      sprintf(buffer, "0x%xu", value);
      put_in_buf(buffer);
      sprintf(buffer, "0x%x", value);
      put_in_buf_for_output(buffer);
      break;
    default:
      break;
  }
}

static void gen_rand_expr() {
  // Avoid being too long
  if (pos > 65536 / 512) return ;
  if (pos_output > 65536 / 512) return ;
  if (rand() % 2) {
    put_in_buf(" ");
    put_in_buf_for_output(" ");
  }
  int choose = rand() % 3;
  switch (choose) {
  case 0:
    gen_rand_value();
    break;
  case 1:
    put_in_buf("(");
    put_in_buf_for_output("(");
    gen_rand_expr();
    put_in_buf(")");
    put_in_buf_for_output(")");
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
    pos = 0;
    pos_output = 0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    // Use '-Wdiv-by-zero' to avoid division by 0
    int ret = system("gcc /tmp/.code.c -Wdiv-by-zero -o /tmp/.expr > /tmp/.expr.compile 2>&1");
    // Try again if the compilation fails
    if (ret != 0) { i--; continue; }

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf_for_output);
  }
  return 0;
}
