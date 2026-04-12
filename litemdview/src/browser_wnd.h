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

#include "html_widget.h"
#include "file_loader.h"
#include "web_history.h"
#include <string>

class browser_window : public Gtk::Window {
  public:
    browser_window(litehtml::context* html_context);
    virtual ~browser_window();

    void open_url(const litehtml::tstring& url);
    void set_url(const litehtml::tstring &url);
    void show_info(char *text, bool autohide);
    bool hide_info(int arg);

    Gtk::ScrolledWindow* get_scrolled() { return &m_scrolled_wnd; }

  private:
    bool on_address_key_press(GdkEventKey* event);
    bool on_key_press(GdkEventKey *event);
    bool on_scroll_event(GdkEventScroll *event);
    bool on_time_out(int arg);
    void open_mem(char *data);
    void on_forward_clicked();
    void on_back_clicked();
    bool on_button_press(GdkEventButton* event);
    void on_show_window();
    bool on_short_time_out(int arg);
    void show_loading();
    void increment_font_size();
    void decrement_font_size();
    void update_font_size();

  protected:
    sigc::connection    m_timer_once;
    sigc::connection    m_timer_info;
    web_history         m_history;
    file_loader         m_file;
    Glib::RefPtr<Gtk::Adjustment>     v_adj;
    html_widget         m_html;
    Gtk::Entry          m_address_bar;
    Gtk::Label          m_info;
    Gtk::VBox           m_vbox;
    Gtk::HBox           m_hbox;
    Gtk::ScrolledWindow m_scrolled_wnd;
};

