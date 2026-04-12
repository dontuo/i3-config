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
#include <stdlib.h>

#define HL_TERM_BUFFER_SIZ 12000

#define BLACK_TEXT    "\033[30;1m"
#define RED_TEXT      "\033[38;5;175m"
#define RRED_TEXT     "\033[38;5;176m"
#define GREEN_TEXT    "\033[38;5;153m"
#define YELLOW_TEXT   "\033[38;5;209m"
#define BLUE_TEXT     "\033[38;5;139m"
#define MAGENTA_TEXT  "\033[35;1m"
#define CYAN_TEXT     "\033[36;1m"
#define COMMENT_TEXT  "\033[38;5;8m"
#define WHITE_TEXT    "\033[38;5;254m"

static char * hl_node_to_class(int t) {
  switch (t) {
    case HL_NODE_TEXT:
/*    case HL_NODE_SCOPE_START:*/
    case HL_NODE_QUOTE:
    case HL_NODE_SYMBOL:
      return MAGENTA_TEXT;
    case HL_NODE_FUNCTION:
      return CYAN_TEXT;
    case HL_NODE_FUNCTION_CALL:
      return BLUE_TEXT;
    case HL_NODE_EXPR:
      return YELLOW_TEXT;
    case HL_NODE_NUMBER:
    case HL_NODE_TYPE:
      return RRED_TEXT;
    case HL_NODE_DECL:
      return RED_TEXT;
    case HL_NODE_DEFINITION:
      return GREEN_TEXT;
    case HL_NODE_CHCOMMENT:
    case HL_NODE_COMMENT_START:
    case HL_NODE_MCOMMENT_START:
      return COMMENT_TEXT;
      break;
    default:
      break;
  }

  return WHITE_TEXT;
}

static size_t hl_compile_concat(char *dst, char *src, size_t off) {
  int i, j;

  for (i = off, j = 0; src[j] != '\0' && i < HL_TERM_BUFFER_SIZ; i++, j++)
    dst[i] = src[j];

  return j;
}

// with endup termination
static size_t hl_compile_sanitize(char *dst, char *src, size_t len, char *class, int end) {
  int off = 0;

  off += hl_compile_concat(dst, class, off);

  for (int i = 0; i < len; i++) {
    if (src[i] == '\\') {
      off += hl_compile_concat(dst, "\x5c", off);
      continue;
    }

    if (off < HL_TERM_BUFFER_SIZ)
      dst[off++] = src[i];
  }

  if (end)
    off += hl_compile_concat(dst, "\033[0m", off);

  return off;
}

static size_t hl_compile_node(hl_node *node, char *buffer, hl_ctx *ctx) {
  size_t off = 0;
  char *class;
  char *tmp;

  switch (node->type) {
/*    case HL_NODE_TEXT:*/
    case HL_NODE_SPACE:
    case HL_NODE_NEWLINE:
      return hl_compile_concat(buffer, node->text, 0);
      break;
    case HL_NODE_CHCOMMENT:
    case HL_NODE_FUNCTION:
    case HL_NODE_FUNCTION_CALL:
    case HL_NODE_DEFINITION:
    case HL_NODE_EXPR:
    case HL_NODE_TYPE:
    case HL_NODE_QUOTE:
    case HL_NODE_DECL:
    case HL_NODE_NUMBER:
      if (ctx->lock)
        break;
      class = hl_node_to_class(node->type);
      return hl_compile_sanitize(buffer, node->text, node->text_len, class, 1);
    case HL_NODE_COMMENT_START:
    case HL_NODE_MCOMMENT_START:
      if (ctx->lock)
        break;
      ctx->lock = node->type;
      class = hl_node_to_class(node->type);
      return hl_compile_sanitize(buffer, node->text, node->text_len, class, 0);
      return off;
    case HL_NODE_COMMENT_END:
    case HL_NODE_MCOMMENT_END:
/*    case HL_NODE_SCOPE_END:*/
      ctx->lock = 0;
      off = hl_compile_concat(buffer, node->text, 0);
      off += hl_compile_concat(buffer, "\033[0m", off);
      return off;
    case HL_NODE_SYMBOL:
      break;
    default:
      break;
  }

  if (ctx->lock) {
    class = hl_node_to_class(ctx->lock);
    off = hl_compile_sanitize(buffer, node->text, node->text_len, class, 0);
    return off;
  } else {
    return hl_compile_sanitize(buffer, node->text, node->text_len, WHITE_TEXT, 1);
  }
}

char * hl_compile_term(hl_root *root) {
  size_t len = 0, plen;
  hl_node *iter = root->node->root;

  hl_ctx *ctx = (hl_ctx *)malloc(sizeof(*ctx));
  char *out = (char *)malloc(sizeof(char));
  char *buffer = (char *)malloc(sizeof(char) * HL_TERM_BUFFER_SIZ);

  for (size_t i = 0; i < root->size; i++) {
    iter = iter->children;
    plen = len;
    len += hl_compile_node(iter, buffer, ctx);
    out = (char *)realloc(out, len * sizeof(char) + 1);

    for (size_t j = plen, k = 0; j < len; j++, k++)
      out[j] = buffer[k];
  }

  free(buffer);
  free(ctx);
  out[len] = 0;

  return out;
}
