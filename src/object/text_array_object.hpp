//  SuperTux
//  Copyright (C) 2018 Nir <goproducti@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_TEXT_ARRAY_OBJECT_HPP
#define HEADER_SUPERTUX_OBJECT_TEXT_ARRAY_OBJECT_HPP

#include "editor/layer_object.hpp"

#include <memory>

#include "supertux/timer.hpp"
#include "object/text_object.hpp"
#include "object/text_array_item.hpp"

typedef size_t ta_index;

/**
 * An array of text objects, intended for narration.

 * @scripting
 * @summary A ""TextArrayObject"" that was given a name can be controlled by scripts.
            Supports all functions and variables of ${SRG_REF_Text}, applying them to the current text item.
            If no text item is available, calling functions or using variables of ${SRG_REF_Text} (other than ""set_text()"") will result in an error.${SRG_NEWPARAGRAPH}
            Intended for scripts with narration.
 * @instances A ""TextArrayObject"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class TextArrayObject final : public LayerObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  TextArrayObject(const std::string& name = "");
  ~TextArrayObject() override = default;

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;

  virtual bool is_saveable() const override { return false; }

  static std::string class_name() { return "text-array"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "TextArrayObject"; }
  static std::string display_name() { return _("Text array"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(TextArrayObject)); }

  virtual const std::string get_icon_path() const override {
    return "images/engine/editor/textarray.png";
  }

  /**
   * @scripting
   * @description Clears all text objects from the stack.
   */
  void clear();
  /**
   * @scripting
   * @description Adds a text object with a specific text at the end of the stack.
   * @param string $text
   */
  void add_text(const std::string& text);
  /**
   * @scripting
   * @description Adds a text object with a specific text and duration at the end of the stack.
   * @param string $text
   * @param float $duration
   */
  void add_text_duration(const std::string& text, float duration);
  /**
   * @scripting
   * @description Sets the current text object by its index.
   * @param int $index
   */
  void set_text_index(ta_index index);
  /**
   * @scripting
   * @description If set, keeps the current text object visible.
   * @param bool $keep_visible
   */
  inline void set_keep_visible(bool keep_visible) { m_keep_visible = keep_visible; }
  /**
   * @scripting
   * @description If set, allows for a fade-in and fade-out transition.
   * @param bool $fade_transition
   */
  inline void set_fade_transition(bool fade_transition) { m_fade_transition = fade_transition; }
  /**
   * @scripting
   * @description Sets the fade-in and fade-out time.
   * @param float $fadetime
   */
  inline void set_fade_time(float fadetime) { m_fadetime = fadetime; }
  /**
   * @scripting
   * @description If set, sets the text array as finished going through all text objects.
                  Alternatively, the "finished" property can be modified.
   * @param bool $done
   */
  inline void set_done(bool done) { m_finished = done; }
  /**
   * @scripting
   * @description If set, lets the text array automatically go through all text objects.
   * @param bool $is_auto
   */
  void set_auto(bool is_auto);
  /**
   * @scripting
   * @description If available, goes to the next text object in the stack.
   */
  void next_text();
  /**
   * @scripting
   * @description If available, goes to the previous text object in the stack.
   */
  void prev_text();

  /*
   * TextObject API related
   * @see: text_object.hpp
   */
  void set_text(const std::string& text);
  const std::string& get_text() const;
  void set_font(const std::string& fontname);
  void fade_in(float fadetime);
  void fade_out(float fadetime);
  void grow_in(float fadetime);
  void grow_out(float fadetime);
  void set_visible(bool visible);
  bool get_visible() const;
  void set_centered(bool centered);
  bool get_centered() const;
  void set_pos(float x, float y);
  float get_x() const;
  float get_y() const;
  void set_anchor_point(int anchor);
  int get_anchor_point() const;
  void set_anchor_offset(float x, float y);
  float get_wrap_width() const;
  void set_wrap_width(float width);
  void set_front_fill_color(float red, float green, float blue, float alpha);
  void set_back_fill_color(float red, float green, float blue, float alpha);
  void set_text_color(float red, float green, float blue, float alpha);
  void set_roundness(float roundness);
  float get_roundness() const;

  /** Gets the text item at a certain index.
      @param: index  the index of the text item to get.
      @return: pointer to the text array item; or nullptr if fails. */
  TextArrayItem* get_text_item(ta_index index) const;

  /** Gets the current text item.
      @return: pointer the current text array item; or nullptr if fails. */
  TextArrayItem* get_current_text_item() const;

  /** Gets the last text item.
      @return: pointer to the last text item; or nullptr if fails. */
  TextArrayItem* get_last_text_item() const;

private:
  /** Overrides the properties of the text objects, according to the flags. */
  void override_properties();

  /** Resets the auto narration state and updates it if necessary. */
  void reset_automation();

  /** Handles user input requests (skipping, rewinding)
      @note: might change to manual mode (disables auto flag) */
  void handle_input_requests();

  /** Should fade transition logic apply
      @return: true if fadeTransition flag is on & the transition is valid;
      false otherwise. */
  bool should_fade() const;

private:
  /**
   * @scripting
   * @description Determines whether the text array has finished going through all text objects.
   */
  bool m_finished;
  bool m_is_auto;

  /**
   * @scripting
   * @description Determines whether the current text object should be kept visible.
   */
  bool m_keep_visible;
  /**
   * @scripting
   * @description Allows for a fade-in and fade-out transition.
   */
  bool m_fade_transition;

  float m_fadetime;

  std::vector<std::unique_ptr<TextArrayItem> > m_texts;
  ta_index m_curTextIndex;
  ta_index m_lastTextIndex;

  Timer m_waiting;

private:
  TextArrayObject(const TextArrayObject&) = delete;
  TextArrayObject& operator=(const TextArrayObject&) = delete;
};

#endif

/* EOF */
