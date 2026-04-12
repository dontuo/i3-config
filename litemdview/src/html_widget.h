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

#include <gtkmm/drawingarea.h>
#include "../litehtml/containers/linux/container_linux.h"
#include "file_loader.h"
#include "html_loader.h"

class browser_window;

class html_widget : public Gtk::DrawingArea,
  public container_linux
{

  litehtml::tstring           m_url;
  litehtml::tstring           m_base_url;
  litehtml::document::ptr     m_html;
  litehtml::context*          m_html_context;
  int                         m_rendered_width;
  litehtml::tstring           m_cursor;
  litehtml::tstring           m_clicked_url;
  browser_window*             m_browser;
  std::string                 m_hash;
  bool                        m_hash_valid;

  public:
  file_loader                 m_file;
  html_loader                 m_html_file;
  html_widget(litehtml::context* html_context, browser_window* browser);
  virtual ~html_widget();
  void open_mem(char *data);
  void open_page(const litehtml::tstring& url, const litehtml::tstring& hash);
  void show_hash(const litehtml::tstring& hash);
  void update_cursor();
  void on_parent_size_allocate(Gtk::Allocation allocation);
  void on_size_allocate(Gtk::Allocation& allocation) override;

  void set_font_size(const litehtml::tstring &selector, uint32_t size);
  void rerender(void);
  void on_parent_changed(Gtk::Widget* previous_parent) override;

  protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  void scroll_to(int x, int y);

  void get_client_rect(litehtml::position& client) const override;

  void on_anchor_click(const litehtml::tchar_t* url,
      const litehtml::element::ptr& el) override;

  void set_cursor(const litehtml::tchar_t* cursor) override;

  void import_css(litehtml::tstring& text,
      const litehtml::tstring& url, litehtml::tstring& baseurl) override;

  void set_caption(const litehtml::tchar_t* caption) override;
  void set_base_url(const litehtml::tchar_t* base_url) override;

  Glib::RefPtr<Gdk::Pixbuf>  get_image(
      const litehtml::tchar_t* url, bool redraw_on_ready) override;

  void make_url(const litehtml::tchar_t* url,
      const litehtml::tchar_t* basepath, litehtml::tstring& out ) override;

  bool on_button_press_event(GdkEventButton* event) override;
  bool on_button_release_event(GdkEventButton* event) override;
  bool on_motion_notify_event(GdkEventMotion* event) override;

  private:
  litehtml::element::ptr get_element(const litehtml::tstring &selector);
  void load_text_file(const litehtml::tstring& url, litehtml::tstring& out);
  Gtk::Allocation get_parent_allocation();
};
