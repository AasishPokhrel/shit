//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#include "supertux/menu/editor_levelset_menu.hpp"

#include "editor/editor.hpp"
#include "gui/item_action.hpp"
#include "gui/menu_item.hpp"
#include "supertux/world.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

EditorLevelsetMenu::EditorLevelsetMenu(World* world_):
  m_world(world_),
  m_levelset_type()
{
  initialize();
}

EditorLevelsetMenu::~EditorLevelsetMenu()
{
  try
  {
    m_world->save();
  }
  catch(std::exception& e)
  {
    log_warning << "Could not save world: " << e.what() << std::endl;
  }
}

void
EditorLevelsetMenu::initialize()
{
  m_levelset_type = m_world->is_levelset() ? 1 : 0;

  add_label(_("World Settings"));
  add_hl();
  add_textfield(_("Name"), &m_world->m_title);
  add_textfield(_("Description"), &m_world->m_description);
  add_string_select(1, _("Type"), &m_levelset_type, {_("Worldmap"), _("Levelset")});
  add_file(_("Title Screen Level"), &m_world->m_title_level, { ".stl" }, m_world->m_basedir, false)
    .set_help(_("A level to be used for the title screen, after exiting the world."));
  add_hl();
  add_back(_("OK"));
}

void
EditorLevelsetMenu::menu_action(MenuItem& item)
{
  if (item.get_id() == 1)
    m_world->m_is_levelset = (m_levelset_type == 1);
}

/* EOF */
