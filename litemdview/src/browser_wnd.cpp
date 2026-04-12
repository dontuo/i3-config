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
#include "browser_wnd.h"
#include "web_history.h"
#include <gdk/gdkkeysyms.h>
#include <sstream>

#define _WTIMER_TIMEOUT 1000
#define _INFOHIDE_TIMEOUT 1000

browser_window::browser_window(litehtml::context* html_context) :
  m_html(html_context, this)
{
  m_file = m_html.m_file;
  set_title("litemdview");

  sigc::slot<bool()> timer0 = sigc::bind(sigc::mem_fun(*this,
                        &browser_window::on_time_out), 0);
  Glib::signal_timeout().connect(timer0, _WTIMER_TIMEOUT);

  add(m_vbox);

  m_vbox.show();

  v_adj = m_scrolled_wnd.get_vadjustment();
  m_vbox.pack_start(m_scrolled_wnd, Gtk::PACK_EXPAND_WIDGET);
  m_scrolled_wnd.add_events(Gdk::SMOOTH_SCROLL_MASK);
  m_scrolled_wnd.show();
  m_scrolled_wnd.add(m_html);
  m_html.show();

  m_vbox.pack_start(m_hbox, Gtk::PACK_SHRINK);
  m_hbox.show();

  m_hbox.pack_start(m_address_bar, Gtk::PACK_EXPAND_WIDGET);
  m_hbox.pack_start(m_info, Gtk::PACK_EXPAND_WIDGET);

  m_info.set_xalign(0);
  m_info.set_text("");
  m_info.hide();

  m_address_bar.hide();

  if (ldv_config.url) {
    m_file.set_basepath(ldv_config.url);
    m_address_bar.set_text(ldv_config.url);
  } else {
    m_address_bar.set_text("");
  }

  m_address_bar.add_events(Gdk::KEY_PRESS_MASK);
  m_address_bar.signal_key_press_event().connect(
      sigc::mem_fun(*this, &browser_window::on_address_key_press), false );

  signal_key_press_event().connect(
      sigc::mem_fun(*this, &browser_window::on_key_press), false);
  m_html.signal_button_press_event().connect(
      sigc::mem_fun(*this, &browser_window::on_button_press), false);
  m_html.signal_scroll_event().connect(
      sigc::mem_fun(*this, &browser_window::on_scroll_event), false);
  set_default_size(1280, 720);

  sigc::slot<bool()> timer1 = sigc::bind(sigc::mem_fun(*this,
                        &browser_window::on_short_time_out), 0);
  m_timer_once = Glib::signal_timeout().connect(timer1, 100);
  show_loading();
}

browser_window::~browser_window() { }

void browser_window::on_show_window() {
  if (!ldv_config.url) {
    open_mem(readme_md);
  } else {
    open_url(ldv_config.url);
  }
}

bool browser_window::on_short_time_out(int arg) {
  (void) arg;
  on_show_window();
  m_timer_once.disconnect();
  return true;
}

bool browser_window::on_key_press(GdkEventKey *event) {
  double vval;

  if (m_address_bar.get_visible() && event->keyval != GDK_KEY_Escape) {
    return false;
  }

  switch (event->keyval) {
    case GDK_KEY_Escape:
      /* uncomfortable and not convinient */
//      if (!m_address_bar.get_visible())
//        ldv_config.app->quit();
      m_address_bar.hide();
//      vval = v_adj->get_value();
//      v_adj->set_value(vval);
      return true;
    case '?':
      open_mem(cheatsheet_md);
      break;
    case GDK_KEY_plus:
      increment_font_size();
      break;
    case GDK_KEY_minus:
      decrement_font_size();
      break;
    case GDK_KEY_equal:
      ldv_config.font_size = LTMD_MINFONTSIZE;
      m_html.set_font_size(".markdown-body", LTMD_MINFONTSIZE);
      break;
    case GDK_KEY_h:
    case GDK_KEY_BackSpace:
      on_back_clicked();
      break;
    case GDK_KEY_l:
      on_forward_clicked();
      break;
    case GDK_KEY_0:
      if (ldv_config.external_theme)
        ldv_config.theme = ldv_config.external_theme;
      else
        ldv_config.theme = theme_darker_css;

      if (ldv_config.url)
        open_url(ldv_config.url);
      else
        open_mem(cheatsheet_md);
      break;
    case GDK_KEY_1:
      ldv_config.theme = theme_light_css;
      if (ldv_config.url)
        open_url(ldv_config.url);
      else
        open_mem(cheatsheet_md);
      break;
    case GDK_KEY_2:
      ldv_config.theme = theme_dark_css;
      if (ldv_config.url)
        open_url(ldv_config.url);
      else
        open_mem(cheatsheet_md);
      break;
    case GDK_KEY_3:
      if (ldv_config.external_theme)
        ldv_config.theme = theme_darker_css;
      else
        break;

      if (ldv_config.url)
        open_url(ldv_config.url);
      else
        open_mem(cheatsheet_md);
      break;
    case GDK_KEY_q:
      if (!m_address_bar.get_visible())
        ldv_config.app->quit();
      break;
    case GDK_KEY_End:
    case GDK_KEY_G:
      vval = v_adj->get_upper();
      v_adj->set_value(vval);
      break;
    case GDK_KEY_Home:
    case GDK_KEY_g:
      vval = 0;
      v_adj->set_value(vval);
      break;
    case GDK_KEY_Down:
    case GDK_KEY_j:
      vval = v_adj->get_value() + v_adj->get_step_increment();
      v_adj->set_value(vval);
      break;
    case GDK_KEY_n:
      if (event->state & GDK_CONTROL_MASK) {
        vval = v_adj->get_value() + v_adj->get_step_increment();
        v_adj->set_value(vval);
      }
      break;
    case GDK_KEY_f:
      if (event->state & GDK_CONTROL_MASK) {
        vval = v_adj->get_value() + v_adj->get_page_size();
        v_adj->set_value(vval);
      }
      break;
    case GDK_KEY_Up:
    case GDK_KEY_k:
      vval = v_adj->get_value() - v_adj->get_step_increment();
      v_adj->set_value(vval);
      break;
    case GDK_KEY_p:
      if (event->state & GDK_CONTROL_MASK) {
        vval = v_adj->get_value() - v_adj->get_step_increment();
        v_adj->set_value(vval);
      }
      break;
    case GDK_KEY_b:
      if (event->state & GDK_CONTROL_MASK) {
        vval = v_adj->get_value() - v_adj->get_page_size();
        v_adj->set_value(vval);
      }
      break;
    case '/':
    case ':':
    case GDK_KEY_o:
      if (m_address_bar.get_visible()) {
        return false;
      }
      m_info.hide();
      m_address_bar.show();
      m_address_bar.grab_focus();
      return true;
    default:
      break;
  }

  return false;
}

void browser_window::show_info(char *text, bool autohide) {
  if (!m_address_bar.get_visible()) {
    m_info.set_text(text);

    if (!m_info.get_visible()) {
      m_info.show();

      if (autohide) {
        sigc::slot<bool()> slot1 = 
          sigc::bind(sigc::mem_fun(*this, &browser_window::hide_info), 0);
        m_timer_info = Glib::signal_timeout().connect(slot1, _INFOHIDE_TIMEOUT);
      }
    }
  }
}

bool browser_window::hide_info(int arg) {
  (void) arg;

  if (m_info.get_visible()) {
    if (m_timer_info) {
      m_timer_info.disconnect();
    }

    m_info.hide();
  }

  return true;
}

void browser_window::update_font_size() {
  if (ldv_config.font_size > LTMD_MAXFONTSIZE)
    ldv_config.font_size = LTMD_MAXFONTSIZE;

  if (ldv_config.font_size < LTMD_MINFONTSIZE)
    ldv_config.font_size = LTMD_MINFONTSIZE;

  m_html.set_font_size(".markdown-body", ldv_config.font_size);
}

void browser_window::increment_font_size() {
  char msg[24];

  if (ldv_config.font_size != LTMD_MAXFONTSIZE) {
    ldv_config.font_size += 4;
    update_font_size();
  }

  sprintf(msg, "zoom: %dpx", ldv_config.font_size);
  show_info(msg, true);
}

void browser_window::decrement_font_size() {
  char msg[24];

  if (ldv_config.font_size != LTMD_MINFONTSIZE) {
    ldv_config.font_size -= 4;
    update_font_size();
  }

  sprintf(msg, "zoom: %dpx", ldv_config.font_size);
  show_info(msg, true);
}

bool browser_window::on_scroll_event(GdkEventScroll *event) {
  if (event->type != GDK_SCROLL)
    return false;

  if ((event->state & GDK_CONTROL_MASK) != GDK_CONTROL_MASK)
    return false;

  if (event->direction == GDK_SCROLL_UP) {
    increment_font_size();
    return true;
  }

  if (event->direction == GDK_SCROLL_DOWN) {
    decrement_font_size();
    return true;
  }

  return false;
}

bool browser_window::on_button_press(GdkEventButton* event) {
  if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
    if (m_address_bar.get_visible()) {
      m_address_bar.hide();
    } else {
      on_back_clicked();
    }
  }

  return false;
}

bool browser_window::on_address_key_press(GdkEventKey* event) {
  if(event->keyval == GDK_KEY_Return) {
    m_address_bar.hide();
    m_address_bar.select_region(0, -1);
    litehtml::tstring url = m_address_bar.get_text();
    m_file.set_basepath(url);
    open_url(url);
    return true;
  }

  return false;
}

void browser_window::on_forward_clicked() {
  std::string url;

  if(m_history.forward(url)) {
    open_url(url);
  }
}

void browser_window::on_back_clicked() {
  std::string url;

  if(m_history.back(url)) {
    open_url(url);
  }
}

bool browser_window::on_time_out(int arg) {
  (void) arg;
  litehtml::tstring url;
  time_t last_modified = m_file.get_modified(ldv_config.url);

  if (last_modified > 0 && last_modified != ldv_config.url_last_modified) {
    url = ldv_config.url;
    open_url(url);
  }

  return true;
}

void browser_window::open_mem(char *data) {
  if (ldv_config.url) {
    free(ldv_config.url);
    ldv_config.url = nullptr;
  }

  m_html.open_mem(data);
}

void browser_window::show_loading() {
  m_html.open_mem((char *)"# Loading... \n");
}

void browser_window::open_url(const litehtml::tstring &url) {
  std::string hash;
  std::string s_url = url;
  bool open_hash_only = false;
  bool reload = false;
  std::string::size_type hash_pos;

  m_info.hide();

  if (url.rfind("http", 0) == 0) {
    if (ldv_config.print_links)
      std::cout << url << std::endl;

    m_address_bar.set_text(url);
    m_address_bar.show();
    return;
  }

  hash_pos = s_url.find_first_of(L'#');

  if (hash_pos != std::wstring::npos) {
    hash = s_url.substr(hash_pos + 1);
    s_url.erase(hash_pos);
   }

  auto current_url = m_history.current();
  hash_pos = current_url.find_first_of(L'#');

  if (hash_pos != std::wstring::npos) {
    current_url.erase(hash_pos);
  }

  if (!current_url.empty()) {
    if (m_history.current() != url) {
      if (current_url == s_url) {
        open_hash_only = true;
      }
    } else {
      reload = true;
    }
  }

  if(!open_hash_only) {
    s_url =
      m_file.find_fix_path(s_url);

    if (!m_file.exists(s_url)) {
       std::string msg = "# File not found \n###" + url;
       open_mem((char *)msg.c_str());

    } else {

      m_html.open_page(s_url, hash);
      if (ldv_config.url && s_url != ldv_config.url && hash.empty()) {
        v_adj->set_value(0);
      }
    }
  } else {
    m_html.show_hash(hash);
  }

  if (!reload && !open_hash_only) {
    m_history.url_opened(s_url);
  }

  m_address_bar.hide();

  if (ldv_config.font_size != LTMD_MINFONTSIZE) {
    m_html.on_parent_changed(nullptr);
    update_font_size();
  }

  set_url(s_url);
}

void browser_window::set_url(const litehtml::tstring &url) {
  if (ldv_config.url) {
    free(ldv_config.url);
  }

  ldv_config.url = strdup(url.c_str());
  ldv_config.url_last_modified = m_file.get_modified(ldv_config.url);
  m_address_bar.set_text(url);
}

