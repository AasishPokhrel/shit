//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

#include "interface/container.hpp"

InterfaceContainer::InterfaceContainer() :
  InterfaceControl(),
  m_children()
{
}

bool
InterfaceContainer::event(const SDL_Event& ev)
{
  for (auto& child : m_children)
    if (child.event(ev))
      return true;
  
  return false;
}

void
InterfaceContainer::update(float dt_sec)
{
  for (auto& child : m_children)
    child.update(dt_sec);
}

void
InterfaceContainer::draw(DrawingContext& context)
{
  context.push_transform();
  context.set_translation(context.get_translation() + m_rect.p1());
  context.set_viewport(Rect(m_rect));

  for (auto& child : m_children)
    child.draw(context);

  context.pop_transform();
}
