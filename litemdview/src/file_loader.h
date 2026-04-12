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

class file_loader {
  std::string m_url;

  public:
  file_loader();
  ~file_loader();

  Glib::RefPtr< Gio::InputStream > load_file(const litehtml::tstring& url);
  Glib::RefPtr<Gdk::Pixbuf> decode_base64(const litehtml::tstring &data);
  const char* get_url() const;
  bool exists(const litehtml::tstring& url);
  char *get_content(const litehtml::tstring &url);
  time_t get_modified(char *filename);
  bool is_html_data(char *data, size_t len);
  std::string find_fix_path(std::string &url);
  void set_basepath(std::string url);

  private:
  std::string m_basepath;
};

