//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#include "supertux/menu/particle_editor_open.hpp"

#include "editor/particle_editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/level.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "util/gettext.hpp"
#include "video/compositor.hpp"

ParticleEditorOpen::ParticleEditorOpen() :
  m_filename()
{
  add_label(_("Load particle file"));

  add_hl();

  std::vector<std::string> extensions;
  extensions.push_back("stcp");
  add_file(_("File"), &m_filename, extensions, "/particles/", true);
  add_entry(MNID_OPEN, _("Open"));

  add_hl();

  add_entry(MNID_CANCEL, _("Cancel"));
}

ParticleEditorOpen::~ParticleEditorOpen()
{
  auto editor = ParticleEditor::current();
  if (editor == nullptr) {
    return;
  }
  editor->reactivate();
}

void
ParticleEditorOpen::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_OPEN:
      std::replace(m_filename.begin(), m_filename.end(), '\\', '/');
      ParticleEditor::current()->open("/particles/" + m_filename);
      MenuManager::instance().clear_menu_stack();
      break;

    case MNID_CANCEL:
      MenuManager::instance().clear_menu_stack();
      break;

    default:
      break;
  }
}

/* EOF */
