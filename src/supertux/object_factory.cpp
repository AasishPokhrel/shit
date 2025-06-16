//  SuperTux
//  Copyright (C) 2004 Ricardo Cruz <rick2@aeiou.pt>
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

#include "supertux/object_factory.hpp"

#include <sstream>

#include "supertux/game_object.hpp"

ObjectFactory::ObjectFactory() :
  factories(),
  m_badguys_names(),
  m_badguys_params(),
  m_objects_names(),
  m_objects_params(),
  m_adding_badguys(false)
{
}

std::unique_ptr<GameObject>
ObjectFactory::create(const std::string& name, const ReaderMapping& reader) const
{
  auto it = factories.find(name);

  if (it == factories.end())
  {
    std::stringstream msg;
    msg << "No factory for object '" << name << "' found.";
    throw std::runtime_error(msg.str());
  }
  else
  {
    return it->second.create(reader);
  }
}

std::string
ObjectFactory::get_display_name(const std::string& name) const
{
  auto it = factories.find(name);

  if (it == factories.end())
  {
    std::stringstream msg;
    msg << "No factory for object '" << name << "' found. Unable to get display name.";
    throw std::runtime_error(msg.str());
  }
  else
  {
    return it->second.get_display_name();
  }
}

bool
ObjectFactory::has_params(const std::string& name, uint8_t params)
{
  for (unsigned int i = 0; i < m_objects_names.size(); i++)
  {
    if (m_objects_names[i] == name)
      return m_objects_params[i] & params;
  }
  return false;
}

std::vector<std::string>
ObjectFactory::get_registered_badguys(uint8_t params)
{
  std::vector<std::string> out;
  for (unsigned int i = 0; i < m_badguys_names.size(); i++)
  {
    if (m_badguys_params[i] & params)
      out.push_back(m_badguys_names[i]);
  }
  return out;
}

std::vector<std::string>
ObjectFactory::get_registered_objects(uint8_t params)
{
  std::vector<std::string> out;
  for (unsigned int i = 0; i < m_objects_names.size(); i++)
  {
    if (m_objects_params[i] & params)
      out.push_back(m_objects_names[i]);
  }
  return out;
}

/* EOF */
