//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SQUIRREL_SQUIRREL_ENVIRONMENT_HPP
#define HEADER_SUPERTUX_SQUIRREL_SQUIRREL_ENVIRONMENT_HPP

#include <string>
#include <vector>

#include <simplesquirrel/vm.hpp>

#include "squirrel/exposable_class.hpp"
#include "squirrel/squirrel_scheduler.hpp"
#include "squirrel/squirrel_util.hpp"
#include "util/log.hpp"

/** The SquirrelEnvironment contains the environment in which a script
    is executed, meaning a root table containing objects and
    variables. */
class SquirrelEnvironment final
{
public:
  SquirrelEnvironment(ssq::VM& vm, const std::string& name);
  virtual ~SquirrelEnvironment();

public:
  inline ssq::VM& get_vm() const { return m_vm; }

  /** Expose this engine under 'name' */
  void expose_self();
  void unexpose_self();

  /** Expose and unexpose objects */
  void expose(ExposableClass& object, const std::string& name);
  void unexpose(const std::string& name);

  /** Convenience function that takes an std::string instead of an
      std::istream& */
  void run_script(const std::string& script, const std::string& sourcename);

  /** Runs a script in the context of the SquirrelEnvironment (m_table will
      be the roottable of this squirrel VM) and keeps a reference to
      the script so the script gets destroyed when the SquirrelEnvironment is
      destroyed). */
  void run_script(std::istream& in, const std::string& sourcename);

  void update(float dt_sec);
  SQInteger wait_for_seconds(HSQUIRRELVM vm, float seconds);
  SQInteger skippable_wait_for_seconds(HSQUIRRELVM vm, float seconds);

private:
  void garbage_collect();

private:
  ssq::VM& m_vm;
  ssq::Table m_table;
  std::string m_name;
  std::vector<ssq::VM> m_scripts;
  std::unique_ptr<SquirrelScheduler> m_scheduler;

private:
  SquirrelEnvironment(const SquirrelEnvironment&) = delete;
  SquirrelEnvironment& operator=(const SquirrelEnvironment&) = delete;
};

#endif

/* EOF */
