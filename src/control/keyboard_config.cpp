//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "control/keyboard_config.hpp"

#include <optional>

#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

/** Delimiter used for config save/load between the control and the player id */
static constexpr const char DELIMITER = ':';

KeyboardConfig::KeyboardConfig() :
  m_keymap(),
  m_configurable_controls({
    Control::UP,
    Control::DOWN,
    Control::LEFT,
    Control::RIGHT,
    Control::JUMP,
    Control::ACTION,
    Control::ITEM,
    Control::PEEK_LEFT,
    Control::PEEK_RIGHT,
    Control::PEEK_UP,
    Control::PEEK_DOWN,
    Control::CONSOLE,
    Control::CHEAT_MENU,
    Control::DEBUG_MENU
  }),
  m_jump_with_up_kbd(false)
{
  // initialize default keyboard map
  m_keymap[SDLK_LEFT]      = {0, Control::LEFT};
  m_keymap[SDLK_RIGHT]     = {0, Control::RIGHT};
  m_keymap[SDLK_UP]        = {0, Control::UP};
  m_keymap[SDLK_DOWN]      = {0, Control::DOWN};
  m_keymap[SDLK_SPACE]     = {0, Control::JUMP};
  m_keymap[SDLK_LCTRL]     = {0, Control::ACTION};
  m_keymap[SDLK_LSHIFT]    = {0, Control::ITEM};
  m_keymap[SDLK_ESCAPE]    = {0, Control::ESCAPE};
  m_keymap[SDLK_p]         = {0, Control::START};
  m_keymap[SDLK_PAUSE]     = {0, Control::START};
  m_keymap[SDLK_RETURN]    = {0, Control::MENU_SELECT};
  m_keymap[SDLK_KP_ENTER]  = {0, Control::MENU_SELECT};
  m_keymap[SDLK_CARET]     = {0, Control::CONSOLE};
  m_keymap[SDLK_DELETE]    = {0, Control::PEEK_LEFT};
  m_keymap[SDLK_PAGEDOWN]  = {0, Control::PEEK_RIGHT};
  m_keymap[SDLK_HOME]      = {0, Control::PEEK_UP};
  m_keymap[SDLK_END]       = {0, Control::PEEK_DOWN};
  m_keymap[SDLK_F1]        = {0, Control::CHEAT_MENU};
  m_keymap[SDLK_F2]        = {0, Control::DEBUG_MENU};
  m_keymap[SDLK_BACKSPACE] = {0, Control::REMOVE};
}

void
KeyboardConfig::read(const ReaderMapping& keymap_mapping)
{
  // backwards compatibility:
  // keycode values changed between SDL1 and SDL2, so we skip old SDL1
  // based values and use the defaults instead on the first read of
  // the config file
  bool config_is_sdl2 = false;
  keymap_mapping.get("sdl2", config_is_sdl2);
  if (!config_is_sdl2)
    return;

  keymap_mapping.get("jump-with-up", m_jump_with_up_kbd);

  auto iter = keymap_mapping.get_iter();
  while (iter.next())
  {
    if (iter.get_key() != "map")
      continue;
    int key = -1;
    auto map = iter.as_mapping();
    map.get("key", key);

    std::string control_text;
    map.get("control", control_text);

    int player_id = 0;
    size_t pos = control_text.find(DELIMITER);
    if (pos != std::string::npos)
    {
      try
      {
        player_id = std::stoi(control_text.substr(0, pos));
      }
      catch (const std::exception&)
      {
        log_warning << "Could not parse player ID '" << control_text.substr(0, pos) << "' to number" << std::endl;
      }
      control_text = control_text.substr(pos + 1);
    }

    const std::optional<Control> maybe_control = Control_from_string(control_text);
    if (maybe_control) {
      if (m_configurable_controls.count(*maybe_control)) {
        bind_key(static_cast<SDL_Keycode>(key), player_id, *maybe_control);
      }
    } else {
      log_warning << "Invalid control '" << control_text << "' in keymap" << std::endl;
    }
  }
}

void
KeyboardConfig::bind_key(SDL_Keycode key, int player, Control c)
{
  // remove all previous mappings for that control and for that key
  for (auto i = m_keymap.begin(); i != m_keymap.end(); /* no ++i */)
  {
    if (i->second == PlayerControl{player, c})
    {
      auto e = i;
      ++i;
      m_keymap.erase(e);
    }
    else
    {
      ++i;
    }
  }

  auto i = m_keymap.find(key);
  if (i != m_keymap.end())
    m_keymap.erase(i);

  // add new mapping
  m_keymap[key] = PlayerControl{player, c};
}

SDL_Keycode
KeyboardConfig::reversemap_key(int player, Control c) const
{
  for (const auto& i : m_keymap)
  {
    if (i.second == PlayerControl{player, c})
    {
      return i.first;
    }
  }

  return SDLK_UNKNOWN;
}

void
KeyboardConfig::write(Writer& writer)
{
  // this flag handles the transition from SDL1 to SDL2, as keycodes
  // are incompatible between the two, if it's not set an old SDL1
  // config file is assumed and controls are reset to default
  writer.write("sdl2", true);

  writer.write("jump-with-up", m_jump_with_up_kbd);

  for (const auto& i : m_keymap)
  {
    std::string player_prefix = (i.second.player > 0) ? std::to_string(i.second.player) + DELIMITER : "";
    writer.start_list("map");
    writer.write("key", static_cast<int>(i.first));
    writer.write("control", player_prefix + Control_to_string(i.second.control));
    writer.end_list("map");
  }
}

/* EOF */
