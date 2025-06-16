//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_TEXTSCROLLER_HPP
#define HEADER_SUPERTUX_OBJECT_TEXTSCROLLER_HPP

#include "editor/layer_object.hpp"

#include <memory>
#include <vector>

#include "supertux/info_box_line.hpp"
#include "control/controller.hpp"

class DrawingContext;
class InfoBoxLine;
class ReaderCollection;
class ReaderMapping;
class ReaderObject;

class TextScroller : public LayerObject
{
public:
  TextScroller(const ReaderMapping& mapping);
  TextScroller(const ReaderObject& root);

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual ObjectSettings get_settings() override;
  static std::string class_name() { return "textscroller"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Text Scroller"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(TextScroller)); }
  virtual const std::string get_icon_path() const override { return "images/engine/editor/textscroller.png"; }

  void scroll(float offset);

  inline void set_default_speed(float default_speed) { m_default_speed = default_speed; }
  inline bool is_finished() const { return m_finished; }
  
protected:
  const Controller* controller;

private:
  void parse_file(const std::string& filename);
  void parse_root(const ReaderObject& root);
  void parse_content(const ReaderCollection& collection);

private:
  std::string m_filename;
  std::string m_finish_script;
  std::vector<std::unique_ptr<InfoBoxLine> > m_lines;
  float m_scroll;
  float m_default_speed;
  float m_x_offset;
  bool m_controllable;
  bool m_finished;
  bool m_fading;

  enum XAnchor {
    SCROLLER_ANCHOR_LEFT,
    SCROLLER_ANCHOR_CENTER,
    SCROLLER_ANCHOR_RIGHT
  };
  enum TextAlign {
    SCROLLER_ALIGN_LEFT,
    SCROLLER_ALIGN_CENTER,
    SCROLLER_ALIGN_RIGHT
  };

  XAnchor m_x_anchor;
  TextAlign m_text_align;

private:
  TextScroller(const TextScroller&) = delete;
  TextScroller& operator=(const TextScroller&) = delete;
};

#endif

/* EOF */
