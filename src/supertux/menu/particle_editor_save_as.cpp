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

#include "supertux/menu/particle_editor_save_as.hpp"

#include "editor/particle_editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/level.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "util/gettext.hpp"
#include "video/compositor.hpp"

ParticleEditorSaveAs::ParticleEditorSaveAs(std::function<void(bool)> callback) :
  m_filename("/particles/custom/"),
  m_callback(std::move(callback))
{
  add_label(_("Save particle as"));

  add_hl();

  add_textfield(_("File name"), &m_filename);
  add_entry(MNID_SAVE, _("Save"));

  add_hl();

  add_entry(MNID_CANCEL, _("Cancel"));
}

ParticleEditorSaveAs::~ParticleEditorSaveAs()
{
  auto editor = ParticleEditor::current();
  if (editor == nullptr) {
    return;
  }
  editor->reactivate();
}

void
ParticleEditorSaveAs::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_SAVE:
      ParticleEditor::current()->save(m_filename);
      // In this very case, if you clear the dialog stack before calling the
      // callback, the callback will lose its reference to the Particle Editor,
      // which will cause a segfault. Somehow. Somebody explain me.  ~Semphris
      // -----------------------------------------------------------------------
      // EDIT: It's because the menu stack is a vector of *unique pointers*, so
      // when the stack is cleared, so is the menu (`this`), and so is the
      // callback; so the callback becomes a dangling pointer and calling it is
      // undefined behavior. Leaving this here for maintainers.      ~Semphris

      m_callback(true);
      MenuManager::instance().clear_menu_stack();
      break;

    case MNID_CANCEL:
      m_callback(false);
      MenuManager::instance().clear_menu_stack();
      break;

    default:
      break;
  }
}

/* EOF */
