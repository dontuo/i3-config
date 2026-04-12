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
#include "file_loader.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <libgen.h>

file_loader::file_loader() { }
file_loader::~file_loader() { }

void file_loader::set_basepath(std::string url) {
  char relbuf[BUFSIZ];
  char unrealbuf[BUFSIZ];
  struct stat sb;

  if (stat(url.c_str(), &sb) == -1) {
    ldv_log("set_basepath() error stat [%s]", m_basepath.c_str());
    return;
  }

  if ((sb.st_mode & S_IFMT) == S_IFREG) {
    realpath(url.c_str(), relbuf);
    m_basepath = dirname(relbuf);
  } else {
    m_basepath = url;
  }

  ldv_log("set_basepath() [%s]", m_basepath.c_str());
}

static const int B64index[256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
  0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

static std::string b64decode(const void* data, const size_t len) {
  unsigned char* p = (unsigned char*)data;
  int pad = len > 0 && (len % 4 || p[len - 1] == '=');
  const size_t L = ((len + 3) / 4 - pad) * 4;
  std::string str(L / 4 * 3 + pad, '\0');

  for (size_t i = 0, j = 0; i < L; i += 4)
  {
    int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
    str[j++] = n >> 16;
    str[j++] = n >> 8 & 0xFF;
    str[j++] = n & 0xFF;
  }
  if (pad)
  {
    int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
    str[str.size() - 1] = n >> 16;

    if (len > L + 2 && p[L + 2] != '=')
    {
      n |= B64index[p[L + 2]] << 6;
      str.push_back(n >> 8 & 0xFF);
    }
  }
  return str;
}

time_t file_loader::get_modified(char *filename) {
  struct stat file_stat;
  memset(&file_stat, 0, sizeof(struct stat));
  stat(filename, &file_stat);
  return file_stat.st_mtime;
}

char *file_loader::get_content(const litehtml::tstring &url) {
  FILE *fd = fopen(url.c_str(), "r");
  char *buf;
  off_t size;
  struct stat st;

  stat(url.c_str(), &st);
  size = st.st_size;

  buf = (char *)malloc(size + 1);

  if (!buf) {
    std::cout << "Out of memory" << std::endl;
    free(buf);
    return nullptr;
  }

  fread(buf, sizeof(char), size, fd);

  if (ferror(fd) != 0) {
    free(buf);
    return nullptr;
  }

  buf[size] = 0;
  fclose(fd);

  return buf;
}

bool file_loader::exists(const litehtml::tstring& url) {
  struct stat file_stat;
  memset(&file_stat, 0, sizeof(struct stat));
  stat(url.c_str(), &file_stat);
  if ((file_stat.st_mode & S_IFMT) == S_IFREG) {
    return true;
  }
  return false;
}

Glib::RefPtr<Gdk::Pixbuf> file_loader::decode_base64(const litehtml::tstring &data) {
  Glib::RefPtr<Gdk::Pixbuf> ptr;
  std::string decoded;
  size_t data_size = data.size();
  guint8 *img;
  int w, h, n;

  if (data_size < 8)
    return ptr;

  decoded = b64decode(data.c_str(), data_size);
  data_size = decoded.length();

  if (data_size < 1)
    return ptr;

  img = static_cast<guint8*>(stbi_load_from_memory(
      (unsigned char*)decoded.c_str(), data_size, &w, &h, &n, 3));

  if (!img)
    return ptr;

  Glib::RefPtr<Gdk::Pixbuf> Pixbuf_from_data(Gdk::Pixbuf::create_from_data(
      img, Gdk::COLORSPACE_RGB, false, 8, w, h, 3 * w));

  ptr = Pixbuf_from_data->copy();
  free(img);

  return ptr;
}

Glib::RefPtr<Gio::InputStream> file_loader::load_file(const litehtml::tstring& url) {
  m_url = url;
  Glib::RefPtr<Gio::FileInputStream> stream;
  Glib::RefPtr<Gio::File> file;

  try {
    file = Gio::File::create_for_path(m_url);
    stream = file->read();
  } catch (Glib::Error &e) {
    ldv_log("load_file() error [%s]", url);
    return Glib::RefPtr<Gio::InputStream>(nullptr);
  }

  return stream;
}

std::string file_loader::find_fix_path(std::string &url) {
  std::string m_url = url;

  ldv_log("find_fix_path() [%s]", m_url.c_str());

  if (exists(m_url)) {
    return m_url;
  }

  if (exists(m_url + "/README.md")) {
    return m_url + "/README.md";
  }

  if (exists(m_url + "/index.md")) {
    return m_url + "/index.md";
  }

  if (m_url.at(0) == '/' && exists("." + m_url)) {
    return "." + m_url;
  }

  if (m_url.at(0) == '/' && exists("." + m_url + "/README.md")) {
    return "." + m_url + "/README.md";
  }

  if (exists(m_basepath + "/" + m_url)) {
    return m_basepath + "/" + m_url;
  }

  return m_url;
}

