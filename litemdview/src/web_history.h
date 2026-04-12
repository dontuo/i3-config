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

typedef std::vector<std::string> string_vector;

class web_history {
  string_vector m_items;
  string_vector::size_type  m_current_item;
  public:
  web_history();
  virtual ~web_history();

  void url_opened(const std::string& url);
  bool back(std::string& url);
  bool forward(std::string& url);
  std::string current() const
  {
    if(m_current_item && m_current_item < m_items.size())
    {
      return m_items[m_current_item];
    }
    return "";
  }
};