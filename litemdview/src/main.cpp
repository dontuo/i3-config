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
#include <config.h>
#include <pwd.h>

extern "C" {
#include <markdown.h>
#include <libhighlight.h>
}

struct ldv_config ldv_config;

char readme_md[] = {
#include "readme.md.inc"
,0
};

char cheatsheet_md[] = {
#include "cheatsheet.md.inc"
,0
};

char gtk_css[] = {
#include "gtkcssprovider.css.inc"
,0
};

char master_css[] = {
#include "master.css.inc"
,0
};

char theme_light_css[] = {
#include "theme_light.css.inc"
,0
};

char theme_dark_css[] = {
#include "theme_dark.css.inc"
,0
};

char theme_darker_css[] = {
#include "theme_darker.css.inc"
,0
};

void ldv_log(const char *fmt, ...) {
  if (!ldv_config.debug)
    return;

  va_list args;
  va_start(args, fmt);

  size_t fmt_len = strlen(fmt);
  char *fmtt = (char *)malloc(fmt_len * sizeof(char) + 2);
  memcpy(fmtt, fmt, fmt_len);
  fmtt[fmt_len++] = '\n';
  fmtt[fmt_len++] = 0;
  vprintf(fmtt, args);
  free(fmtt);

  va_end(args);
}

static char * ldv_code_wrap(char *src, const char *start, const char *end) {
  size_t slen = strlen(src);
  size_t start_len = strlen(start);
  size_t end_len = strlen(end);
  size_t full_len = start_len + end_len + slen;
  char *out = (char *)malloc(full_len * sizeof(char) + 2);

  memcpy(out, start, start_len);
  memcpy(out + start_len, src, slen);
  memcpy(out + start_len + slen, end, end_len);
  out[full_len] = '\0';

  return out;
}

static char * ldv_code_format(char *str, int len, void *outc) {
  (void) outc;
  char *out, *tmp;

  if (len < 1)
    return NULL;

  hl_root *root = hl_parser((uint8_t *)str, len, NULL);
  tmp = hl_compile_html(root);
  hl_root_free(root);

  out = ldv_code_wrap(tmp, "<div class=\"code-block\">", "</div>");
  free(tmp);

  return out;
}

static void ldv_free_format(char *str, void *e_data) {
  (void) e_data;

  if (str)
    free(str);
}

static char *ldv_makeheader(void) {
  const char *p1 = "<!doctype html><html><head>\n<title></title>\n<style>";
  const char *p2 = "</style></head><body><div class=\"markdown-body\">";

  return ldv_code_wrap(ldv_config.theme, p1, p2);
}

char *ldv_makemd(const char *markdown, size_t markdown_len) {
  char *header;
  const char *footer = "</div></body></html>";
  char *cp = ldv_config.mkd_opt;
  char *buf = nullptr;
  char *out = nullptr;
  size_t bsiz;

  mkd_flag_t flags;
  mkd_init_flags(&flags);

  while (*cp) {
    switch (*cp) {
      case '0':
        set_mkd_flag(&flags, MKD_NOLINKS); break;
      case '1':
        set_mkd_flag(&flags, MKD_NOIMAGE); break;
      case '2':
        set_mkd_flag(&flags, MKD_NOPANTS); break;
      case '3':
        set_mkd_flag(&flags, MKD_NOHTML); break;
      case '4':
        set_mkd_flag(&flags, MKD_NORMAL_LISTITEM); break;
      case '5':
        set_mkd_flag(&flags, MKD_TAGTEXT); break;
      case '6':
        set_mkd_flag(&flags, MKD_NO_EXT); break;
      case '7':
        set_mkd_flag(&flags, MKD_CDATA); break;
      case '8':
        set_mkd_flag(&flags, MKD_NOSUPERSCRIPT); break;
      case '9':
        set_mkd_flag(&flags, MKD_NORELAXED); break;
      case 'a':
        set_mkd_flag(&flags, MKD_NOTABLES); break;
      case 'b':
        set_mkd_flag(&flags, MKD_NOSTRIKETHROUGH); break;
      case 'c':
        set_mkd_flag(&flags, MKD_1_COMPAT); break;
      case 'd':
        set_mkd_flag(&flags, MKD_AUTOLINK); break;
      case 'e':
        set_mkd_flag(&flags, MKD_NOHEADER); break;
      case 'f':
        set_mkd_flag(&flags, MKD_TABSTOP); break;
      case 'g':
        set_mkd_flag(&flags, MKD_SAFELINK); break;
      case 'h':
        set_mkd_flag(&flags, MKD_NODIVQUOTE); break;
      case 'i':
        set_mkd_flag(&flags, MKD_NOSTYLE); break;
      case 'j':
        set_mkd_flag(&flags, MKD_DLDISCOUNT); break;
      case 'k':
        set_mkd_flag(&flags, MKD_DLEXTRA); break;
      case 'l':
        set_mkd_flag(&flags, MKD_FENCEDCODE); break;
      case 'm':
        set_mkd_flag(&flags, MKD_IDANCHOR); break;
      case 'n':
        set_mkd_flag(&flags, MKD_GITHUBTAGS); break;
      case 'o':
        set_mkd_flag(&flags, MKD_URLENCODEDANCHOR); break;
      case 'p':
        set_mkd_flag(&flags, MKD_LATEX); break;
      case 'q':
        set_mkd_flag(&flags, MKD_TOC); break;
      default:
        break;
    }
    cp++;
  }


  Document *mddoc = mkd_string(markdown, markdown_len, &flags);
  mddoc->dirty = 1;
  mddoc->cb.e_codefmt = (mkd_callback_t)ldv_code_format;
  mddoc->cb.e_free = (mkd_free_t)ldv_free_format;
  /*  mddoc->cb.e_url = url_format;*/

  if (!mddoc)
    return nullptr;

  mkd_compile(mddoc, &flags);
  bsiz = mkd_document(mddoc, &buf);

  if (bsiz < 1) {
    mkd_cleanup(mddoc);
    return nullptr;
  }

  header = ldv_makeheader();
  bsiz += strlen(header) + strlen(footer);

  out = (char *)malloc(bsiz * sizeof(char) + 8);
  sprintf(out, "%s%s%s\n", header, buf, footer);

  out[bsiz] = 0;

  free(header);
  mkd_cleanup(mddoc);
  return out;
}

static void ldv_set_defaults(void) {
  ldv_config.theme = theme_darker_css;
  ldv_config.mkd_opt = strdup("68dehilnmq");
  ldv_config.url = nullptr;
  ldv_config.font_size = 16;
  ldv_config.print_links = false;
  ldv_config.external_theme = nullptr;
  /* Just in case */
  ldv_config.debug = false;
}

static void version() {
  fprintf(stdout, "litemdview %s\n", VERSION);
}

static void usage(char *app) {
  fprintf(stdout, "litemdview %s\n", VERSION);
  fprintf(stdout, "This program is a free software published under GPLv2.\n");
  fprintf(stdout, "Usage: %s file.md\n", app);
  fprintf(stdout, "\t-p print html into stdout and exit\n");
  fprintf(stdout, "\t-t <num> of theme to use\n");
  fprintf(stdout, "\t-s <file> load external css\n");
  fprintf(stdout, "\t-a print links into stdout\n");
  fprintf(stdout, "\t-h show this information\n");
  fprintf(stdout, "\t-v version\n");
}

static int convert_to_html(char *s_url) {
  size_t filecontent_len;
  char *filecontent;
  char *htmlcontent;
  file_loader *m_file = new file_loader();
  std::string m_url = s_url;
  std::string url = m_file->find_fix_path(m_url);

  if (!m_file->exists(url)) {
    std::cout << "File not found: " << url << std::endl;
    return ENOENT;
  }
  if (!(filecontent = m_file->get_content(url))) {
    std::cout << "Filed to read: " << url << std::endl;
    return ENOENT;
  }

  filecontent_len = strlen(filecontent);

  if (!(htmlcontent = ldv_makemd(filecontent, filecontent_len))) {
    std::cout << "Markdown error: " << url << std::endl;
    free(filecontent);
    delete m_file;
    return ENOENT;
  }

  free(filecontent);
  delete m_file;

  std::cout << htmlcontent << "\n";
  free(htmlcontent);
  return 0;
}

static void ldv_set_external_theme(char *filename) {
  file_loader *m_file;

  if (!filename)
    return;

  m_file = new file_loader();

  if (m_file->exists(filename)) {
    ldv_config.external_theme = m_file->get_content(filename);
    ldv_config.theme = ldv_config.external_theme;
  }

  delete m_file;
}

static void ldv_set_userhome_theme() {
  file_loader *m_file = new file_loader();
  char *homedir = getenv("HOME");
  std::string m_str;

  if (homedir != NULL) {
    m_str.append(homedir);
    m_str.append("/");
    m_str.append(LTMD_USERTHEME_FILENAME);

    if (m_file->exists(m_str)) {
      ldv_set_external_theme((char *)m_str.c_str());
      delete m_file;
      return;
    }
  }

  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);

  if (pw == NULL) {
    delete m_file;
    return;
  }

  m_str.append(pw->pw_dir);
  m_str.append("/");
  m_str.append(LTMD_USERTHEME_FILENAME);

  if (m_file->exists(m_str)) {
    ldv_set_external_theme((char *)m_str.c_str());
  }

  delete m_file;
}

int main (int argc, char *argv[]) {
  int opt;
  uint8_t intarg;
  ldv_set_defaults();

  while ((opt = getopt(argc, argv, "adpvho:t:s:")) != -1) {
    switch (opt) {
      case 'a':
        ldv_config.print_links = true;
        break;
      case 't':
        intarg = atoi(optarg);
        switch(intarg) {
          case 1:
            ldv_config.theme = theme_light_css;
            break;
          case 2:
            ldv_config.theme = theme_dark_css;
            break;
          default:
            ldv_config.theme = theme_darker_css;
            break;
        }
        break;
      case 'p':
        if (argc > 1 && strlen(argv[argc-1]) > 0) {
          return convert_to_html(argv[argc -1]);
        }
        break;
      case 'h':
        usage(argv[0]);
        return 0;
      case 'd':
        ldv_config.debug = true;
        ldv_log("main() enable debug");
        break;
      case 's':
        ldv_set_external_theme(optarg);
        break;
      case 'v':
        version();
        return 0;
      default:
        usage(argv[0]);
        return 1;
    }
  }

  if (argc > 1 && strlen(argv[argc-1]) > 0) {
    ldv_config.url = strdup(argv[argc-1]);
  }

  if (!ldv_config.external_theme)
    ldv_set_userhome_theme();

  ldv_config.app = Gtk::Application::create("litemdview.browser");

  try {
    //load css
    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
  //  cssProvider->load_from_path("style.css");
    cssProvider->load_from_data(gtk_css);
    Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
    //get default screen
    Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
    //add provider for screen in all application
     styleContext->add_provider_for_screen(screen,
         cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  } catch(const Gtk::CssProviderError &error) {
    std::cerr << "Failed to load style:" << error.code() << std::endl;
  }

  litehtml::context html_context;
  html_context.load_master_stylesheet(master_css);
  browser_window win(&html_context);

  return ldv_config.app->run(win);
}

