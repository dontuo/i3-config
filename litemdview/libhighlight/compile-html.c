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

#ifdef CONFIG_USE_HTML

#include <string.h>
#include <stdlib.h>

#define HL_HTML_BUFFER_SIZ 12000

static char * hl_node_to_class(int t) {
  switch (t) {
    case HL_NODE_TEXT:
      return "text";
    case HL_NODE_SCOPE_START:
      return "scope";
    case HL_NODE_QUOTE:
      return "quote";
    case HL_NODE_NUMBER:
    case HL_NODE_SYMBOL:
      return "symbol";
    case HL_NODE_FUNCTION:
      return "func";
    case HL_NODE_FUNCTION_CALL:
      return "call";
    case HL_NODE_EXPR:
      return "expr";
    case HL_NODE_TYPE:
      return "type";
    case HL_NODE_DECL:
      return "decl";
    case HL_NODE_DEFINITION:
      return "def";
    case HL_NODE_CHCOMMENT:
    case HL_NODE_COMMENT_START:
    case HL_NODE_MCOMMENT_START:
      return "comment";
    default:
      break;
  }

  return "t";
}

static size_t hl_compile_concat(char *dst, char *src, size_t off) {
  int i, j;

  for (i = off, j = 0; src[j] != '\0' && i < HL_HTML_BUFFER_SIZ; i++, j++)
    dst[i] = src[j];

  return j;
}

// with endup termination
static size_t hl_compile_sanitize(char *dst, char *src, size_t len, char *class, int end) {
  int off = 0;

  off += hl_compile_concat(dst, "<span class=\"hl-", off);
  off += hl_compile_concat(dst, class, off);
  off += hl_compile_concat(dst, "\">", off);

  for (int i = 0; i < len; i++) {
    if (src[i] == '<') {
      off += hl_compile_concat(dst, "&lt;", off);
      continue;
    }

    if (src[i] == '>') {
      off += hl_compile_concat(dst, "&gt;", off);
      continue;
    }

    if (off < HL_HTML_BUFFER_SIZ)
      dst[off++] = src[i];
  }

  if (end)
    off += hl_compile_concat(dst, "</span>", off);

  return off;
}

static size_t hl_compile_node(hl_node *node, char *buffer) {
  size_t off = 0;
  char *class;
  char *tmp;

  /*
   * Convert a simple types first
   */
  switch (node->type) {
    case HL_NODE_SPACE:
      if (node->text[0] == ' ')
        return hl_compile_concat(buffer, "&nbsp;", 0);
      if (node->text[0] == '\t')
        return hl_compile_concat(buffer, "&nbsp;&nbsp;", 0);
    case HL_NODE_NEWLINE:
      return hl_compile_concat(buffer, "<br>\n", 0);
    case HL_NODE_QUOTE:
    case HL_NODE_TEXT:
    case HL_NODE_FUNCTION:
    case HL_NODE_FUNCTION_CALL:
    case HL_NODE_DEFINITION:
    case HL_NODE_EXPR:
    case HL_NODE_TYPE:
    case HL_NODE_DECL:
    case HL_NODE_CHCOMMENT:
    case HL_NODE_NUMBER:
      class = hl_node_to_class(node->type);
      return hl_compile_sanitize(buffer, node->text, node->text_len, class, 1);
    case HL_NODE_COMMENT_START:
    case HL_NODE_MCOMMENT_START:
    case HL_NODE_SCOPE_START:
      class = hl_node_to_class(node->type);
      return hl_compile_sanitize(buffer, node->text, node->text_len, class, 0);
    case HL_NODE_COMMENT_END:
    case HL_NODE_MCOMMENT_END:
    case HL_NODE_SCOPE_END:
      off = hl_compile_concat(buffer, node->text, 0);
      off += hl_compile_concat(buffer, "</span>", off);
      return off;
    case HL_NODE_SYMBOL:
      if (node->text[0] == '<') {

        if (node->children)
          node->children->type = HL_NODE_DEFINITION;

        return hl_compile_concat(buffer, "&lt;", 0);
      } else if (node->text[0] == '>') {
        return hl_compile_concat(buffer, "&gt;", 0);
      }
    default:
      break;
  }

  return hl_compile_sanitize(buffer, node->text, node->text_len, "t", 1);
}

char * hl_compile_html(hl_root *root) {
  size_t len = 0, plen;
  hl_node *iter = root->node->root;

  char *out = (char *)malloc(sizeof(char));
  char *buffer = (char *)malloc(sizeof(char) * HL_HTML_BUFFER_SIZ);

  for (size_t i = 0; i < root->size; i++) {
    iter = iter->children;
    plen = len;
    len += hl_compile_node(iter, buffer);
    out = (char *)realloc(out, len * sizeof(char) + 1);

    for (size_t j = plen, k = 0; j < len; j++, k++)
      out[j] = buffer[k];
  }

  free(buffer);
  out[len] = 0;

  return out;
}

#endif

