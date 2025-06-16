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

#ifndef HEADER_SUPERTUX_OBJECT_TEXT_OBJECT_HPP
#define HEADER_SUPERTUX_OBJECT_TEXT_OBJECT_HPP

#include "editor/layer_object.hpp"

#include "math/anchor_point.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/font_ptr.hpp"

class DrawingContext;
class ReaderMapping;

/**
 * A text object intended for scripts that want to tell a story.

 * @scripting
 * @summary A ""TextObject"" that was given a name can be controlled by scripts.
 * @instances A ""TextObject"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class TextObject final : public LayerObject
{
  static Color default_color;

public:
  static void register_class(ssq::VM& vm);

public:
  TextObject(const std::string& name = "");
  ~TextObject() override;

  static std::string class_name() { return "textobject"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "TextObject"; }
  static std::string display_name() { return _("Text"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(TextObject)); }

  virtual const std::string get_icon_path() const override { return "images/engine/editor/textarray.png"; }

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual bool is_saveable() const override { return false; }

  /**
   * @scripting
   * @description Sets the text string to be displayed.
   * @param string $text
   */
  void set_text(const std::string& text);
  /**
   * @scripting
   * @description Returns the displayed text.
   */
  inline const std::string& get_text() const { return m_text; }
  /**
   * @scripting
   * @description Sets the font of the text to be displayed.
   * @param string $fontname Valid values are normal, big and small.
   */
  void set_font(const std::string& fontname);
  /**
   * @scripting
   * @description Fades in the specified text for the next ""fadetime"" seconds.
   * @param float $fadetime
   */
  void fade_in(float fadetime);
  /**
   * @scripting
   * @description Fades out the specified text for the next ""fadetime"" seconds.
   * @param float $fadetime
   */
  void fade_out(float fadetime);
  /**
   * @scripting
   * @description Grows in the specified text for the next ""fadetime"" seconds.
   * @param float $fadetime
   */
  void grow_in(float fadetime);
  /**
   * @scripting
   * @description Grows out the specified text for the next ""fadetime"" seconds.
   * @param float $fadetime
   */
  void grow_out(float fadetime);
  /**
   * @scripting
   * @description Shows or hides the text abruptly (drastic counterpart to ""fade_in()"" and ""fade_out()"").
   * @param bool $visible
   */
  void set_visible(bool visible);
  /**
   * @scripting
   * @description Returns ""true"" if the text is visible.
   */
  inline bool get_visible() const { return m_visible; }
  /**
   * @scripting
   * @description If ""centered"" is ""true"", the text will be centered on the screen. Otherwise, it will be left-aligned.
   * @param bool $centered
   */
  inline void set_centered(bool centered) { m_centered = centered; }
  /**
   * @scripting
   * @description Returns ""true"" if the text is centered.
   */
  inline bool get_centered() const { return m_centered; }
  /**
   * @scripting
   * @description Sets the offset of the text, relative to the anchor point.
   * @param float $x
   * @param float $y
   */
  inline void set_pos(float x, float y) { m_pos = Vector(x, y); }
  /**
   * @scripting
   * @description Returns the X offset of the text, relative to the anchor point.
   */
  inline float get_x() const { return m_pos.x; }
  /**
   * @scripting
   * @description Returns the Y offset of the text, relative to the anchor point.
   */
  inline float get_y() const { return m_pos.y; }
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @deprecated Use ""get_x()"" instead!
   * @description Returns the X offset of the text, relative to the anchor point.
   */
  float get_pos_x() const;
  /**
   * @scripting
   * @deprecated Use ""get_y()"" instead!
   * @description Returns the Y offset of the text, relative to the anchor point.
   */
  float get_pos_y() const;
#endif
  /**
   * @scripting
   * @description Sets the anchor point of the text.
   * @param int $anchor One of the ""ANCHOR_*"" constants (see ${SRG_REF_AnchorPoints}).
   */
  inline void set_anchor_point(int anchor) { m_anchor = static_cast<AnchorPoint>(anchor); }
  /**
   * @scripting
   * @description Returns the current anchor point of the text (one of the ""ANCHOR_*"" constants; see ${SRG_REF_AnchorPoints}).
   */
  inline int get_anchor_point() const { return static_cast<int>(m_anchor); }
  /**
   * @scripting
   * @description Sets the anchor offset of the text.
   * @param float $x
   * @param float $y
   */
  inline void set_anchor_offset(float x, float y) { m_anchor_offset = Vector(x, y); }
  /**
   * @scripting
   * @description Gets the text wrap width of the text.
   */
  inline float get_wrap_width() const { return m_wrap_width; }
  /**
   * @scripting
   * @description Sets the text wrap width of the text.
   * @param float $width
   */
  inline void set_wrap_width(float width) { m_wrap_width = width; }
  /**
   * @scripting
   * @description Sets the front fill color of the text.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   */
  void set_front_fill_color(float red, float green, float blue, float alpha);
  /**
   * @scripting
   * @description Sets the back fill color of the text.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   */
  void set_back_fill_color(float red, float green, float blue, float alpha);
  /**
   * @scripting
   * @description Sets the text color.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   */
  void set_text_color(float red, float green, float blue, float alpha);
  /**
   * @scripting
   * @description Sets the frame's roundness.
   * @param float $roundness
   */
  inline void set_roundness(float roundness) { m_roundness = roundness; }
  /**
   * @scripting
   * @description Returns the roundness of the text.
   */
  inline float get_roundness() const { return m_roundness; }

  inline void set_anchor_point(AnchorPoint anchor) { m_anchor = anchor; }
  inline void set_anchor_offset(const Vector& offset) { m_anchor_offset = offset; }

  inline void set_pos(const Vector& pos) { m_pos = pos; }
  inline const Vector& get_pos() const { return m_pos; }

private:
  void wrap_text();

private:
  FontPtr m_font;
  /**
   * @scripting
   * @description The displayed text.
   */
  std::string m_text;
  std::string m_wrapped_text;
  float m_fade_progress;
  float m_fadetime;
  /**
   * @scripting
   * @description Determines whether the text is visible.
   */
  bool m_visible;
  /**
   * @scripting
   * @description Determines whether the text is centered.
   */
  bool m_centered;
  AnchorPoint m_anchor;
#ifdef DOXYGEN_SCRIPTING 
  /**
   * @scripting
   * @description The current anchor point. 
   */
  int m_anchor_point;
#endif
  Vector m_anchor_offset;
  Vector m_pos;
  /**
   * @scripting
   * @description Determines the maximum wrap width of the text.
   */
  float m_wrap_width;
  Color m_front_fill_color;
  Color m_back_fill_color;
  Color m_text_color;
  /**
   * @scripting
   * @description Determines the roundness of the text frame.
   */
  float m_roundness;
  bool m_growing_in;
  bool m_growing_out;
  bool m_fading_in;
  bool m_fading_out;
  bool m_grower;
  bool m_fader;

private:
  TextObject(const TextObject&) = delete;
  TextObject& operator=(const TextObject&) = delete;
};

#endif

/* EOF */
