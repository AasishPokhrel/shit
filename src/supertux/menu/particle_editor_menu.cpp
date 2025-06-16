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

#include "supertux/menu/particle_editor_menu.hpp"

#include "editor/particle_editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_filesystem.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/level.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "util/gettext.hpp"
#include "video/compositor.hpp"

ParticleEditorMenu::ParticleEditorMenu()
{
  add_label(_("Particle Editor"));

  add_hl();

  add_entry(MNID_RETURNTOEDITOR, _("Return to Editor"));
  add_entry(MNID_NEW, _("New Particle Config"));
  add_entry(MNID_SAVE, _("Save Particle Config"));
  add_entry(MNID_SAVE_AS, _("Save Particle Config as..."));
  add_entry(MNID_LOAD, _("Load Another Particle Config"));

  add_hl();

  add_entry(MNID_OPEN_DIR, _("Open Particle Directory"));
  add_entry(MNID_HELP, _("Keyboard Shortcuts"));

  add_hl();

  add_entry(MNID_QUITEDITOR, _("Exit Particle Editor"));
}

ParticleEditorMenu::~ParticleEditorMenu()
{
  auto editor = ParticleEditor::current();
  if (editor == nullptr) {
    return;
  }
  editor->reactivate();
}

void
ParticleEditorMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_RETURNTOEDITOR:
      MenuManager::instance().clear_menu_stack();
      break;

    case MNID_NEW:
    {
      MenuManager::instance().clear_menu_stack();
      auto editor = ParticleEditor::current();
      editor->check_unsaved_changes([editor] {
        editor->new_file();
      });
    }
      break;

    case MNID_SAVE:
    {
      MenuManager::instance().clear_menu_stack();
      auto editor = ParticleEditor::current();
      editor->request_save();
    }
      break;

    case MNID_SAVE_AS:
    {
      MenuManager::instance().clear_menu_stack();
      auto editor = ParticleEditor::current();
      editor->request_save(true);
    }
      break;

    case MNID_OPEN_DIR:
      ParticleEditor::current()->open_particle_directory();
      break;

    case MNID_LOAD:
      //MenuManager::instance().set_menu(MenuStorage::PARTICLE_EDITOR_OPEN);
    {
      const std::vector<std::string>& filter = {".stcp"};
      MenuManager::instance().push_menu(std::make_unique<FileSystemMenu>(
        &ParticleEditor::current()->m_filename,
        filter,
        "/particles",
        true,
        [](const std::string& new_filename) {
          ParticleEditor::current()->open("/particles/" +
                                        ParticleEditor::current()->m_filename);
          MenuManager::instance().clear_menu_stack();
        }
      ));
    }
      break;

    case MNID_HELP:
    {
      auto dialog = std::make_unique<Dialog>();
      dialog->set_text(_("Keyboard Shortcuts:\n---------------------\nEsc = Open Menu\nCtrl+S = Save\nCtrl+Shift+S = Save as\nCtrl+O = Open\nCtrl+Z = Undo\nCtrl+Y = Redo"));
      dialog->add_cancel_button(_("Got it!"));
      MenuManager::instance().set_dialog(std::move(dialog));
    }
    break;

    case MNID_QUITEDITOR:
      MenuManager::instance().clear_menu_stack();
      ParticleEditor::current()->m_quit_request = true;
      break;

    default:
      break;
  }
}

/* EOF */
