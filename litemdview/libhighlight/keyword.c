/*
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  Author: g0tsu
 *  Email:  g0tsu at dnmx.0rg
 */

#include <libhighlight.h>
#include <string.h>
#include <lang/hl_c.h>
#include <lang/hl_sh.h>
#include <lang/hl_java.h>

static int hl_cmp(char *a, const char *b, int len) {
  return (strncmp(a, b, len) == 0);
}

static int hl_isspecial(int c) {
  switch (c) {
    case ',':
    case ';':
    case '[':
    case ']':
    case '(':
    case ')':
    case '-':
    case '.':
    case '\0':
    case ' ':
      return 1;
    default:
      break;
  }

  return 0;
}

int hl_keyword_is_number(char *str, int len) {
  for (int i = 0; i < len; i++) {
    if (hl_isspecial(str[i]))
      continue;

    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }

  return 1;
}

int hl_keyword_expr(char *word, int len) {
  for (int i = 0; i < HL_C_EXPR_SIZ; i++) {
    if (hl_cmp(word, HL_C_EXPR[i], len))
      return 1;
  }

  for (int i = 0; i < HL_SH_EXPR_SIZ; i++) {
    if (hl_cmp(word, HL_SH_EXPR[i], len))
      return 1;
  }

  for (int i = 0; i < HL_JAVA_EXPR_SIZ; i++) {
    if (hl_cmp(word, HL_JAVA_EXPR[i], len))
      return 1;
  }

  return 0;
}

int hl_keyword_type(char *word, int len) {
  for (int i = 0; i < HL_C_TYPE_SIZ; i++) {
    if (hl_cmp(word, HL_C_TYPE[i], len))
      return 1;
  }

  for (int i = 0; i < HL_SH_TYPE_SIZ; i++) {
    if (hl_cmp(word, HL_SH_TYPE[i], len))
      return 1;
  }

  for (int i = 0; i < HL_JAVA_TYPE_SIZ; i++) {
    if (hl_cmp(word, HL_JAVA_TYPE[i], len))
      return 1;
  }

  return 0;
}

int hl_keyword_decl(char *word, int len) {
  for (int i = 0; i < HL_C_DECL_SIZ; i++) {
    if (hl_cmp(word, HL_C_DECL[i], len))
      return 1;
  }

  for (int i = 0; i < HL_SH_DECL_SIZ; i++) {
    if (hl_cmp(word, HL_SH_DECL[i], len))
      return 1;
  }

  for (int i = 0; i < HL_JAVA_DECL_SIZ; i++) {
    if (hl_cmp(word, HL_JAVA_DECL[i], len))
      return 1;
  }

  return 0;
}

