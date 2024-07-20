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

#include <isa.h>
#include <cpu/cpu.h>
#include <memory/paddr.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
#ifdef CONFIG_WATCHPOINT
void init_wp_pool();
#endif

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return 0;
}

static int cmd_help(char *args);

static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
#ifdef CONFIG_WATCHPOINT
static int cmd_w(char *args);
static int cmd_d(char *args);
#endif

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Continue the execution for N instructions. Format: `si [N]'. N=1 if not specified", cmd_si },
  { "info", "Print the program status. `info r' prints register status, and `info w' prints watchpoint status", cmd_info },
  { "x", "Scan memory. Format: `x N EXPR'. Print 4*N bytes of memory starting from the value of EXPR", cmd_x },
#ifdef CONFIG_WATCHPOINT
  { "w", "Add a watchpoint. Format: `w EXPR'. The program will stop when the value of EXPR changes", cmd_w },
  { "d", "Delete a watchpoint. Format: `d N'. N is the number of the watchpoint to be deleted", cmd_d },
#endif
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  // Extract the first argument
  // Note that since we are parsing the same argument command,
  // the strtok function should be called with NULL
  // See `man 3 strtok'
  char *arg = strtok(NULL, " ");
  uint64_t steps = 1;
  if (arg != NULL) {
    // Check whether the parameter is a valid number
    char **endptr = malloc(sizeof(char*));
    steps = strtoull(arg, /* String to be parsed */ 
                     endptr, /* Address of the first invalid character */
                     10 /* Base, force decimal */
                    );
    if (!(*arg != '\0' && **endptr == '\0')) {  // Refer to `man 3 stroull'
      printf("si: Invalid argument '%s' for si\n", arg);
      free(endptr);
      return 1;
    }
    free(endptr);
  }
  // PRId64: macro for printing int64_t
  printf("Executing %" PRId64 " instruction(s)\n", steps);

  // Call the execution function
  cpu_exec(steps);
  return 0;
}

static int cmd_info(char *args) {
  // Extract the first argument
  char *arg = strtok(NULL, " ");

  // Check whether the parameter is leagal
  if (arg == NULL) {
    printf("info: Missing argument\n");
    return 1;
  }
  char *more_arg = strtok(NULL, " ");
  if (more_arg != NULL) {
    printf("info: Too many arguments\n");
    return 1;
  }

  if (strcmp(arg, "r") == 0) {
    // Print the register status
    printf("Register status\n");
    isa_reg_display();
  } 
#ifdef CONFIG_WATCHPOINT
  else if (strcmp(arg, "w") == 0) {
    // Print the watchpoint status
    printf("Watchpoint status\n");
    print_wp();
  }
#endif
  else {
    printf("info: Invalid argument '%s'\n", arg);
#ifndef CONFIG_WATCHPOINT
    if (strcmp(arg, "w") == 0)
      printf("Watchpoint not enabled. Recompile NEMU with config `watchpoint' enabled in menuconfig if you need.\n");
#endif
    return 1;
  }
  return 0;
}

static int cmd_x(char *args) {
  // Extract the two arguments and check their validity
  char *arg_len = strtok(NULL, " ");
  char *arg_expr = strtok(NULL, "\0");  // Parse until the end of the string
  if (arg_expr == NULL) {
    printf("x: Missing argument\n");
    return 1;
  }
  if (strtok(NULL, "\0") != NULL) {
    printf("x: Too many arguments\n");
    return 1;
  }

  // Parse the length
  int len = atoi(arg_len);
  if (len <= 0) {
    printf("x: Invalid argument '%s'\n", arg_len);
    return 1;
  }

  // Parse the address
  // TODO: Change to value of expressions
  paddr_t addr = strtoull(arg_expr, NULL, 16 /* Base, force hexadecimal */);
  if (!in_pmem(addr)) {
    printf("x: Invalid address " FMT_WORD ". It is out of physical memory. \n", addr);
    return 1;
  }
  printf("Scanning memory from address " FMT_WORD " for %d bytes\n", addr, len);

  // Allocate a buffer for the scanned value
  uint8_t *buffer = malloc(sizeof(char) * len);

  // Read the value from the memory byte by byte
  // Note that the memory is little-endian
  // so we place the bytes reversely
  // to print the result in human-readable order
  for(int i = 0; i < len; i++)
    buffer[len - i - 1] = paddr_read(addr + i, 1);

  // Print the scanned value byte by byte
  for (int i = 0; i < len; i++)
    printf("%02x ", buffer[i]);
  printf("    ");
  for (int i = 0; i < len; i++)
    printf("%c", (char)buffer[i]);
  printf("\n");
  
  free(buffer);
  return 0;
}

#ifdef CONFIG_WATCHPOINT
static int cmd_w(char *args) {
  // Extract the first argument
  char *arg = strtok(NULL, "\0");

  // Check whether the parameter is leagal
  if (arg == NULL) {
    printf("w: Missing argument\n");
    return 1;
  }
  if (strtok(NULL, "\0") != NULL) {
    printf("w: Too many arguments\n");
    return 1;
  }

  // Add the watchpoint
  WP *wp = new_wp(arg);
  if (wp == NULL) {
    printf("w: Failed to add watchpoint\n");
    return 1;
  }
  printf("w: Added watchpoint %d: '%s'\n", wp->NO, wp->str);
  return 0;
}

static int cmd_d(char *args) {
  // Extract the first argument
  char *arg = strtok(NULL, " ");

  // Check whether the parameter is leagal
  if (arg == NULL) {
    printf("d: Missing argument\n");
    return 1;
  }
  char *more_arg = strtok(NULL, " ");
  if (more_arg != NULL) {
    printf("d: Too many arguments\n");
    return 1;
  }

  // Parse the number
  int number = atoi(arg);
  if (number <= 0) {
    printf("d: Invalid argument '%s'\n", arg);
    return 1;
  }

  // Delete the watchpoint
  free_wp(number);
  return 0;
}
#endif

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) {
      printf("Unknown command '%s'\n", cmd);
#ifndef CONFIG_WATCHPOINT
      if (strcmp(cmd, "w") == 0 || strcmp(cmd, "d") == 0)
        printf("Watchpoint not enabled. Recompile NEMU with config `watchpoint' enabled in menuconfig if you need.\n");
#endif
    }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
#ifdef CONFIG_WATCHPOINT
  init_wp_pool();
#endif
}
