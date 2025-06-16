//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_MENU_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_MENU_HPP

#include "gui/menu.hpp"

#include <functional>

class Editor;
class GameObject;

class ObjectMenu final : public Menu
{
public:
  enum {
    MNID_UPDATE,
    MNID_PATCH_NOTES,
    MNID_REMOVE,
    MNID_REMOVEFUNCTION,
    MNID_TEST_FROM_HERE,
    MNID_OPEN_PARTICLE_EDITOR
  };

public:
  ObjectMenu(GameObject* go, const std::function<bool (GameObject*)>& remove_function = {});
  ~ObjectMenu() override;

  void refresh() override;

  virtual void menu_action(MenuItem& item) override;
  virtual bool on_back_action() override;

private:
  Editor& m_editor;
  GameObject* m_object;
  const std::function<bool (GameObject*)> m_remove_function;

private:
  ObjectMenu(const ObjectMenu&) = delete;
  ObjectMenu& operator=(const ObjectMenu&) = delete;
};

#endif

/* EOF */
