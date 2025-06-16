//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#include "supertux/sector_base.hpp"

#include "squirrel/squirrel_virtual_machine.hpp"
#include "util/log.hpp"

namespace Base {

Sector::Sector(const std::string& type) :
  m_name(),
  m_init_script(),
  m_squirrel_environment(new SquirrelEnvironment(SquirrelVirtualMachine::current()->get_vm(), type))
{
}

void
Sector::finish_construction(bool)
{
  for (auto& object : get_objects())
    object->finish_construction();
}

void
Sector::run_script(const std::string& script, const std::string& sourcename)
{
  m_squirrel_environment->run_script(script, sourcename);
}

bool
Sector::before_object_add(GameObject& object)
{
  m_squirrel_environment->expose(object, object.get_name());
  return true;
}

void
Sector::before_object_remove(GameObject& object)
{
  m_squirrel_environment->unexpose(object.get_name());
}

} // namespace Base

/* EOF */
