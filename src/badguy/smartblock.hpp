//  SuperTux
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

#ifndef HEADER_SUPERTUX_BADGUY_SMARTBLOCK_HPP
#define HEADER_SUPERTUX_BADGUY_SMARTBLOCK_HPP

#include "badguy/mriceblock.hpp"

class SmartBlock final : public MrIceBlock
{
public:
  SmartBlock(const ReaderMapping& reader);

  virtual std::string get_water_sprite() const override { return "images/objects/water_drop/pink_drop.sprite"; }
  static std::string class_name() { return "smartblock"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Mrs. Iceblock"); }
  virtual std::string get_display_name() const override { return display_name(); }
    virtual GameObjectClasses get_class_types() const override { return MrIceBlock::get_class_types().add(typeid(SmartBlock)); }

  GameObjectTypes get_types() const override { return {}; }

private:
  SmartBlock(const SmartBlock&) = delete;
  SmartBlock& operator=(const SmartBlock&) = delete;
};

#endif

/* EOF */
