//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_GUI_MENU_MANAGER_HPP
#define HEADER_SUPERTUX_GUI_MENU_MANAGER_HPP

#include "util/currenton.hpp"

#include <memory>
#include <vector>

#include "control/controller.hpp"
#include "gui/menu_action.hpp"
#include "gui/menu_transition.hpp"
#include "math/rectf.hpp"

class Controller;
class Dialog;
class Menu;
class Notification;
union SDL_Event;

class MenuManager final : public Currenton<MenuManager>
{
private:
  static const float s_menu_repeat_initial;
  static const float s_menu_repeat_rate;

public:
  static MenuManager& instance();

private:
  template<typename T>
  struct SettableItem {
    std::unique_ptr<T> current = nullptr;
    std::unique_ptr<T> next = nullptr;
    bool has_next = false;
  };

private:
  std::vector<std::unique_ptr<Menu> > m_menu_stack;
  std::unique_ptr<MenuTransition> m_transition;

  float m_menu_repeat_time;

  SettableItem<Dialog> m_dialog;
  SettableItem<Notification> m_notification;

public:
  MenuManager();
  ~MenuManager() override;

  void event(const SDL_Event& event);
  void process_input(const Controller& controller);
  void refresh();

  void draw(DrawingContext& context);

  void set_dialog(std::unique_ptr<Dialog> dialog);
  void set_notification(std::unique_ptr<Notification> notification);

  void set_menu(int id, bool skip_transition = false);
  void set_menu(std::unique_ptr<Menu> menu, bool skip_transition = false);
  void push_menu(int id, bool skip_transition = false);
  void push_menu(std::unique_ptr<Menu> menu, bool skip_transition = false);
  void pop_menu(bool skip_transition = false);
  void clear_menu_stack(bool skip_transition = false);

  void on_window_resize();

  inline bool is_active() const { return !m_menu_stack.empty(); }
  inline bool has_dialog() const { return m_dialog.current || m_dialog.has_next; }

  Menu* current_menu() const;
  Menu* previous_menu() const;

private:
  void check_input_action(Control control, MenuAction action,
                          const Controller& controller, MenuAction& result);

  template<typename T>
  Rectf to_rect(T& menu)
  {
    return Rectf(menu.get_center_pos().x - menu.get_width() / 2,
                 menu.get_center_pos().y - menu.get_height() / 2,
                 menu.get_center_pos().x + menu.get_width() / 2,
                 menu.get_center_pos().y + menu.get_height() / 2);
  }

  template<typename S, typename T>
  Rectf to_transition_rect(S* from, T* to)
  {
    if (from)
      return to_rect(*from);
    else
      return Rectf(to->get_center_pos(), Sizef(0, 0));
  }

  template<typename S, typename T>
  void transition(S* from, T* to)
  {
    if (!from && !to)
      return;

    m_transition->start(to_transition_rect(from, to),
                        to_transition_rect(to, from));
  }

private:
  MenuManager(const MenuManager&) = delete;
  MenuManager& operator=(const MenuManager&) = delete;
};

#endif

/* EOF */
