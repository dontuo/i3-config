#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Author: g0tsu
# Email:  g0tsu at dnmx.0rg
#
# This file is a part of litemdview project
# https://notabug.org/g0tsu/litemdview

noinst_LTLIBRARIES=libgumbo.la liblitehtml.la
libgumbo_la_CFLAGS=-I./src/gumbo/include -I./src/gumbo/include/gumbo
libgumbo_la_LDFLAGS=-static
libgumbo_la_SOURCES=src/gumbo/attribute.c src/gumbo/char_ref.c src/gumbo/error.c \
									src/gumbo/parser.c src/gumbo/string_buffer.c \
									src/gumbo/string_piece.c src/gumbo/tag.c src/gumbo/tokenizer.c \
									src/gumbo/utf8.c src/gumbo/util.c src/gumbo/vector.c
liblitehtml_la_CXXFLAGS=-I./src/gumbo/include -I./src/gumbo/include/gumbo \
									 -I./include -I./include/litehtml
liblitehtml_la_LDFLAGS=-lgumbo -static
liblitehtml_la_SOURCES= src/background.cpp src/box.cpp src/codepoint.cpp \
	src/context.cpp src/css_length.cpp src/css_selector.cpp src/document.cpp \
	src/el_anchor.cpp src/el_base.cpp src/el_before_after.cpp src/el_body.cpp \
	src/el_break.cpp src/el_cdata.cpp src/el_comment.cpp src/el_div.cpp \
	src/element.cpp src/el_font.cpp src/el_image.cpp src/el_li.cpp \
	src/el_link.cpp src/el_para.cpp src/el_script.cpp src/el_space.cpp \
	src/el_style.cpp src/el_table.cpp src/el_td.cpp src/el_text.cpp \
	src/el_title.cpp src/el_tr.cpp src/html.cpp src/html_tag.cpp \
	src/iterators.cpp src/media_query.cpp src/num_cvt.cpp src/strtod.cpp \
	src/style.cpp src/stylesheet.cpp src/table.cpp src/tstring_view.cpp \
	src/url.cpp src/url_path.cpp src/utf8_strings.cpp src/web_color.cpp
