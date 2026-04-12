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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <libhighlight.h>

/*
 * A list for tokens
 */
hl_node * hl_node_create() {
  hl_node * root = (hl_node *)malloc(sizeof(*root));

  memset(root, 0, sizeof(*root));
  root->root = root;

  return root;
}

hl_node * hl_node_insert(hl_node * node) {
  hl_node *children = hl_node_create();

  children->root = node->root;
  children->parent = node;

  node->children = children;

  return children;
}

hl_node * hl_node_at(hl_node * node, int at) {
  hl_node *iter = node->root;

  for (int i = 0; i < at; i++) {
    if (iter->children)
      iter = iter->children;
  }

  return iter;
}

void hl_node_free(hl_node * root) {
  hl_node *iter = root->root;
  iter->parent = NULL;

  while (iter->children) {
    iter = iter->children;
  }

  while (iter->parent) {
    iter = iter->parent;

    if (iter->children->text)
      free(iter->children->text);

    free(iter->children);
  }

  if (iter->text)
    free(iter->text);

  free(iter);
}

void hl_root_free(hl_root *root) {
  if (root) {
    if (root->node->root)
      hl_node_free(root->node->root);
    free(root);
  }
}

