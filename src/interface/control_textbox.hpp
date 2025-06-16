//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTBOX_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTBOX_HPP

#include <list>

#include "control/input_manager.hpp"
#include "interface/control.hpp"


class ControlTextbox : public InterfaceControl
{
public:
  ControlTextbox();

  virtual void draw(DrawingContext& context) override;
  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;
  virtual bool on_key_up(const SDL_KeyboardEvent& key) override;
  virtual bool on_key_down(const SDL_KeyboardEvent& key) override;
  virtual bool event(const SDL_Event& ev) override;

  virtual void update(float dt_sec) override;

  /** Binds a string to the textbox */
  inline void bind_string(std::string* value) { m_string = value; }

  /** Returns the full string held in m_charlist */
  const std::string& get_string() const;

  /** Gets at which (absolute) index, in the text, corresponds an on-screen point */
  int get_text_position(const Vector& pos) const;

  /** Returns true if the given text would fit inside the box */
  bool fits(const std::string& text) const;

  /**
   * Copies the current selected text to the clipboard. If no text is selected,
   * does nothing and returns false.
   *
   * @returns true if copied successfully, false if error OR nothing was selected.
   */
  bool copy() const;

  /**
   * Pastes the text from the clipboard. If the clipboard doesn't have text,
   * does nothing and returns false.
   *
   * @returns true if pasted successfully, false if error OR cliboard was empty/non-text
   */
  bool paste();

  /** @returns the text currently selected by the user. */
  std::string get_selected_text() const;

  /** @returns true if some text was selected and therefore erased. */
  bool erase_selected_text();

  /**
   * Puts the given text at the currently selected position, replacing the text
   * that was selected, if any.
   *
   * @returns true if some text was deleted; false if no text was replaced.
   */
  bool put_text(const std::string& text);

protected:
  /** Transfers the string into the binded variable, if any. Can be overridden
   *  by children if they use different value members (like float, int, etc).
   *  parse_value MUST make the validation, and MUST use get_contents() to
   *  fetch the string to parse, as get_string() will return the precedent
   *  valid string.
   *  @param call_on_change Whether calling this function should also call
   *                        m_on_change(). Children classes that implement
   *                        another m_value member (float, int, etc.) should
   *                        call `ControlTextbox::parse_value(false)` to parse
   *                        the string without calling m_on_change immediately;
   *                        the child class should call m_on_change() by itself
   *                        when and only when they have parsed their value.
   *  @returns Whether or not the value in the textbox was a valid value. If
   *           it isn't, then parse_value() should handle reverting the char
   *           vector to its last valid state.
   *  @see get_string()
   *  @see revert_value()
   */
  virtual bool parse_value(bool call_on_change = true);

  /** Reverts the contents of the char vector to the value of the member
   *  variable. Can be overridden by children, as parse_value() does.
   */
  virtual void revert_value();

protected:
  /** WARNING : These function returns the status of the string *as the user
   *  types*, and therefore are unsuited for callers expecting a validated
   *  value. If such case is, use get_string() instead. If you use these
   *  functions below, you'll get a half-typed value! (And don't validate
   *  while the user is typing, it could make typing impossible!)
   */

  /** Converts the internal char vector to an actual string and returns it. */
  std::string get_contents() const;

  /** Returns first "amount" chars held in m_charlist */
  std::string get_first_chars(int amount) const;

  /** Returns the part of the string that is actually displayed */
  std::string get_contents_visible() const;

  /** Returns first "amount" chars that are displayed */
  std::string get_first_chars_visible(int amount) const;

public:
  /** Optional, a function to validate the string. If nullptr, then all values
   *  are assumed to be valid.
   *
   *  @param ControlTextbox* A pointer to the original control.
   *  @param std::string The string that is about to be applied to the textbox.
   *  @returns Whether or not this value is valid. If not, then the internal
   *           string will not be modified. Tip : You can manually change the
   *           string using set_string() inside this function and return false
   *           to establish a custom value (for example, a max string length).
   */
  bool (*m_validate_string)(ControlTextbox*, std::string);

protected:
  /** Holds the list of characters that are in the textbox. When characters are
   *  added or deleted, this is the member variable that is getting edited.
   */
  std::list<char> m_charlist;

  /** This is the value that should be looked at by external functions.
   *  @see get_string()
   *  @see bind_string()
   *  @see m_internal_string_backup
   */
  std::string* m_string;

  /** Used so that if m_string is not bound, get_string() won't break/segfault
   *  @see get_string()
   *  @see bind_string()
   *  @see m_string
   */
  std::string m_internal_string_backup;

  float m_cursor_timer;
  int m_caret_pos;
  int m_secondary_caret_pos; /**< Used for selections */
  bool m_shift_pressed, m_ctrl_pressed;
  bool m_mouse_pressed;

  /**
   * If the string is too long to be contained in the box,
   * use this offset to select which characters will be
   * displayed on the screen
   */
  int m_current_offset;

protected:
  void delete_char_before_caret();

  /** Returns the largest string fitting in the box. */
  std::string get_truncated_text(const std::string& text) const;

  /** Changes m_current_offset so that the caret is visible */
  void recenter_offset();

private:
  ControlTextbox(const ControlTextbox&) = delete;
  ControlTextbox& operator=(const ControlTextbox&) = delete;
};

#endif

/* EOF */
