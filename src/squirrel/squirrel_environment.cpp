//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "squirrel/squirrel_environment.hpp"

#include <algorithm>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "squirrel/squirrel_util.hpp"
#include "squirrel/squirrel_virtual_machine.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"

SquirrelEnvironment::SquirrelEnvironment(ssq::VM& vm, const std::string& name) :
  m_vm(vm),
  m_table(m_vm.newTable()),
  m_name(name),
  m_scripts(),
  m_scheduler(std::make_unique<SquirrelScheduler>(m_vm))
{
  // Set the root table as delegate.
  m_table.setDelegate(m_vm);
}

SquirrelEnvironment::~SquirrelEnvironment()
{
  m_scripts.clear();
  m_table.reset();
}

void
SquirrelEnvironment::expose_self()
{
  m_vm.set(m_name.c_str(), m_table);
}

void
SquirrelEnvironment::unexpose_self()
{
  m_vm.remove(m_name.c_str());
}

void
SquirrelEnvironment::expose(ExposableClass& object, const std::string& name)
{
  if (name.empty()) return;

  const std::string class_name = object.get_exposed_class_name();
  assert(!class_name.empty());

  try
  {
    ssq::Class sq_class = m_vm.findClass(class_name.c_str());
    m_vm.newInstancePtr(m_table, sq_class, name.c_str(), &object);
  }
  catch (const std::exception& err)
  {
    log_warning << "Couldn't expose object of class '" << class_name << "': " << err.what() << std::endl;
  }
}

void
SquirrelEnvironment::unexpose(const std::string& name)
{
  if (name.empty()) return;

  try
  {
    m_table.remove(name.c_str());
  }
  catch (const std::exception& err)
  {
    log_warning << "Couldn't unregister object: " << err.what() << std::endl;
  }
}

void
SquirrelEnvironment::run_script(const std::string& script, const std::string& sourcename)
{
  if (script.empty()) return;

  std::istringstream stream(script);
  run_script(stream, sourcename);
}

void
SquirrelEnvironment::garbage_collect()
{
  m_scripts.erase(
    std::remove_if(m_scripts.begin(), m_scripts.end(),
                   [](ssq::VM& thread) {
                     return thread.getState() != SQ_VMSTATE_SUSPENDED;
                   }),
    m_scripts.end());
}

void
SquirrelEnvironment::run_script(std::istream& in, const std::string& sourcename)
{
  garbage_collect();

  try
  {
    ssq::VM thread = m_vm.newThread(64);
    thread.setForeignPtr(this);
    thread.setRootTable(m_table);

    thread.run(thread.compileSource(in, sourcename.c_str()), true);

    m_scripts.push_back(std::move(thread));
  }
  catch (const std::exception& err)
  {
    log_warning << err.what() << std::endl;
  }
}

SQInteger
SquirrelEnvironment::wait_for_seconds(HSQUIRRELVM vm, float seconds)
{
  return m_scheduler->schedule_thread(vm, g_game_time + seconds, false);
}

SQInteger
SquirrelEnvironment::skippable_wait_for_seconds(HSQUIRRELVM vm, float seconds)
{
  return m_scheduler->schedule_thread(vm, g_game_time + seconds, true);
}

void
SquirrelEnvironment::update(float dt_sec)
{
  m_scheduler->update(g_game_time);
}

/* EOF */
