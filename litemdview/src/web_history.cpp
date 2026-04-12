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
#include "web_history.h"

web_history::web_history() {
  m_current_item = 0;
}

web_history::~web_history() { }

void web_history::url_opened( const std::string& url ) {
  if(!m_items.empty()) {

    if (m_current_item != m_items.size() - 1) {

      if(m_current_item > 0 && m_items[m_current_item - 1] == url) {
        m_current_item--;
      } else if (m_current_item < m_items.size() - 1 && m_items[m_current_item + 1] == url) {
        m_current_item++;
      } else {

        m_items.erase(m_items.begin() + m_current_item + 1, m_items.end());
        m_items.push_back(url);
        m_current_item = m_items.size() - 1;
      }

    } else {

      if (m_current_item > 0 && m_items[m_current_item - 1] == url) {
        m_current_item--;
      } else {
        m_items.push_back(url);
        m_current_item = m_items.size() - 1;
      }
    }
  } else {
    m_items.push_back(url);
    m_current_item = m_items.size() - 1;
  }
}

bool web_history::back( std::string& url) {
  if (m_items.empty()) return false;

  if (m_current_item > 0) {
    url = m_items[m_current_item - 1];
    return true;
  }

  return false;
}

bool web_history::forward( std::string& url) {
  if(m_items.empty())	return false;

  if(m_current_item < m_items.size() - 1) {
    url = m_items[m_current_item + 1];
    return true;
  }

  return false;
}
