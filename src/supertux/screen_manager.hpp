//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_SCREEN_MANAGER_HPP
#define HEADER_SUPERTUX_SUPERTUX_SCREEN_MANAGER_HPP

#include <chrono>
#include <memory>
#include <SDL.h>

#include "config.h"

#include "control/mobile_controller.hpp"
#include "squirrel/squirrel_thread_queue.hpp"
#include "supertux/screen.hpp"
#include "util/currenton.hpp"

class Compositor;
class ControllerHUD;
class DrawingContext;
class InputManager;
class MenuManager;
class MenuStorage;
class ScreenFade;
class VideoSystem;

/**
 * Manages, updates and draws all Screens, Controllers, Menus and the Console.
 */
class ScreenManager final : public Currenton<ScreenManager>
{
public:
  ScreenManager(VideoSystem& video_system, InputManager& input_manager);
  ~ScreenManager() override;

  void run();
  void quit(std::unique_ptr<ScreenFade> fade = {});
  inline void set_speed(float speed) { m_speed = speed; }
  inline float get_speed() const { return m_speed; }
  bool has_pending_fadeout() const;

  void on_window_resize();

  // push new screen on screen_stack
  void push_screen(std::unique_ptr<Screen> screen, std::unique_ptr<ScreenFade> fade = {});
  void pop_screen(std::unique_ptr<ScreenFade> fade = {});
  void set_screen_fade(std::unique_ptr<ScreenFade> fade);

  void loop_iter();

  inline const std::vector<std::unique_ptr<Screen>>& get_screen_stack() { return m_screen_stack; }

private:
  struct FPS_Stats;
  void draw_fps(DrawingContext& context, FPS_Stats& fps_statistics);
  void draw_player_pos(DrawingContext& context);
  void draw(Compositor& compositor, FPS_Stats& fps_statistics);
  void update_gamelogic(float dt_sec);
  void process_events();
  void handle_screen_switch();

private:
  VideoSystem& m_video_system;
  InputManager& m_input_manager;
  std::unique_ptr<MenuStorage> m_menu_storage;
  std::unique_ptr<MenuManager> m_menu_manager;
  std::unique_ptr<ControllerHUD> m_controller_hud;
  MobileController m_mobile_controller;

  std::chrono::steady_clock::time_point last_time;
  float elapsed_time;
  const float seconds_per_step;
  std::unique_ptr<FPS_Stats> m_fps_statistics;

  float m_speed;
  struct Action
  {
    enum Type { PUSH_ACTION, POP_ACTION, QUIT_ACTION };
    Type type;
    std::unique_ptr<Screen> screen;

    Action(Type type_,
           std::unique_ptr<Screen> screen_ = {}) :
      type(type_),
      screen(std::move(screen_))
    {}
  };

  std::vector<Action> m_actions;

  std::unique_ptr<ScreenFade> m_screen_fade;
  std::vector<std::unique_ptr<Screen> > m_screen_stack;
};

#endif

/* EOF */
