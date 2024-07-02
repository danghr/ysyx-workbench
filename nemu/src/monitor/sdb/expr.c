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
#include <common.h>
#include <memory/paddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  // Starting from 256 avoids conflict with ASCII code
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NUMBER, // Numbers
  TK_HEX, // Hexadecimal numbers
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {"[ \t]+", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus / negative number
  {"\\*", '*'},         // multiply / pointer dereference
  {"\\/", '/'},         // divide
  {"\\(", '('},         // left parenthesis
  {"\\)", ')'},         // right parenthesis
  {"0x[0-9a-fA-F]+", TK_HEX}, // hexadecimal numbers
  {"[0-9]+", TK_NUMBER}, // numbers
  {"==", TK_EQ},        // equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

#define EXPR_C_MAX_TOKENS 64
static Token tokens[EXPR_C_MAX_TOKENS] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  for (int i = 0; i < EXPR_C_MAX_TOKENS; i++) {
    tokens[i].type = -1;
    tokens[i].str[0] = '\0';
  }

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        // The `tokens' array only supports EXPR_C_MAX_TOKENS elements
        if(nr_token >= EXPR_C_MAX_TOKENS) {
          printf("Too many tokens\n");
          nr_token = 0;
          return false;
        }

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;  // Do nothing, drop spaces

          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
            assert(substr_len == 1);  // Should only matche one character
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
          
          case TK_HEX:
          case TK_NUMBER:
            // The length of the number should be less than the buffer
            assert(substr_len < 32);
            tokens[nr_token].type = rules[i].token_type;

            // Copy the number string to the token and record the string
            // for further conversion
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            // Note that strncpy does not automatically add null character
            // at the end of the string, so we need to manually do that
            tokens[nr_token].str[substr_len] = '\0';
            
            nr_token++;
            break;
          default: assert(0);   // Should not be reached
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool eval(int p, int q, int64_t *ret) {
  Log("Evaluating tokens from %d to %d", p, q);
  // Function called by program.
  // Assertion is fine as these conditions should never be reached.
  // nr_token has already been checked by function `expr'. 
  assert(q < nr_token);
  assert(p <= q);

  if (p == q) {
    // Single token, should only be a number
    // Convert the string to number
    if (tokens[p].type != TK_NUMBER && tokens[p].type != TK_HEX) {
      printf("Invalid expression. Token %s is not a number.\n", tokens[p].str);
      return false;
    }
    char *endptr;
#ifdef CONFIG_ISA64
    *ret = (int64_t)strtoll(tokens[p].str, &endptr, 0);
#else
    *ret = (int64_t)strtol(tokens[p].str, &endptr, 0);
#endif
    assert(*endptr == '\0');
    Log("Returning value of tokens from %d to %d is %ld", p, q, *ret);
    return true;
  }

  /*********************************
   *** Step 1: Check parentheses ***
   *********************************/
  // Check if the expression is surrounded by parentheses
  int parentheses_cnt = 0;
  bool surrounded_by_parentheses = true;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') parentheses_cnt++;
    if (tokens[i].type == ')') parentheses_cnt--;
    if (parentheses_cnt < 0) {
      printf("Invalid expression. Mismatched parentheses, more ')' than '('.\n");
      return false;
    }
    if (i > p && i < q && parentheses_cnt == 0) {
      // Avoid situations like (...)+(...)
      surrounded_by_parentheses = false;
      break;
    }
  }
  if (parentheses_cnt != 0) {
    printf("Invalid expression. Mismatched parentheses. %d more '(' than ')'.\n",
      parentheses_cnt);
    return false;
  }

  // Check if the expression is surrounded by parentheses
  if (tokens[p].type == '(' && tokens[q].type == ')' && surrounded_by_parentheses) {
    return eval(p + 1, q - 1, ret);
  }

  /************************************
   *** Step 2: Find major operators ***
   ************************************/
  // Find the major operator
  // i.e., the operator with the least prirority
  // as it needs to be computed last
  int major_op = -1;
  int in_parentheses = 0;
  for (int i = p; i <= q; i++) {
    // Skip all expressions in parentheses
    if (tokens[i].type == '(') in_parentheses++;
    if (tokens[i].type == ')') in_parentheses--;
    if (in_parentheses > 0 || (tokens[i].type == ')' && in_parentheses == 0))
      continue;

    // Skip all numbers
    if (tokens[i].type == TK_NUMBER || tokens[i].type == TK_HEX)
      continue;

    // + and - are the lowest priority operators
    // We need to record the last one among them
    if (tokens[i].type == '+' || tokens[i].type == '-') {
      // Check whether it is a negative number
      // A negative number should be the first token
      // or the token after a left parenthesis
      // or the token after an operato
      if (i == p ||
          tokens[i - 1].type == '(' ||
          tokens[i - 1].type == '+' ||
          tokens[i - 1].type == '-' ||
          tokens[i - 1].type == '*' ||
          tokens[i - 1].type == '/'
      ) {
        // Only negative symbol is allowed
        if (tokens[i].type != '-') {
          printf(
            "Invalid expression. Unknown operator type %d ('%s') at location %d. It should be '-' for negative numbers.\n",
            tokens[i].type, (char *)(&tokens[i].type), i
          );
          return false;
        }
        Log("Negative symbol found at location %d", i);
        // Record only when there is no major operator
        if (major_op == -1) {
          // Negative numbers should be the first token in this situation
          assert(i == p);
          major_op = i;
        }
        continue;
      }
      // Now we have a valid + or - (as minus) operator
      // It automatically overrides the previous operator
      // no matter it is +, -, * or /;
      if (major_op < i)
        major_op = i;
    } else if (tokens[i].type == '*' || tokens[i].type == '/') {
      // Check whether it is a dereference
      // Similar to negative numbers,
      // a dereference operator should be the first token
      // or the token after a left parenthesis
      // or the token after an operator
      if (i == p ||
          tokens[i - 1].type == '(' ||
          tokens[i - 1].type == '+' ||
          tokens[i - 1].type == '-' ||
          tokens[i - 1].type == '*' ||
          tokens[i - 1].type == '/'
      ) {
        // Only dereference symbol is allowed
        if (tokens[i].type != '*') {
          printf(
            "Invalid expression. Unknown operator type %d ('%s') at location %d. It should be '*' for dereference symbols.\n",
            tokens[i].type, (char *)(&tokens[i].type), i
          );
          return false;
        }
        Log("Dereference symbol found at location %d", i);
        // Record only when there is no major operator
        if (major_op == -1) {
          // Dereference symbols should be the first token in this situation
          assert(i == p);
          major_op = i;
        }
        continue;
      }
      // Now we have a valid * (as multiplication) or / operator
      // It automatically overrides the previous operator
      // if it is * or /;
      if (
        major_op == -1 || 
        (major_op < i && (tokens[major_op].type == '*' || tokens[major_op].type == '/'))
      ) major_op = i;
    } else {
      printf("Invalid expression. Unknown operator type %d ('%s') at location %d.\n",
        tokens[i].type, (char *)(&tokens[i].type), i
      );
      return false;
    }
  }

  if (major_op == -1) {
    printf("Invalid expression. No major operator found.\n");
    return false;
  }

  Log("Major operator found at %d, type '%s'", major_op, (char *)(&tokens[major_op].type));
  
  // Detect unary operators
  // According to previous code, it should be the first token
  if (major_op == p) {
    int64_t unary_value;
    if (tokens[major_op].type == '-') {
      // Negative number
      if (!eval(p + 1, q, &unary_value)) return false;
      *ret = -unary_value;
      return true;
    } else if (tokens[major_op].type == '*') {
      // Derefence
      if (!eval(p + 1, q, &unary_value)) return false;
      // Dereference the value
      // Check if the address is in the physical memory
      if (!in_pmem(unary_value)) {
        printf("Invalid expression. Dereferencing address 0x%lx is out of physical memory.\n", unary_value);
        return false;
      }
      *ret = paddr_read(unary_value, 4);
      return true;
    }
    assert(0);  // Should not be reached
  }

  // Evaluate the left and right expressions
  int64_t left, right;
  if (!eval(p, major_op - 1, &left) ||
      !eval(major_op + 1, q, &right))
    return false;
  
  // Compute the result
  switch (tokens[major_op].type) {
    case '+': *ret = left + right; break;
    case '-': *ret = left - right; break;
    case '*': *ret = left * right; break;
    case '/': *ret = left / right; break;
    default: assert(0);  // Should not be reached
  }
  Log("Value of tokens from %d to %d is %ld", p, q, *ret);
  return true;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // Now we have nr_tokens tokens
  assert(nr_token > 0);
  assert(nr_token < EXPR_C_MAX_TOKENS);

  int64_t result;
  // Use signed integer to support negative numbers
  if (!eval(0, nr_token - 1, &result)) {
    *success = false;
    return 0;
  }

  *success = true;
  return (word_t)result;
}
