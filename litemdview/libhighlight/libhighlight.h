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

#ifndef _HL_LIBHIGHLIGHT_H
#define _HL_LIBHIGHLIGHT_H

#include <stdint.h>
#include <stddef.h>
#include <config.h>

#ifndef CONFIG_USE_HTML
#define CONFIG_USE_HTML 1
#endif

enum {
  HL_NODE_SPACE = 1,
  HL_NODE_NEWLINE,
  HL_NODE_TEXT,
  HL_NODE_SCOPE_START,
  HL_NODE_SCOPE_END,
  HL_NODE_QUOTE,
  HL_NODE_SYMBOL,
  HL_NODE_FUNCTION,
  HL_NODE_FUNCTION_CALL,
  HL_NODE_EXPR,
  HL_NODE_TYPE,
  HL_NODE_DECL,
  HL_NODE_DEFINITION,
  HL_NODE_COMMENT_START,
  HL_NODE_COMMENT_END,
  HL_NODE_MCOMMENT_START,
  HL_NODE_MCOMMENT_END,
  HL_NODE_CHCOMMENT,
  HL_NODE_NUMBER
};

typedef struct hl_node {
  uint8_t type;
  char *text;
  int text_len;
  struct hl_node *children;
  struct hl_node *parent;
  struct hl_node *root;
} hl_node;

typedef struct hl_root {
  hl_node *node;
  size_t size;
  size_t text_size;
} hl_root;

typedef struct hl_ctx {
  uint8_t last;
  uint8_t lock;
  uint8_t sh;
} hl_ctx;

hl_root * hl_parser(uint8_t *buffer, size_t len,
    void (*user_next_word)(char *, int, hl_node *, hl_ctx *ctx));

hl_root * hl_parser_file(
    int fd, void (*user_next_word)(char *, int, hl_node *, hl_ctx *));

void hl_next_word(char *word, int len, hl_node *node, hl_ctx *ctx);

hl_node * hl_node_create();
hl_node * hl_node_insert(hl_node * node);
hl_node * hl_node_at(hl_node * node, int at);

void hl_node_free(hl_node * root);
void hl_root_free(hl_root *root);

#ifdef CONFIG_USE_HTML
  char * hl_compile_html(hl_root *root);
#endif

char * hl_compile_term(hl_root *root);

void hl_compile_pp(hl_node *node);

int hl_keyword_expr(char *word, int len);
int hl_keyword_type(char *word, int len);
int hl_keyword_decl(char *word, int len);
int hl_keyword_is_number(char *str, int len);

#endif

