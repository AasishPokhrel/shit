//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//                2023 Vankata453
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

#include "gui/menu_object_select.hpp"

#include <fmt/format.h>

#include "editor/object_menu.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_list.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/game_object.hpp"
#include "supertux/game_object_factory.hpp"
#include "util/log.hpp"

ObjectSelectMenu::ObjectSelectMenu(std::vector<std::unique_ptr<GameObject>>& objects, uint8_t get_objects_param,
                                   const std::function<void (std::unique_ptr<GameObject>)>& add_object_func) :
  m_objects(objects),
  m_get_objects_param(get_objects_param),
  m_add_object_function(add_object_func),
  m_selected()
{
  refresh();
}

void
ObjectSelectMenu::refresh()
{
  clear();

  add_label(_("List of objects"));
  add_hl();
  add_entry(-2, fmt::format(fmt::runtime(_("Select object ({})")),
      m_selected.empty() ? "" : GameObjectFactory::instance().get_display_name(m_selected)));
  add_entry(-3, _("Add"));
  add_hl();

  int i = 0;
  for (auto& obj : m_objects)
  {
    add_entry(i, obj->get_display_name());
    i++;
  }

  add_hl();
  add_back(_("OK"));
}

void
ObjectSelectMenu::add_object()
{
  if (m_selected.empty())
  {
    log_warning << "Cannot add object: No object selected." << std::endl;
    return;
  }

  auto obj = GameObjectFactory::instance().create(m_selected);
  obj->update_version(); // Ensure the object is on its latest version

  if (m_add_object_function)
  {
    try
    {
      m_add_object_function(std::move(obj));
    }
    catch (const std::exception& err)
    {
      Dialog::show_message(err.what());
    }
  }
  else
  {
    m_objects.push_back(std::move(obj));
  }

  refresh();
}

void
ObjectSelectMenu::remove_object(GameObject* obj)
{
  m_objects.erase(
    std::remove_if(m_objects.begin(), m_objects.end(), [obj](const auto& found_obj)
    {
      return obj == found_obj.get();
    }), m_objects.end());

  refresh();
}

void
ObjectSelectMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0)
  {
    MenuManager::instance().push_menu(std::make_unique<ObjectMenu>(m_objects[item.get_id()].get(),
        [this](GameObject* obj) {
          Dialog::show_confirmation(_("Are you sure you want to remove this object from the list?"), [this, obj]() {
            remove_object(obj);
            MenuManager::instance().pop_menu();
          });
          return false;
        }));
  }
  else if (item.get_id() == -2)
  {
    MenuManager::instance().push_menu(std::make_unique<ListMenu>(
        GameObjectFactory::instance().get_registered_objects(m_get_objects_param),
        &m_selected, this, [](const std::string& obj_name) {
          return GameObjectFactory::instance().get_display_name(obj_name);
        }));
  }
  else if (item.get_id() == -3)
  {
    add_object();
  }
}

/* EOF */
