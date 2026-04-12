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

#include "globals.h"
#include <iostream>
#include <gumbo.h>
#include "html_loader.h"

html_loader::html_loader() { }
html_loader::~html_loader() { }

char *last_gumbo_out;

static char *body_wrap(char *data)  {
  const char *p1 = "<!doctype html><html><head><style>";
  const char *p2 = "</style></head><body><div class=\"markdown-body\">";
  const char *p3 = "</body></html>";
  char *out;
  size_t str_size;
  std::stringstream stream;
  std::string str;

  stream << p1;
  stream << ldv_config.theme;
  stream << p2;
  stream << data;
  stream << p3;

  str = stream.str();
  str_size = str.size();

  out = (char *)malloc(str_size * sizeof(char) + 1);
  memcpy(out, str.c_str(), str_size);
  out[str_size] = 0;

  return out;
}

static void get_body(GumboNode *node) {
  GumboVector *children;
  int i;

  /* If the current node is not an element, then the direct return */
  if (node->type != GUMBO_NODE_ELEMENT) return;

  if (node->v.element.tag = GUMBO_TAG_BODY) {
    if (!last_gumbo_out && node->v.element.original_tag.data) {
      last_gumbo_out = strdup(node->v.element.original_tag.data);
    }
    return;
  }

  if (node->v.element.tag == GUMBO_TAG_BODY ||
    node->v.element.tag == GUMBO_TAG_HTML) {
    /* Get the node of all child element node */
    children = &node->v.element.children;

    /* Recursive all child nodes of this node */
    for (i = 0; !last_gumbo_out && i < children->length;++i)
      get_body((GumboNode *)children->data[i]);
  }
}

char *html_loader::prepare_data(char *data, size_t len) {
  (void) len;
  GumboOutput *output;
  char *out = nullptr;
  last_gumbo_out = nullptr;

  output = gumbo_parse(data);

  get_body(output->root);

  gumbo_destroy_output(&kGumboDefaultOptions,output);

  if (last_gumbo_out) {
    out = body_wrap(last_gumbo_out);
    free(last_gumbo_out);
  }

  return out;
}

bool html_loader::is_html_data(char *data, size_t len) {
  int i = 0;

  for (; i < len; i++) { //iterate till first character accurs
    if (isspace(data[i]))
      continue;
    break;
  }

  if (data[i] == '<' && data[i+1] == '!') { // it is definitely <!doctype
    return true;
  }

  return false;
}

