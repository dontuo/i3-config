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

AC_INIT(litemdview, 0.0.32)
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

PKG_CHECK_MODULES(FONTCONFIG, [fontconfig])
PKG_CHECK_MODULES(GDKMM, [gdkmm-3.0])
PKG_CHECK_MODULES(GTKMM, [gtkmm-3.0])
PKG_CHECK_MODULES(CAIRO, [cairo])
PKG_CHECK_MODULES(PANGO, [pango])
PKG_CHECK_MODULES(PANGOCAIRO, [pangocairo])

CFLAGS="$CFLAGS -Wno-sign-compare -Wno-narrowing -D_REENTRANT -D _FILE_OFFSET_BITS=64"
EXTRA_CXXFLAGS="$CFLAGS $FONTCONFIG_CFLAGS $GDKMM_CFLAGS $GTKMM_CFLAGS $CAIRO_CFLAGS"
EXTRA_CXXFLAGS="$EXTRA_CXXFLAGS $PANGO_CFLAGS $PANGOCAIRO_CFLAGS"
EXTRA_LIBS="$LIBS $FONTCONFIG_LIBS $GDKMM_LIBS $GTKMM_LIBS $CAIRO_LIBS"
EXTRA_LIBS="$EXTRA_LIBS $PANGO_LIBS $PANGOCAIRO_LIBS"

AC_SUBST(EXTRA_CXXFLAGS)
AC_SUBST(EXTRA_LIBS)
AC_HEADER_STDC
AC_CONFIG_MACRO_DIRS([m4])
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h unistd.h])

AC_ARG_ENABLE([embedded_img],
                  AS_HELP_STRING([--enable-embedded-img], [enable embedded images support]))

AS_IF([test "x$enable_embedded_img" = "xyes"], [
  AC_DEFINE(CONFIG_USE_EMBEDDED_IMG, 1, [embedded images support])
])

AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AM_SILENT_RULES([yes])
AC_STRUCT_ST_BLOCKS
AC_HEADER_TIME
AC_STRUCT_TM

AC_CHECK_LIB(m, main)
AC_CHECK_HEADERS([stdlib.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
                  Makefile
                  xxd/Makefile
                  discount/Makefile
                  libhighlight/Makefile
                  litehtml/Makefile
                  src/Makefile
                   ])
LT_INIT
AC_OUTPUT