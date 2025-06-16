//  SuperTux
//  Copyright (C) 2004 Tobas Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_OPTIONS_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_OPTIONS_MENU_HPP

#include "gui/menu.hpp"

class OptionsMenu final : public Menu
{
private:
  static bool less_than_volume(const std::string& lhs, const std::string& rhs);

public:
  enum Type {
    LOCALE,
    VIDEO,
    AUDIO,
    CONTROLS,
    EXTRAS,
    ADVANCED
  };

public:
  OptionsMenu(Type type, bool complete);
  ~OptionsMenu() override;

  void refresh() override;
  void on_window_resize() override;

  void menu_action(MenuItem& item) override;

private:
  void insert_label(const std::string& text);

  void add_magnification();
  void add_aspect_ratio();
  void add_window_resolutions();
  void add_resolutions();
  void add_vsync();
  void add_sound_volume();
  void add_music_volume();
  void add_flash_intensity();
  void add_mobile_control_scales();

private:
  enum MenuIDs {
    MNID_WINDOW_RESIZABLE,
    MNID_WINDOW_RESOLUTION,
    MNID_FULLSCREEN,
    MNID_FULLSCREEN_RESOLUTION,
    MNID_FIT_WINDOW,
    MNID_MAGNIFICATION,
    MNID_ASPECTRATIO,
    MNID_VSYNC,
    MNID_FRAME_PREDICTION,
    MNID_SOUND,
    MNID_MUSIC,
    MNID_SOUND_VOLUME,
    MNID_MUSIC_VOLUME,
    MNID_FLASH_INTENSITY,
    MNID_RUMBLING,
    MNID_DEVELOPER_MODE,
    MNID_CHRISTMAS_MODE,
    MNID_TRANSITIONS,
    MNID_CUSTOM_TITLE_LEVELS,
    MNID_CONFIRMATION_DIALOG,
    MNID_PAUSE_ON_FOCUSLOSS,
    MNID_CUSTOM_CURSOR,
    MNID_RELEASE_CHECK,
    MNID_DISABLE_NETWORK,
    MNID_MOBILE_CONTROLS,
    MNID_MOBILE_CONTROLS_SCALE
  };

private:
  struct StringOption {
    std::vector<std::string> list = {};
    int next = 0;
  };

private:
  const Type m_type;
  const bool m_complete;

  StringOption m_magnifications;
  StringOption m_aspect_ratios;
  StringOption m_window_resolutions;
  StringOption m_resolutions;
  StringOption m_vsyncs;
  StringOption m_sound_volumes;
  StringOption m_music_volumes;
  StringOption m_flash_intensity_values;
  StringOption m_mobile_control_scales;

private:
  OptionsMenu(const OptionsMenu&) = delete;
  OptionsMenu& operator=(const OptionsMenu&) = delete;
};

#endif

/* EOF */
