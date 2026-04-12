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

LDADD = ../litehtml/liblitehtml.la \
	../litehtml/libgumbo.la ../discount/libmarkdown.la \
	../libhighlight/libhighlight.la

bin_PROGRAMS = litemdview
litemdview_CXXFLAGS = @EXTRA_CXXFLAGS@ -I../litehtml/include/ \
											-I../discount/ -I../litehtml/src/gumbo/include \
											-I../stb -I../libhighlight
litemdview_LDFLAGS = @EXTRA_LIBS@ ../discount/libmarkdown.la \
															../libhighlight/libhighlight.la
litemdview_SOURCES = browser_wnd.cpp browser_wnd.h file_loader.cpp \
	file_loader.h globals.h html_widget.cpp html_widget.h main.cpp \
	web_history.h web_history.cpp html_loader.cpp html_loader.h \
	../litehtml/containers/linux/container_linux.cpp \
	../litehtml/containers/linux/container_linux.h \
	../libhighlight/libhighlight.la \
	../discount/libmarkdown.la \
	../litehtml/liblitehtml.la

XXD="../xxd/xxd"

master.css.inc:
	cat ../themes/master.css | $(XXD) -i > $@
	cat ../themes/markdown-light.css | $(XXD) -i > theme_light.css.inc
	cat ../themes/markdown-dark.css | $(XXD) -i > theme_dark.css.inc
	cat ../themes/markdown-darker.css | $(XXD) -i > theme_darker.css.inc
	cat ../README.md | $(XXD) -i > readme.md.inc
	cat ../md-cheatsheet/README.md | $(XXD) -i > cheatsheet.md.inc
	cat ../themes/gtkcssprovider.css | $(XXD) -i > gtkcssprovider.css.inc

main.cpp: master.css.inc
