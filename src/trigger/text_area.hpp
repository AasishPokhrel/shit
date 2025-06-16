//  SuperTux
//  Copyright (C) 2021 mrkubax10 <mrkubax10@onet.pl>
//                2021 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_TRIGGER_TEXT_AREA_HPP
#define HEADER_SUPERTUX_TRIGGER_TEXT_AREA_HPP

#include "trigger/trigger_base.hpp"

#include "supertux/timer.hpp"

class TextArea final : public Trigger
{
private:
  enum class Status
  {
    NOT_STARTED,
    FADING_IN,
    WAITING,
    FADING_OUT,
    FINISHED
  };

public:
  TextArea(const ReaderMapping& mapping);

  virtual void draw(DrawingContext& context) override;
  virtual void event(Player& player, EventType type) override;
  virtual void update(float dt_sec) override;

  virtual ObjectSettings get_settings() override;
  static std::string class_name() { return "text-area"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Text Area"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual bool has_variable_size() const override { return true; }
  virtual GameObjectClasses get_class_types() const override { return Trigger::get_class_types().add(typeid(TextArea)); }

private:
  bool m_once;
  std::vector<std::string> m_items;
  float m_delay;
  float m_fade_delay;
  size_t m_current_text;
  Status m_status;
  Timer m_timer;
  AnchorPoint m_anchor;
  Vector m_anchor_offset;

private:
  TextArea(const TextArea&) = delete;
  TextArea& operator=(const TextArea&) = delete;
};

#endif
/* EOF */
