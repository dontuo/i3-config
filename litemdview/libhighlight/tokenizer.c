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

static hl_node * hl_token_find_prev(hl_node *node) {
  hl_node *iter = node;

  while (iter->parent) {
    iter = iter->parent;

    if (iter->type != HL_NODE_SPACE)
      return iter;
  }

  return NULL;
}

static void hl_token_check_function(hl_node *node) {
  hl_node *prev = hl_token_find_prev(node);

  if (prev && node->type == HL_NODE_TYPE)
    node->type = HL_NODE_FUNCTION;
  else if (node->type == 0)
    node->type = HL_NODE_FUNCTION_CALL;
}

static void hl_token_single(uint8_t c, hl_node *node, hl_ctx *ctx) {
  hl_node *prev;

  if (c >= '0' && c <= '9') {
    node->type = HL_NODE_NUMBER;
    return;
  }

  switch (c) {
    /*
     * Multiline comment
     */
    case '#':
      if (ctx->last == 0 && ctx->sh == 1) {
        ctx->last = HL_NODE_COMMENT_START;
        node->type = HL_NODE_COMMENT_START;
      }
      break;
    case '*':
      if (ctx->sh == 1)
        break;

      prev = node->parent;

      if (ctx->last == 0 && prev && prev->text_len == 1 && prev->text[0] == '/') {
        prev->type = HL_NODE_MCOMMENT_START;
        ctx->last = HL_NODE_MCOMMENT_START;
      }
      break;
    /*
     * A single line comment
     */
    case '/':
      prev = hl_token_find_prev(node);

      if (ctx->last == 0 && prev && prev->text_len == 1 && prev->text[0] == '/') {
        prev->type = HL_NODE_COMMENT_START;
        ctx->last = HL_NODE_COMMENT_START;
      }

      if (ctx->last == HL_NODE_MCOMMENT_START && prev->text[0] == '*') {
        node->type = HL_NODE_MCOMMENT_END;
        ctx->last = 0;
      }
      break;
    case ' ':
    case '\t':
    case '\v':
      node->type = HL_NODE_SPACE;
      break;
    case '\n':
      node->type = HL_NODE_NEWLINE;
      prev = hl_token_find_prev(node);

      if (prev && ctx->last == HL_NODE_COMMENT_START) {
        if (prev->type == HL_NODE_COMMENT_START)
          prev->type = HL_NODE_CHCOMMENT;
        else 
          prev->type = HL_NODE_COMMENT_END;
        ctx->last = 0;
      }
      break;
    case '(':
      node->type = HL_NODE_SCOPE_START;
      hl_token_check_function(hl_token_find_prev(node));
      break;
    case ')':
      node->type = HL_NODE_SCOPE_END;
      break;
    case '>':
    case '<':
      node->type = HL_NODE_SYMBOL;
    default:
      break;
  }
}

void hl_next_word(char *word, int len, hl_node *node, hl_ctx *ctx) {
  node->text = strdup(word);
  node->text_len = len;

  if (len == 1) {
    hl_token_single(word[0], node, ctx);
    return;
  }

  if (ctx->last == HL_NODE_MCOMMENT_START || ctx->last == HL_NODE_COMMENT_START)
    return;

  if (word[0] == '"' && word[len -1] == '"') {
    node->type = HL_NODE_QUOTE;
    return;
  }

  if (word[0] == '\'' && word[len-1] == '\'') {
    node->type = HL_NODE_QUOTE;
    return;
  }

  if (hl_keyword_expr(word, len)) {
    node->type = HL_NODE_EXPR;
    return;
  }

  if (hl_keyword_type(word, len)) {
    node->type = HL_NODE_TYPE;
    return;
  }

  if (hl_keyword_decl(word, len)) {
    node->type = HL_NODE_DECL;
    return;
  }

  if (hl_keyword_is_number(word, len)) {
    node->type = HL_NODE_NUMBER;
    return;
  }

  if (word[0] == '#' && word[1] == '!') {
    ctx->last = HL_NODE_COMMENT_START;
    ctx->sh = 1;
    node->type = HL_NODE_COMMENT_START;
    return;
  }

  if (ctx->sh == 1) {

    if (word[0] == '$') {
      node->type = HL_NODE_TYPE;
      return;
    }

    if (word[0] == '#') {
      ctx->last = HL_NODE_COMMENT_START;
      node->type = HL_NODE_COMMENT_START;
      return;
    }

  } else {

    if (word[0] == '#' && hl_keyword_decl(++word, len - 1)) {
      node->type = HL_NODE_DECL;
      return;
    }
  }

}

