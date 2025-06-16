//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_OPTIONS_SELECT_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_OPTIONS_SELECT_MENU_HPP

#include "gui/menu.hpp"

class OptionsSelectMenu final : public Menu
{
private:
  static const std::string s_icons_base_dir;

public:
  OptionsSelectMenu(bool complete = true);

  void menu_action(MenuItem& item) override;

private:
  enum MenuIDs {
    MNID_OPTIONCATEGORIES
  };

private:
  bool m_complete_options;

private:
  OptionsSelectMenu(const OptionsSelectMenu&) = delete;
  OptionsSelectMenu& operator=(const OptionsSelectMenu&) = delete;
};

#endif

/* EOF */
