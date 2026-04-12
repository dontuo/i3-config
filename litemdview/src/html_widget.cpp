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
#include "html_widget.h"
#include "browser_wnd.h"
#include <litehtml/url_path.h>
#include <litehtml/url.h>
#include <chrono>
#include <config.h>

#define BUFF_SIZE    10 * 1024

html_widget::html_widget(litehtml::context* html_context, browser_window* browser) {
  m_hash_valid        = false;
  m_browser           = browser;
  m_rendered_width    = 0;
  m_html_context      = html_context;
  m_html              = nullptr;
  add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
}

html_widget::~html_widget() { }

bool html_widget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  litehtml::position pos;

  GdkRectangle rect;
  gdk_cairo_get_clip_rectangle(cr->cobj(), &rect);

  pos.width    = rect.width;
  pos.height   = rect.height;
  pos.x        = rect.x;
  pos.y        = rect.y;

  cr->rectangle(0, 0, get_allocated_width(), get_allocated_height());
  cr->set_source_rgb(1, 1, 1);
  cr->fill();

  if(m_html) {
    m_html->draw((litehtml::uint_ptr) cr->cobj(), 0, 0, &pos);
  }

  return true;
}

void html_widget::get_client_rect(litehtml::position& client) const {
  client.width  = get_parent()->get_allocated_width();
  client.height = get_parent()->get_allocated_height();
  client.x = 0;
  client.y = 0;
}


void html_widget::on_anchor_click(const litehtml::tchar_t* url,
    const litehtml::element::ptr& el) {
  (void) el;

  if(url) {
    make_url(url, m_base_url.c_str(), m_clicked_url);
  }
}

void html_widget::set_cursor(const litehtml::tchar_t* cursor) {
  if(cursor) {
    if(m_cursor != cursor) {
      m_cursor = cursor;
      update_cursor();
    }
  }
}

void html_widget::import_css(litehtml::tstring& text,
    const litehtml::tstring& url, litehtml::tstring& baseurl) {
  (void) text;
  (void) url;
  (void) baseurl;
//  std::string css_url;
//  make_url(url.c_str(), baseurl.c_str(), css_url);
//  load_text_file(css_url, text);
//  if(!text.empty()) {
//    baseurl = css_url;
//  }
}

void html_widget::set_caption(const litehtml::tchar_t* caption) {
  if(get_parent_window()) {
    get_parent_window()->set_title(caption);
  }
}

void html_widget::set_base_url(const litehtml::tchar_t* base_url) {
  if(base_url) {
    m_base_url = litehtml::resolve(litehtml::url(m_url), litehtml::url(base_url)).string();
  } else {
    m_base_url = m_url;
  }
}

Glib::RefPtr<Gdk::Pixbuf> html_widget::get_image(
    const litehtml::tchar_t* url, bool redraw_on_ready) {
  (void) redraw_on_ready;
  Glib::RefPtr<Gdk::Pixbuf> ptr;
  std::string m_url = url;

  m_url = m_file.find_fix_path(m_url);

#ifdef CONFIG_USE_EMBEDDED_IMG
  if (m_url.find_first_of("data:image/png;base64,") == 0) {
    return m_file.decode_base64(m_url.substr(22));
  }
#endif

  if (!m_file.exists(m_url)) {
    ldv_log("html_widget::get_image() not exists [%s]", m_url.c_str());
    return ptr;
  }

  Glib::RefPtr< Gio::InputStream > stream = m_file.load_file(m_url);
  ptr = Gdk::Pixbuf::create_from_stream(stream);

  return ptr;
}

Gtk::Allocation html_widget::get_parent_allocation() {
  Gtk::Container* parent = get_parent();
  return parent->get_allocation();
}

void html_widget::open_mem(char *data) {
  char *htmlcontent;
  std::string html;

  if (!(htmlcontent = ldv_makemd(data, strlen(data)))) {
    return;
  }

  m_html = litehtml::document::createFromString(htmlcontent, this, m_html_context);

  if(m_html) {
    m_rendered_width = get_parent_allocation().get_width();
    m_html->render(m_rendered_width);
    set_size_request(m_html->width(), m_html->height());
  }

  free(htmlcontent);
  queue_draw();
  scroll_to(0, 0);
}

void html_widget::open_page(const litehtml::tstring& url,
    const litehtml::tstring& hash) {
  m_url       = url;
  m_base_url  = url;
  char *filecontent = nullptr;
  char *htmlcontent = nullptr;
  size_t filecontent_len;

  std::string html;

  if (!m_file.exists(url)) {
    std::cout << "File not found: " << url << std::endl;
    return;
  }

  if (!(filecontent = m_file.get_content(url))) {
    std::cout << "Filed to read: " << url << std::endl;
    return;
  }

  filecontent_len = strlen(filecontent);

  /* Lets check if it is a regular html file */
  if (!m_html_file.is_html_data(filecontent, filecontent_len)) {

    /* It has no html signature, lets try to markdowninfy it */
    if (!(htmlcontent = ldv_makemd(filecontent, filecontent_len))) {
      std::cout << "Markdown error: " << url << std::endl;
      free(filecontent);
      return;
    }

    m_html = litehtml::document::createFromString(
        htmlcontent, this, m_html_context);
  } else {
    if (!(htmlcontent = m_html_file.prepare_data(filecontent, filecontent_len))) {
      std::cout << "Prepare html error: " << url << std::endl;
      free(filecontent);
      return;
    }
  }

  m_html = litehtml::document::createFromString(
        htmlcontent, this, m_html_context);

  if (m_html) {
    m_rendered_width = get_parent_allocation().get_width();
    m_html->render(m_rendered_width);
    m_hash = hash;
    m_hash_valid = true;
    set_size_request(m_html->width(), m_html->height());
  }

  free(filecontent);
  free(htmlcontent);
  queue_draw();
}

void html_widget::scroll_to(int x, int y) {
  auto vadj = m_browser->get_scrolled()->get_vadjustment();
  auto hadj = m_browser->get_scrolled()->get_hadjustment();
  vadj->set_value(vadj->get_lower() + y);
  hadj->set_value(hadj->get_lower() + x);
}

litehtml::element::ptr html_widget::get_element(const litehtml::tstring &selector) {
    litehtml::element::ptr el = m_html->root()->select_one(selector);
    return el;
}

void html_widget::rerender(void) {
  m_html->media_changed();
  queue_draw();
  m_html->render(m_rendered_width);
  set_size_request(m_html->width(), m_html->height());
}

void html_widget::set_font_size(const litehtml::tstring &selector, uint32_t size) {
  litehtml::element::ptr el;
  char style_string[26];

  snprintf(style_string, 25, "font-size: %dpx;", size);

  el = get_element(selector);

  if (!el)
    return;

  el->set_attr("style", style_string);
  el->parse_styles();
  rerender();
}

void html_widget::show_hash(const litehtml::tstring& hash) {
  if(!hash.empty()) {
    std::string selector = "#" + hash;
    litehtml::element::ptr el = m_html->root()->select_one(selector);

    if (!el) {
      selector = "[name=" + hash + "]";
      el = m_html->root()->select_one(selector);
    }

    if (el) {
      litehtml::position pos = el->get_placement();
      scroll_to(0, pos.top());
    }
  }
}

void html_widget::make_url(const litehtml::tchar_t* url, 
    const litehtml::tchar_t* basepath, litehtml::tstring& out) {

  if(!basepath || !basepath[0]) {

    if(!m_base_url.empty()) {
      out = litehtml::resolve(litehtml::url(m_base_url), litehtml::url(url)).string();
    } else {
      out = url;
    }
  } else {
    out = litehtml::resolve(litehtml::url(basepath), litehtml::url(url)).string();
  }
}

void html_widget::on_parent_size_allocate(Gtk::Allocation allocation) {
  if(m_html && m_rendered_width != allocation.get_width()) {
    m_rendered_width = allocation.get_width();
    m_html->media_changed();
    m_html->render(m_rendered_width);
    set_size_request(m_html->width(), m_html->height());
    queue_draw();
  }
}

void html_widget::on_parent_changed(Gtk::Widget* previous_parent) {
  (void) previous_parent;

  Gtk::Widget* viewport = get_parent();
  if(viewport) {
    viewport->signal_size_allocate()
      .connect(sigc::mem_fun(*this, &html_widget::on_parent_size_allocate));
  }

}
bool html_widget::on_button_press_event(GdkEventButton *event) {

  if(m_html) {
    litehtml::position::vector redraw_boxes;

    if(m_html->on_lbutton_down((int) event->x, 
          (int) event->y, (int) event->x, (int) event->y, redraw_boxes)) {

      for(auto& pos : redraw_boxes) {
        queue_draw_area(pos.x, pos.y, pos.width, pos.height);
      }
    }
  }
  return true;
}

bool html_widget::on_button_release_event(GdkEventButton *event) {

  if(m_html) {
    litehtml::position::vector redraw_boxes;
    m_clicked_url.clear();

    if(m_html->on_lbutton_up((int) event->x, (int) event->y,
          (int) event->x, (int) event->y, redraw_boxes)) {

      for(auto& pos : redraw_boxes) {
        queue_draw_area(pos.x, pos.y, pos.width, pos.height);
      }
    }

    if(!m_clicked_url.empty()) {
      m_browser->open_url(m_clicked_url);
    }
  }

  return true;
}

bool html_widget::on_motion_notify_event(GdkEventMotion *event) {
  litehtml::element::ptr over_el;
  char *href;

  if(m_html) {
    litehtml::position::vector redraw_boxes;

    if(m_html->on_mouse_over((int) event->x, (int) event->y,
        (int) event->x, (int) event->y, redraw_boxes)) {

      over_el = m_html->root()->get_element_by_point(
          (int) event->x, (int) event->y, (int) event->x, (int) event->y);

      href = (char *) over_el->get_tagName();

      if (href[0] == 'a') {
        if ((href = (char *) over_el->get_attr("href"))) {
          m_browser->show_info(href, false);
        }
      } else {
        m_browser->hide_info(0);
      }

      for(auto& pos : redraw_boxes) {
        queue_draw_area(pos.x, pos.y, pos.width, pos.height);
      }
    }
  }

  return true;
}

void html_widget::update_cursor() {

  Gdk::CursorType cursType = Gdk::ARROW;

  if(m_cursor == _t("pointer")) {
    cursType = Gdk::HAND1;
  }

  if(cursType == Gdk::ARROW) {
    get_window()->set_cursor();
  } else {
    get_window()->set_cursor( Gdk::Cursor::create(cursType) );
  }
}

void html_widget::load_text_file(
    const litehtml::tstring& url, litehtml::tstring& out) {

  out.clear();
  Glib::RefPtr< Gio::InputStream > stream = m_file.load_file(url);
  gssize sz;
  char buff[BUFF_SIZE + 1];

  while( (sz = stream->read(buff, BUFF_SIZE)) > 0 ) {
    buff[sz] = 0;
    out += buff;
  }
}

void html_widget::on_size_allocate(Gtk::Allocation& allocation) {
  Gtk::DrawingArea::on_size_allocate(allocation);
  if(m_hash_valid) {
    show_hash(m_hash);
    m_hash_valid = false;
  }
}

