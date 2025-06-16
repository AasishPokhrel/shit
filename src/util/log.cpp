//  SuperTux Debug Helper Functions
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "util/log.hpp"

#include <iostream>
#ifdef __ANDROID__
#include <android/log.h>
#endif

#include "math/rectf.hpp"
#include "supertux/console.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"

#ifdef __ANDROID__
// To print only SuperTux logs in Android logcat output, use command:
// adb logcat -s 'SuperTux:V' 'SDL:V' 'DEBUG:V'
class _android_debugbuf: public std::streambuf
{
  public:
  _android_debugbuf()
  {
    pos = 0;
    buf[0] = 0;
  }

  protected:
  virtual int overflow(int c = EOF)
  {
    if (EOF == c)
    {
      return '\0';  // returning EOF indicates an error
    }
    else
    {
      outputchar(c);
      return c;
    }
  }
  // we don’t do input so always return EOF
  virtual int uflow()
  {
    return EOF;
  }

  // we don’t do input so always return 0 chars read
  virtual int xsgetn(char *, int)
  {
    return 0;
  }

  // Calls outputchar() for each character.
  virtual int xsputn(const char *s, int n)
  {
    for (int i = 0; i < n; ++i)
    {
      outputchar(s[i]);
    }
    return n;// we always process all of the chars
  }

  private:

  char buf[512];
  int pos;

  void outputchar(char c)
  {
    // TODO: mutex
    buf[pos] = c;
    pos++;
    if (pos >= sizeof(buf) - 1 || c == '\n' || c == '\r' || c == 0)
    {
      buf[pos] = 0;
      __android_log_print(ANDROID_LOG_INFO, "SuperTux", "%s", buf);
      pos = 0;
      buf[pos] = 0;
    }
  }
};

static std::ostream android_logcat(new _android_debugbuf());
#endif

LogLevel g_log_level = LOG_WARNING;

std::ostream& get_logging_instance(bool use_console_buffer)
{
  if (ConsoleBuffer::current() && use_console_buffer)
    return (ConsoleBuffer::output);
  else
#ifdef __ANDROID__
    return android_logcat;
#else
    return (std::cerr);
#endif
}

static std::ostream& log_generic_f (const char *prefix, const char* file, int line, bool use_console_buffer = true)
{
  get_logging_instance (use_console_buffer) << prefix << " " << file << ":" << line << " ";
  return (get_logging_instance (use_console_buffer));
}

std::ostream& log_debug_f(const char* file, int line, bool use_console_buffer = true)
{
  return (log_generic_f ("[DEBUG]", file, line, use_console_buffer));
}

std::ostream& log_info_f(const char* file, int line)
{
  return (log_generic_f ("[INFO]", file, line));
}

std::ostream& log_warning_f(const char* file, int line)
{
  if (g_config && g_config->developer_mode &&
     Console::current() && !Console::current()->hasFocus()) {
    Console::current()->open();
  }
  return (log_generic_f ("[WARNING]", file, line));
}

std::ostream& log_fatal_f(const char* file, int line)
{
  if (g_config && g_config->developer_mode &&
     Console::current() && !Console::current()->hasFocus()) {
    Console::current()->open();
  }
  return (log_generic_f ("[FATAL]", file, line));
}

/* Callbacks used by tinygettext */
void log_info_callback(const std::string& str)
{
    log_info << "\r\n[TINYGETTEXT] " << str << std::endl;
}

void log_warning_callback(const std::string& str)
{
    log_debug << "\r\n[TINYGETTEXT] " << str << std::endl;
}

void log_error_callback(const std::string& str)
{
    log_warning << "\r\n[TINYGETTEXT] " << str << std::endl;
}

/* EOF */
