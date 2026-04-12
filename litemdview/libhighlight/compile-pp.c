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
#include <stdio.h>

static void hl_print_node(hl_node *node) {
  switch (node->type) {
    default:
      printf(" %d<%s>", node->type, node->text);
      break;
  }
}

/*
 * A simple pretty print compiler
 */
void hl_compile_pp(hl_node *node) {
  hl_node *iter = node->root;

  while (1) {
    iter = iter->children;

    if (!iter)
      break;

    hl_print_node(iter);
  }
}

