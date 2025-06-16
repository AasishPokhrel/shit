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

#include "gui/menu_paths.hpp"

#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "object/path_gameobject.hpp"
#include "object/path_object.hpp"
#include "supertux/sector.hpp"

#include <fmt/format.h>

auto on_select = [](const std::string& path, PathObject& target, const std::string& path_ref) {
  return [path, &target, path_ref] {
    auto dialog = std::make_unique<Dialog>();
    dialog->add_default_button(_("Clone"), [path, path_ref] {
      auto* from = Editor::current()->get_sector()->get_object_by_name<PathGameObject>(path);
      auto* into = Editor::current()->get_sector()->get_object_by_name<PathGameObject>(path_ref);
      if (from && into) {
        from->copy_into(*into);
        MenuManager::instance().pop_menu();
      } else {
        log_warning << "Could not copy path, misses " << (from ? "" : "'from'")
                    << (into ? "" : "'into'") << std::endl;
        Dialog::show_message(_("An error occurred and the game could\nnot clone the path. Please contact\nthe developers for support."));
      }
    });
    dialog->add_button(_("Bind"), [path, &target] {
      target.editor_set_path_by_ref(path);
        MenuManager::instance().pop_menu();
    });
    dialog->add_cancel_button(_("Cancel"));
    dialog->set_text(_("Do you wish to clone the path to edit it separately,\nor do you want to bind both paths together\nso that any edit on one edits the other?"));
    MenuManager::instance().set_dialog(std::move(dialog));
  };
};

PathsMenu::PathsMenu(PathObject& target, const std::string& path_ref)
{
  add_label(fmt::format(fmt::runtime(_("Path {}")), path_ref));
  add_hl();

  const auto paths = Editor::current()->get_sector()->get_objects_by_type<PathGameObject>();

  for (const auto& path : paths)
  {
    add_entry(path.get_name(), on_select(path.get_name(), target, path_ref));
  }

  add_hl();
  add_back(_("Back"));
}

/* EOF */
