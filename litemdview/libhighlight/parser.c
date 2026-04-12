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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/*
 * Single iteration parser/tokenizer
 * with comments and quotes support
 */
#define MAX_WORD_SIZE 4096
hl_root * hl_parser(uint8_t *buffer, size_t len,
    void (*user_next_word)(char *, int, hl_node *, hl_ctx *)) {

  char *word = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
  int word_len = 0;
  char iscomment = 0, skipcomment = 0;
  char _prev[1], _pprev[1];
  char *pp = _prev, *ppp = _pprev, *cp = buffer;
  size_t lenct = 0;
  void (*next_word)(char *, int, hl_node *, hl_ctx *) =
    (user_next_word ? user_next_word : hl_next_word);

  hl_ctx *ctx = (hl_ctx *)malloc(sizeof(*ctx));
  memset(ctx, 0, sizeof(*ctx));

  hl_root *res = (hl_root *)malloc(sizeof(*res));
  memset(res, 0, sizeof(*res));

  res->node = hl_node_create();

  while (lenct++ != len) {
    switch (*cp) {
      case ' ':
      case '\n':
      case '\t':
      case '\v':
        if (iscomment || skipcomment)
          break;

        goto next_word;
      case '\'':
        if (iscomment || (*pp == '\\' && *ppp != '\\'))
          break;

        skipcomment = !skipcomment;
        break;
      case '}':
      case '{':
      case ')':
      case '(':
      case ';':
      case ',':
      case ':':
      case '/':
      case '>':
      case '<':
        if (iscomment || skipcomment)
          break;

        goto next_word;
        break;
      case '"':
        if (skipcomment || *pp == '\\')
          break;

        iscomment = !iscomment;
        break;
      default:
        break;
    }

    if (word_len < MAX_WORD_SIZE)
      word[word_len++] = *cp;
    else {
      goto next_word;
    }

    *ppp = *pp;
    *pp = *cp;

    if (0) {
next_word:

      if (word_len != 0) {
        res->text_size += word_len;
        res->size++;

        res->node = hl_node_insert(res->node);
        *(word + word_len) = 0;
        next_word(word, word_len, res->node, ctx);
        word_len = 0;
      }

      res->text_size++;
      res->size++;

      *word = *cp;
      res->node = hl_node_insert(res->node);
      *(word + 1) = 0;
      next_word(word, 1, res->node, ctx);
    }

    if (lenct == len) {
      if (word_len > 0) {
        res->text_size += word_len;
        res->size++;

        res->node = hl_node_insert(res->node);
        *(word + word_len) = 0;
        next_word(word, word_len, res->node, ctx);
      }
      break;
    }

    cp++;
  }

  /*
   * Always end up with newline
   */
  if (*cp != '\n') {
    res->node = hl_node_insert(res->node);
    next_word("\n", 1, res->node, ctx);
  }

  free(ctx);
  free(word);

  return res;
}

hl_root * hl_parser_file(
    int fd, void (*user_next_word)(char *, int, hl_node *, hl_ctx *)) {

  char *word = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
  int word_len = 0;
  char iscomment = 0, skipcomment = 0;
  char _prev[1], _pprev[1], _buff[1];
  char *pp = _prev, *ppp = _pprev, *cp = _buff;
  size_t lenct = 0;
  void (*next_word)(char *, int, hl_node *, hl_ctx *) =
    (user_next_word ? user_next_word : hl_next_word);

  hl_ctx *ctx = (hl_ctx *)malloc(sizeof(*ctx));
  memset(ctx, 0, sizeof(*ctx));

  hl_root *res = (hl_root *)malloc(sizeof(*res));
  memset(res, 0, sizeof(*res));

  res->node = hl_node_create();

  while (read(fd, cp, 1) != 0) {
    switch (*cp) {
      case ' ':
      case '\n':
      case '\t':
      case '\v':
        if (iscomment || skipcomment)
          break;

        goto next_fword;
      case '\'':
        if (iscomment || (*pp == '\\' && *ppp != '\\'))
          break;

        skipcomment = !skipcomment;
        break;
      case '}':
      case '{':
      case ')':
      case '(':
      case ';':
      case ',':
      case ':':
      case '/':
      case '>':
      case '<':
        if (iscomment || skipcomment)
          break;

        goto next_fword;
        break;
      case '"':
        if (skipcomment || *pp == '\\')
          break;

        iscomment = !iscomment;
        break;
      default:
        break;
    }

    if (word_len < MAX_WORD_SIZE)
      word[word_len++] = *cp;
    else {
      goto next_fword;
    }

    *ppp = *pp;
    *pp = *cp;

    if (0) {
next_fword:

      if (word_len != 0) {
        res->text_size += word_len;
        res->size++;

        res->node = hl_node_insert(res->node);
        *(word + word_len) = 0;
        next_word(word, word_len, res->node, ctx);
        word_len = 0;
      }

      res->text_size++;
      res->size++;

      *word = *cp;
      res->node = hl_node_insert(res->node);
      *(word + 1) = 0;
      next_word(word, 1, res->node, ctx);
    }
  }

  /*
   * Always end up with newline
   */
  if (*cp != '\n') {
    res->node = hl_node_insert(res->node);
    next_word("\n", 1, res->node, ctx);
  }

  free(ctx);
  free(word);

  return res;
}
