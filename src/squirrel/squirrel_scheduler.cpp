//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "squirrel/squirrel_scheduler.hpp"

#include <algorithm>

#include <simplesquirrel/exceptions.hpp>

#include "squirrel/squirrel_util.hpp"
#include "squirrel/squirrel_virtual_machine.hpp"
#include "supertux/level.hpp"
#include "util/log.hpp"

SquirrelScheduler::SquirrelScheduler(ssq::VM& vm) :
  m_vm(vm),
  schedule()
{
}

void
SquirrelScheduler::update(float time)
{
  while (!schedule.empty() &&
         (schedule.front().wakeup_time < time ||
          (schedule.front().skippable && Level::current() &&
           Level::current()->m_skip_cutscene)))
  {
    HSQOBJECT thread_ref = schedule.front().thread_ref;

    sq_pushobject(m_vm.getHandle(), thread_ref);
    sq_getweakrefval(m_vm.getHandle(), -1);

    HSQUIRRELVM scheduled_vm;
    if (sq_gettype(m_vm.getHandle(), -1) == OT_THREAD &&
       SQ_SUCCEEDED(sq_getthread(m_vm.getHandle(), -1, &scheduled_vm))) {
      if (SQ_FAILED(sq_wakeupvm(scheduled_vm, SQFalse, SQFalse, SQTrue, SQFalse))) {
        std::ostringstream msg;
        msg << "Error waking VM: ";
        sq_getlasterror(scheduled_vm);
        if (sq_gettype(scheduled_vm, -1) != OT_STRING) {
          msg << "(no info)";
        } else {
          const char* lasterr;
          sq_getstring(scheduled_vm, -1, &lasterr);
          msg << lasterr;
        }
        log_warning << msg.str() << std::endl;
        sq_pop(scheduled_vm, 1);
      }
    }

    sq_release(m_vm.getHandle(), &thread_ref);
    sq_pop(m_vm.getHandle(), 2);

    std::pop_heap(schedule.begin(), schedule.end());
    schedule.pop_back();
  }
}

SQInteger
SquirrelScheduler::schedule_thread(HSQUIRRELVM scheduled_vm, float time, bool skippable)
{
  // create a weakref to the VM
  sq_pushthread(m_vm.getHandle(), scheduled_vm);
  sq_weakref(m_vm.getHandle(), -1);

  ScheduleEntry entry;
  if (SQ_FAILED(sq_getstackobj(m_vm.getHandle(), -1, & entry.thread_ref))) {
    sq_pop(m_vm.getHandle(), 2);
    throw ssq::Exception(m_vm.getHandle(), "Couldn't get thread weakref from vm");
  }
  entry.wakeup_time = time;
  entry.skippable = skippable;

  sq_addref(m_vm.getHandle(), & entry.thread_ref);
  sq_pop(m_vm.getHandle(), 2);

  schedule.push_back(entry);
  std::push_heap(schedule.begin(), schedule.end());

  return sq_suspendvm(scheduled_vm);
}

/* EOF */
