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

#ifndef HEADER_SUPERTUX_BADGUY_YETI_STALACTITE_HPP
#define HEADER_SUPERTUX_BADGUY_YETI_STALACTITE_HPP

#include "badguy/stalactite.hpp"

class YetiStalactite final : public Stalactite
{
public:
  YetiStalactite(const ReaderMapping& mapping);

  virtual void active_update(float dt_sec) override;
  virtual void after_editor_set() override;
  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;

  virtual bool is_flammable() const override;
  static std::string class_name() { return "yeti_stalactite"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Yeti's Stalactite"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Stalactite::get_class_types().add(typeid(YetiStalactite)); }

  void start_shaking();
  bool is_hanging() const;

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  YetiStalactite(const YetiStalactite&) = delete;
  YetiStalactite& operator=(const YetiStalactite&) = delete;
};

#endif

/* EOF */
