//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_ERROR_HANDLER_HPP
#define HEADER_SUPERTUX_SUPERTUX_ERROR_HANDLER_HPP

#include <iostream>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace ErrorHandler {
  void set_handlers();

  std::string get_stacktrace();
  std::string get_system_info();

  void error_dialog_crash(const std::string& stacktrace);
  void error_dialog_exception(const std::string& exception = "");

#ifdef WIN32
  LONG WINAPI supertux_seh_handler(_In_ _EXCEPTION_POINTERS* ExceptionInfo);
  //CONTEXT* pcontext;
#else
  [[ noreturn ]] void handle_error(int sig);
#endif
  void report_error(const std::string& details);

  [[ noreturn ]] void close_program();
}

#endif

/* EOF */
