//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
//                2007 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/menu/keyboard_menu.hpp"

#include "control/input_manager.hpp"
#include "control/keyboard_manager.hpp"
#include "gui/item_controlfield.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/gettext.hpp"

#include <fmt/format.h>

KeyboardMenu::KeyboardMenu(InputManager& input_manager, int player_id) :
  m_input_manager(input_manager),
  m_player_id(player_id)
{
  add_label(_("Setup Keyboard"));
  add_hl();
  add_controlfield(static_cast<int>(Control::UP),         _("Up"));
  add_controlfield(static_cast<int>(Control::DOWN),       _("Down"));
  add_controlfield(static_cast<int>(Control::LEFT),       _("Left"));
  add_controlfield(static_cast<int>(Control::RIGHT),      _("Right"));
  add_controlfield(static_cast<int>(Control::JUMP),       _("Jump"));
  add_controlfield(static_cast<int>(Control::ACTION),     _("Action"));
  add_controlfield(static_cast<int>(Control::ITEM),       _("Item Pocket"));

  add_controlfield(static_cast<int>(Control::PEEK_LEFT),  _("Peek Left"));
  add_controlfield(static_cast<int>(Control::PEEK_RIGHT), _("Peek Right"));
  add_controlfield(static_cast<int>(Control::PEEK_UP),    _("Peek Up"));
  add_controlfield(static_cast<int>(Control::PEEK_DOWN),  _("Peek Down"));

  if (m_player_id == 0)
  {
    if (g_config->developer_mode) {
      add_controlfield(static_cast<int>(Control::CONSOLE), _("Console"));
      add_controlfield(static_cast<int>(Control::CHEAT_MENU), _("Cheat Menu"));
      add_controlfield(static_cast<int>(Control::DEBUG_MENU), _("Debug Menu"));
    }
  }
  add_toggle(static_cast<int>(Control::CONTROLCOUNT), _("Jump with Up"), &g_config->keyboard_config.m_jump_with_up_kbd);
  add_hl();

  if (m_player_id == 0)
  {
    for (int id = 1; id < m_input_manager.get_num_users(); id++)
    {
      add_entry(fmt::format(fmt::runtime(_("Player {}")), std::to_string(id + 1)),
      [&input_manager, id] {
        MenuManager::instance().push_menu(std::make_unique<KeyboardMenu>(input_manager, id));
      });
    }
    add_hl();
  }

  add_back(_("Back"));
  refresh();
}

std::string
KeyboardMenu::get_key_name(SDL_Keycode key) const
{
  switch (key) {
    case SDLK_UNKNOWN:
      return _("None");
    case SDLK_UP:
      return _("Up cursor");
    case SDLK_DOWN:
      return _("Down cursor");
    case SDLK_LEFT:
      return _("Left cursor");
    case SDLK_RIGHT:
      return _("Right cursor");
    case SDLK_RETURN:
      return _("Return");
    case SDLK_SPACE:
      return _("Space");
    case SDLK_RSHIFT:
      return _("Right Shift");
    case SDLK_LSHIFT:
      return _("Left Shift");
    case SDLK_RCTRL:
      return _("Right Control");
    case SDLK_LCTRL:
      return _("Left Control");
    case SDLK_RALT:
      return _("Right Alt");
    case SDLK_LALT:
      return _("Left Alt");
    case SDLK_RGUI:
      return _("Right Command");
    case SDLK_LGUI:
      return _("Left Command");
    default:
      return SDL_GetKeyName(static_cast<SDL_Keycode>(key));
  }
}

void
KeyboardMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0 && item.get_id() < static_cast<int>(Control::CONTROLCOUNT))
  {
    ItemControlField& field = static_cast<ItemControlField&>(item);
    field.change_input(_("Press Key"));
    m_input_manager.keyboard_manager->bind_next_event_to(m_player_id, static_cast<Control>(item.get_id()));
  }
}

void
KeyboardMenu::refresh()
{
  const auto& controls = { Control::UP, Control::DOWN, Control::LEFT, Control::RIGHT,
                           Control::JUMP, Control::ACTION, Control::ITEM,
                           Control::PEEK_LEFT, Control::PEEK_RIGHT,
                           Control::PEEK_UP, Control::PEEK_DOWN };

  const auto& developer_controls = { Control::CHEAT_MENU, Control::DEBUG_MENU, Control::CONSOLE };

  for(const auto& control : controls)
  {
    refresh_control(control);
  }

  if (g_config->developer_mode && m_player_id == 0)
  {
    for(const auto& control: developer_controls)
    {
      refresh_control(control);
    }
  }
}

void
KeyboardMenu::refresh_control(const Control& control)
{
  ItemControlField& field = static_cast<ItemControlField&>(get_item_by_id(static_cast<int>(control)));
  field.change_input(get_key_name(g_config->keyboard_config.reversemap_key(m_player_id, control)));
}

/* EOF */
