//  SuperTux
//  Copyright (C) 2022 Daniel Ward <weluvgoatz@gmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_FISH_HARMLESS_HPP
#define HEADER_SUPERTUX_BADGUY_FISH_HARMLESS_HPP

#include "badguy/fish_swimming.hpp"

/** Fish, which doesn't harm the player. */
class FishHarmless final : public FishSwimming
{
public:
  FishHarmless(const ReaderMapping& reader);

  static std::string class_name() { return "fish-harmless"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Harmless Fish"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return FishSwimming::get_class_types().add(typeid(FishHarmless)); }
  virtual std::string get_overlay_size() const override { return "1x1"; }

  GameObjectTypes get_types() const override { return {}; }

protected:
  virtual void initialize() override;

private:
  FishHarmless(const FishHarmless&) = delete;
  FishHarmless& operator=(const FishHarmless&) = delete;
};

#endif

/* EOF */
