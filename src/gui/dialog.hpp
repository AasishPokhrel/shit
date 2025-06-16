//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_GUI_DIALOG_HPP
#define HEADER_SUPERTUX_GUI_DIALOG_HPP

#include <SDL.h>
#include <functional>
#include <string>
#include <vector>

#include "gui/menu_manager.hpp"
#include "math/sizef.hpp"
#include "util/gettext.hpp"

class Controller;
class DrawingContext;

class Dialog
{
private:
  struct Button
  {
    std::string text;
    std::function<void ()> callback;
  };

  std::string m_text;
  std::vector<Button> m_buttons;
  int m_selected_button;
  int m_cancel_button;
  bool m_passive;
  bool m_clear_diags;

  Sizef m_text_size;
  Sizef m_size;

public:
  Dialog(bool passive = false, bool auto_clear_dialogs = true);
  virtual ~Dialog();

  void set_text(const std::string& text);

  void add_button(const std::string& text, const std::function<void ()>& callback = {});

  /** The default gets focused when the dialog is first shown */
  void add_default_button(const std::string& text, const std::function<void ()>& callback = {});

  /** The cancel button can not only be activated by selecting it, but
      via the MENU_BACK button */
  void add_cancel_button(const std::string& text, const std::function<void ()>& callback = {});

  void clear_buttons();

  void event(const SDL_Event& event);
  void process_input(const Controller& controller);
  void draw(DrawingContext& context);
  virtual void update() {}
  bool is_passive() const
  {
    return m_passive;
  }

  Vector get_center_pos() const;

  inline float get_width() const { return m_size.width; }
  inline float get_height() const { return m_size.height; }

  static void show_message(const std::string& text, bool passive = false, bool no_auto_clear = false, const std::function<void ()>& ok_callback = {})
  {
    auto dialog = std::make_unique<Dialog>(passive, !no_auto_clear);
    dialog->set_text(text);
    dialog->clear_buttons();
    dialog->add_button(_("OK"), ok_callback);
    MenuManager::instance().set_dialog(std::move(dialog));
  }

  static void show_confirmation(const std::string& text, const std::function<void ()>& callback, bool no_auto_clear = false)
  {
    auto dialog = std::make_unique<Dialog>(false, !no_auto_clear);
    dialog->set_text(text);
    dialog->clear_buttons();
    dialog->add_default_button(_("Yes"), callback);
    dialog->add_cancel_button(_("No"));
    MenuManager::instance().set_dialog(std::move(dialog));
  }

private:
  void on_button_click(int button) const;
  int get_button_at(const Vector& pos) const;

private:
  Dialog(const Dialog&) = delete;
  Dialog& operator=(const Dialog&) = delete;
};

#endif

/* EOF */
