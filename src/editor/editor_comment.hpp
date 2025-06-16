//  SuperTux
//  Copyright (C) 2025 MatusGuy <matusguy@supertux.org>
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

#ifndef HEADER_SUPERTUX_EDITOR_EDITOR_COMMENT_HPP
#define HEADER_SUPERTUX_EDITOR_EDITOR_COMMENT_HPP

#include "supertux/moving_object.hpp"
#include "video/layer.hpp"

class ReaderMapping;
class InfoBoxLine;

class EditorComment : public MovingObject
{
public:
  enum Type
  {
    NOTE,
    TODO,
    FIXME,
    HACK
  };

public:
  explicit EditorComment(const ReaderMapping& reader);

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override { MovingObject::update(dt_sec); }
  virtual HitResponse collision(MovingObject&, const CollisionHit&) override { return ABORT_MOVE; }
  virtual void check_state() override;

  virtual bool has_variable_size() const override { return true; }
  static std::string class_name() { return "editor-comment"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Editor Comment"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(EditorComment)); }

  virtual GameObjectTypes get_types() const override;
  virtual ObjectSettings get_settings() override;

  virtual int get_layer() const override { return LAYER_FOREGROUND1 - 5; }

private:
  void refresh_comment();
  Color get_color() const;

private:
  std::string m_comment;
  std::vector<std::unique_ptr<InfoBoxLine>> m_lines;

private:
  EditorComment(const EditorComment&) = delete;
  EditorComment& operator=(const EditorComment&) = delete;
};

#endif // HEADER_SUPERTUX_EDITOR_EDITOR_COMMENT_HPP
