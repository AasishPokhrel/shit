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

#ifndef HEADER_SUPERTUX_SQUIRREL_SQUIRREL_VIRTUAL_MACHINE_HPP
#define HEADER_SUPERTUX_SQUIRREL_SQUIRREL_VIRTUAL_MACHINE_HPP

#include <memory>

#include <simplesquirrel/vm.hpp>

#include "util/currenton.hpp"

class SquirrelThreadQueue;
class SquirrelScheduler;

class SquirrelVirtualMachine final : public Currenton<SquirrelVirtualMachine>
{
public:
  SquirrelVirtualMachine(bool enable_debugger);
  ~SquirrelVirtualMachine() override;

  inline ssq::VM& get_vm() { return m_vm; }

  SQInteger wait_for_seconds(HSQUIRRELVM vm, float seconds);
  SQInteger skippable_wait_for_seconds(HSQUIRRELVM vm, float seconds);
  void update(float dt_sec);

  /** adds thread waiting for a screen switch event */
  SQInteger wait_for_screenswitch(HSQUIRRELVM vm);

  /** wakes up threads waiting for a screen switch event */
  void wakeup_screenswitch();

private:
  void update_debugger();

private:
  ssq::VM m_vm;

  std::unique_ptr<SquirrelThreadQueue> m_screenswitch_queue;
  std::unique_ptr<SquirrelScheduler> m_scheduler;

private:
  SquirrelVirtualMachine(const SquirrelVirtualMachine&) = delete;
  SquirrelVirtualMachine& operator=(const SquirrelVirtualMachine&) = delete;
};

#endif

/* EOF */
