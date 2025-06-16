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

#ifndef HEADER_SUPERTUX_TRIGGER_SEQUENCE_TRIGGER_HPP
#define HEADER_SUPERTUX_TRIGGER_SEQUENCE_TRIGGER_HPP

#include "trigger/trigger_base.hpp"

#include "supertux/sequence.hpp"

class SequenceTrigger final : public Trigger
{
public:
  SequenceTrigger(const ReaderMapping& reader);

  static std::string class_name() { return "sequencetrigger"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Sequence Trigger"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual bool has_variable_size() const override { return true; }
  virtual GameObjectClasses get_class_types() const override { return Trigger::get_class_types().add(typeid(SequenceTrigger)); }

  virtual ObjectSettings get_settings() override;

  virtual void event(Player& player, EventType type) override;
  virtual void draw(DrawingContext& context) override;

  std::string get_sequence_name() const;

private:
  EventType triggerevent;
  Sequence sequence;
  std::string new_spawnpoint;
  std::string fade_tilemap;
  TilemapFadeType fade;

private:
  SequenceTrigger(const SequenceTrigger&) = delete;
  SequenceTrigger& operator=(const SequenceTrigger&) = delete;
};

#endif

/* EOF */
