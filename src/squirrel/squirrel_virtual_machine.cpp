//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "squirrel/squirrel_virtual_machine.hpp"

#include <cstring>
#include <stdarg.h>
#include <stdio.h>

#include "physfs/ifile_stream.hpp"
#include "squirrel/squirrel_scheduler.hpp"
#include "squirrel/squirrel_thread_queue.hpp"
#include "squirrel/squirrel_util.hpp"
#include "squirrel/supertux_api.hpp"
#include "supertux/console.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"

#ifdef ENABLE_SQDBG
#  include "../../external/squirrel/sqdbg/sqrdbg.h"
namespace {
HSQREMOTEDBG debugger = nullptr;
} // namespace
#endif

#ifdef __clang__
__attribute__((__format__ (__printf__, 2, 0)))
#endif
static void printFunc(HSQUIRRELVM, const char* fmt, ...)
{
  char buf[4096];
  const char separator[] = "\n";
  va_list arglist;
  va_start(arglist, fmt);
  vsnprintf(buf, sizeof(buf), fmt, arglist);
  const char* ptr = strtok(buf, separator);
  while (ptr != nullptr)
  {
    ConsoleBuffer::output << "[SCRIPTING] " << ptr << std::endl;
    ptr = strtok(nullptr, separator);
  }
  va_end(arglist);
}

static const char* DEFAULT_SCRIPT_FILE = "scripts/default.nut";

SquirrelVirtualMachine::SquirrelVirtualMachine(bool enable_debugger) :
  m_vm(64, ssq::Libs::BLOB | ssq::Libs::MATH | ssq::Libs::STRING),
  m_screenswitch_queue(),
  m_scheduler()
{
  m_vm.setForeignPtr(this);

  m_screenswitch_queue = std::make_unique<SquirrelThreadQueue>(m_vm);
  m_scheduler = std::make_unique<SquirrelScheduler>(m_vm);

  if (enable_debugger) {
#ifdef ENABLE_SQDBG
    sq_enabledebuginfo(m_vm.getHandle(), SQTrue);
    debugger = sq_rdbg_init(m_vm.getHandle(), 1234, SQFalse);
    if (debugger == nullptr)
      throw ssq::Exception(m_vm.getHandle(), "Couldn't initialize squirrel debugger");

    sq_enabledebuginfo(m_vm.getHandle(), SQTrue);
    log_info << "Waiting for debug client..." << std::endl;
    if (SQ_FAILED(sq_rdbg_waitforconnections(debugger)))
      throw ssq::Exception(m_vm.getHandle(), "Waiting for debug clients failed");
    log_info << "debug client connected." << std::endl;
#endif
  }

  // Set print function.
  m_vm.setPrintFunc(&printFunc, &printFunc);

  // Remove rand and srand calls from sqstdmath, we'll provide our own.
  m_vm.remove("srand");
  m_vm.remove("rand");

  // Register SuperTux API.
  register_supertux_scripting_api(m_vm);

  // Try to load the default script.
  try
  {
    IFileStream stream(DEFAULT_SCRIPT_FILE);
    m_vm.run(m_vm.compileSource(stream, DEFAULT_SCRIPT_FILE));
  }
  catch (const std::exception& err)
  {
    log_warning << "Couldn't load 'default.nut': " << err.what() << std::endl;
  }
}

SquirrelVirtualMachine::~SquirrelVirtualMachine()
{
#ifdef ENABLE_SQDBG
  if (debugger)
  {
    sq_rdbg_shutdown(debugger);
    debugger = nullptr;
  }
#endif
}

void
SquirrelVirtualMachine::update(float dt_sec)
{
  update_debugger();
  m_scheduler->update(g_game_time);
}

void
SquirrelVirtualMachine::update_debugger()
{
#ifdef ENABLE_SQDBG
  if (debugger)
    sq_rdbg_update(debugger);
#endif
}

SQInteger
SquirrelVirtualMachine::wait_for_seconds(HSQUIRRELVM vm, float seconds)
{
  return m_scheduler->schedule_thread(vm, g_game_time + seconds, false);
}

SQInteger
SquirrelVirtualMachine::skippable_wait_for_seconds(HSQUIRRELVM vm, float seconds)
{
  return m_scheduler->schedule_thread(vm, g_game_time + seconds, true);
}

SQInteger
SquirrelVirtualMachine::wait_for_screenswitch(HSQUIRRELVM vm)
{
  return m_screenswitch_queue->add(vm);
}

void
SquirrelVirtualMachine::wakeup_screenswitch()
{
  m_screenswitch_queue->wakeup();
}

/* EOF */
