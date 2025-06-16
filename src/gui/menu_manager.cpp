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

#include "gui/menu_manager.hpp"

#include "control/input_manager.hpp"
#include "gui/dialog.hpp"
#include "gui/menu.hpp"
#include "gui/mousecursor.hpp"
#include "gui/notification.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "util/log.hpp"
#include "video/drawing_context.hpp"

const float MenuManager::s_menu_repeat_initial = 0.4f;
const float MenuManager::s_menu_repeat_rate = 0.1f;

MenuManager&
MenuManager::instance()
{
  return *current();
}


MenuManager::MenuManager() :
  m_menu_stack(),
  m_transition(new MenuTransition),
  m_menu_repeat_time(),
  m_dialog(),
  m_notification()
{
}

MenuManager::~MenuManager()
{
}

void
MenuManager::refresh()
{
  for (const auto& menu : m_menu_stack)
    menu->refresh();
}

void
MenuManager::check_input_action(Control control, MenuAction action,
                                const Controller& controller, MenuAction& result)
{
  if (controller.pressed(control))
  {
    result = action;
    m_menu_repeat_time = g_real_time + s_menu_repeat_initial;
  }
  if (controller.hold(control) &&
      m_menu_repeat_time != 0 && g_real_time > m_menu_repeat_time)
  {
    result = action;
    m_menu_repeat_time = g_real_time + s_menu_repeat_rate;
  }
}

void
MenuManager::process_input(const Controller& controller)
{
  if (m_dialog.current && !m_dialog.current->is_passive())
  {
    m_dialog.current->process_input(controller);
  }
  else if (current_menu())
  {
    MenuAction action = MenuAction::NONE;

    check_input_action(Control::UP, MenuAction::UP, controller, action);
    check_input_action(Control::DOWN, MenuAction::DOWN, controller, action);
    check_input_action(Control::LEFT, MenuAction::LEFT, controller, action);
    check_input_action(Control::RIGHT, MenuAction::RIGHT, controller, action);

    if (controller.pressed(Control::ACTION) ||
        controller.pressed(Control::JUMP) ||
        controller.pressed(Control::MENU_SELECT) ||
        (!current_menu()->is_sensitive() && controller.pressed(Control::MENU_SELECT_SPACE)))
    {
      action = MenuAction::HIT;
    }

    if (controller.pressed(Control::ESCAPE) ||
        controller.pressed(Control::CHEAT_MENU) ||
        controller.pressed(Control::DEBUG_MENU) ||
        controller.pressed(Control::MENU_BACK))
    {
      action = MenuAction::BACK;
    }

    check_input_action(Control::REMOVE, MenuAction::REMOVE, controller, action);

    current_menu()->process_action(action);
  }
}

void
MenuManager::event(const SDL_Event& ev)
{
  if (m_notification.current)
    m_notification.current->event(ev);

  if (m_transition->is_active()) return; // Do not continue, if a transition is active

  if (m_dialog.current && !m_dialog.current->is_passive())
    m_dialog.current->event(ev);
  else if (current_menu())
    current_menu()->event(ev);
}

void
MenuManager::draw(DrawingContext& context)
{
  if (m_dialog.has_next) // Has next dialog
  {
    // Removed to fix a crash with changing dialog in initial next dialog update
    //if (m_dialog.next) m_dialog.next->update();

    if (m_dialog.current && m_dialog.next)
    {
      const bool current_passive = m_dialog.current->is_passive();
      const bool next_passive = m_dialog.next->is_passive();

      if (!current_passive && !next_passive)
        transition(m_dialog.current.get(), m_dialog.next.get());
      else if (current_passive)
        transition(current_menu(), m_dialog.next.get());
      else if (next_passive)
        transition(m_dialog.current.get(), current_menu());
    }
    else if (m_dialog.current && !m_dialog.current->is_passive())
    {
      transition(m_dialog.current.get(), current_menu());
    }
    else if (m_dialog.next && !m_dialog.next->is_passive())
    {
      transition(current_menu(), m_dialog.next.get());
    }

    m_dialog.current = std::move(m_dialog.next);
    m_dialog.has_next = false;
  }

  if (m_notification.has_next) // Has next notification
  {
    m_notification.current = std::move(m_notification.next);
    m_notification.has_next = false;
  }

  if (m_transition->is_active())
  {
    m_transition->update();
    m_transition->draw(context);
  }
  else // Has no active transition
  {
    if (m_dialog.current)
    {
      m_dialog.current->update();
      m_dialog.current->draw(context);
    }

    if (current_menu() && (!m_dialog.current || m_dialog.current->is_passive()))
    {
      // brute force the transition into the right shape in case the
      // menu has changed sizes
      m_transition->set(to_rect(*current_menu()));
      m_transition->draw(context);

      current_menu()->draw(context);
    }
  }

  if (m_notification.current) // Has current notification
    m_notification.current->draw(context);

  if ((has_dialog() || is_active()) && MouseCursor::current()) // Cursor should be drawn
    MouseCursor::current()->draw(context);
}


void
MenuManager::set_dialog(std::unique_ptr<Dialog> dialog)
{
  m_dialog.next = std::move(dialog);
  m_dialog.has_next = true;
}

void
MenuManager::set_notification(std::unique_ptr<Notification> notification)
{
  m_notification.next = std::move(notification);
  m_notification.has_next = true;
}


void
MenuManager::set_menu(int id, bool skip_transition)
{
  set_menu(MenuStorage::instance().create(static_cast<MenuStorage::MenuId>(id)), skip_transition);
}

void
MenuManager::set_menu(std::unique_ptr<Menu> menu, bool skip_transition)
{
  if (menu)
  {
    if (!skip_transition)
      transition(m_menu_stack.empty() ? nullptr : m_menu_stack.back().get(), menu.get());
    m_menu_stack.clear();
    m_menu_stack.push_back(std::move(menu));
  }
  else
  {
    if (!skip_transition)
      transition<Menu, Menu>(m_menu_stack.empty() ? nullptr : m_menu_stack.back().get(), nullptr);
    m_menu_stack.clear();
  }

  // just to be sure...
  InputManager::current()->reset();
}

void
MenuManager::push_menu(int id, bool skip_transition)
{
  push_menu(MenuStorage::instance().create(static_cast<MenuStorage::MenuId>(id)), skip_transition);
}

void
MenuManager::push_menu(std::unique_ptr<Menu> menu, bool skip_transition)
{
  assert(menu);
  if (!skip_transition)
    transition(m_menu_stack.empty() ? nullptr : m_menu_stack.back().get(), menu.get());
  m_menu_stack.push_back(std::move(menu));
}

void
MenuManager::pop_menu(bool skip_transition)
{
  if (m_menu_stack.empty())
  {
    log_warning << "trying to pop on an empty menu_stack" << std::endl;
    return;
  }

  if (!skip_transition)
    transition(m_menu_stack.back().get(),
               m_menu_stack.size() >= 2 ? m_menu_stack[m_menu_stack.size() - 2].get() : nullptr);
  m_menu_stack.pop_back();
}

void
MenuManager::clear_menu_stack(bool skip_transition)
{
  if (!skip_transition)
    transition<Menu, Menu>(m_menu_stack.empty() ? nullptr : m_menu_stack.back().get(), nullptr);
  m_menu_stack.clear();
}


void
MenuManager::on_window_resize()
{
  for (const auto& menu : m_menu_stack)
    menu->on_window_resize();
}


Menu*
MenuManager::current_menu() const
{
  if (m_menu_stack.empty())
    return nullptr;
  else
    return m_menu_stack.back().get();
}

Menu*
MenuManager::previous_menu() const
{
  if (m_menu_stack.size() < 2)
    return nullptr;
  else
    return m_menu_stack.end()[-2].get();
}

/* EOF */
