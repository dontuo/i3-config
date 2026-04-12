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

#pragma once

#include <fstream>
#include <string>
#include <cerrno>
#include <clocale>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <memory.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cairo.h>
#include <gtkmm.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <litehtml.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fontconfig/fontconfig.h>
#include <cairo-ft.h>
#include <gdk/gdk.h>
#include <cairomm/context.h>
#include <config.h>

#define LTMD_MAXFONTSIZE 48
#define LTMD_MINFONTSIZE 16

#define LTMD_USERTHEME_FILENAME ".litemdview"

struct ldv_config {
  char *theme;
  char *external_theme;
  char *mkd_opt;
  char *url;
  bool print_links;
  bool debug;
  uint8_t font_size;
  time_t url_last_modified;
  Glib::RefPtr<Gtk::Application> app;
};

extern struct ldv_config ldv_config;
extern char readme_md[];
extern char cheatsheet_md[];
extern char gtk_css[];
extern char master_css[];
extern char theme_light_css[];
extern char theme_dark_css[];
extern char theme_darker_css[];

char *ldv_makemd(const char *markdown, size_t markdown_len);
void ldv_log(const char *fmt, ...);

