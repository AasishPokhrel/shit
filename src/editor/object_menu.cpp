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

#include "editor/object_menu.hpp"

#include <fmt/format.h>

#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/sector.hpp"
#include "supertux/game_object.hpp"
#include "supertux/moving_object.hpp"

ObjectMenu::ObjectMenu(GameObject* go, const std::function<bool (GameObject*)>& remove_function) :
  m_editor(*Editor::current()),
  m_object(go),
  m_remove_function(remove_function)
{
  m_object->save_state();

  refresh();
}

void
ObjectMenu::refresh()
{
  clear();

  ObjectSettings os = m_object->get_settings();
  add_label(os.get_name());
  add_hl();
  for (const auto& oo_ptr : os.get_options())
  {
    const auto& oo = *oo_ptr;

    if (!(oo.get_flags() & OPTION_HIDDEN)) {
      oo.add_to_menu(*this);
    }
  }

  if (!m_object->is_up_to_date())
  {
    add_hl();
    add_entry(MNID_PATCH_NOTES, _("Patch Notes"));
    add_entry(MNID_UPDATE, _("Update"));
  }

  if (m_remove_function)
  {
    add_hl();
    add_entry(MNID_REMOVEFUNCTION, _("Remove"));
  }

  add_hl();
  add_back(_("OK"), -1);
}

ObjectMenu::~ObjectMenu()
{
}

void
ObjectMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_UPDATE:
      Dialog::show_confirmation(_("This will update the object to its latest functionality.\nCheck the \"Patch Notes\" for more information.\n\nKeep in mind this is very likely to break the proper behaviour of the object.\nMake sure to re-check any behaviour, related to the object."), [this]() {
        m_object->update_version();
        refresh();
      });
      break;

    case MNID_PATCH_NOTES:
    {
      const std::vector<std::string> patches = m_object->get_patches();
      std::string text;

      for (int version = m_object->get_version() + 1;
           version <= 1 + static_cast<int>(patches.size()); version++)
      {
        text += fmt::format(fmt::runtime(_("Patch notes for v{}:")), version) + "\n" + patches[version - 2];
        if (version < 1 + static_cast<int>(patches.size()))
          text += "\n\n";
      }

      Dialog::show_message(text);
      refresh();
      break;
    }

    case MNID_REMOVE:
      m_editor.delete_markers();
      m_editor.m_reactivate_request = true;
      m_object->remove_me();
      MenuManager::instance().pop_menu();
      break;

    case MNID_REMOVEFUNCTION:
      if (m_remove_function(m_object))
        MenuManager::instance().pop_menu();
      break;

    case MNID_TEST_FROM_HERE: {
      const MovingObject* obj = static_cast<const MovingObject*>(m_object);
      m_editor.m_test_pos = std::make_pair(m_editor.get_sector()->get_name(),
                                           obj->get_pos());
      m_editor.m_test_request = true;
      MenuManager::instance().pop_menu();
      break;
    }

    case MNID_OPEN_PARTICLE_EDITOR:
      m_editor.m_particle_editor_request = true;
      MenuManager::instance().pop_menu();
      break;

    default:
      break;
  }
}

bool
ObjectMenu::on_back_action()
{
  // FIXME: this is a bit fishy, menus shouldn't mess with editor internals
  BIND_SECTOR(*m_editor.get_sector());

  m_object->after_editor_set();
  m_object->check_state();

  if (!MenuManager::instance().previous_menu())
  {
    m_editor.m_reactivate_request = true;
    if (!dynamic_cast<MovingObject*>(m_object)) {
      m_editor.sort_layers();
    }
  }

  return true;
}

/* EOF */
