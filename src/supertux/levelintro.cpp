//  SuperTux -- LevelIntro screen
//  Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
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

#include "supertux/levelintro.hpp"

#include "control/input_manager.hpp"
#include "math/random.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/level.hpp"
#include "supertux/player_status.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "video/compositor.hpp"

#include <fmt/format.h>


// TODO: Display all players on the intro scene
LevelIntro::LevelIntro(const Level& level, const Statistics* best_level_statistics, const PlayerStatus& player_status) :
  m_level(level),
  m_best_level_statistics(best_level_statistics),
  m_player_sprite(),
  m_santa_sprite(),
  m_player_sprite_py(),
  m_player_sprite_vy(),
  m_player_sprite_jump_timer(),
  m_player_status(player_status)
{
  for (int i = 0; i < InputManager::current()->get_num_users(); i++)
  {
    push_player();
  }
}

LevelIntro::~LevelIntro()
{
}

void
LevelIntro::setup()
{
}

void
LevelIntro::update(float dt_sec, const Controller& controller)
{
  // Check if it's time to exit the screen
  if (controller.pressed_any(Control::JUMP, Control::ACTION, Control::MENU_SELECT,
                             Control::START, Control::ESCAPE)) {
    ScreenManager::current()->pop_screen(std::make_unique<FadeToBlack>(FadeToBlack::FADEOUT, 0.1f));
  }

  // Check if players connected/disconnected
  while(m_player_sprite.size() < static_cast<size_t>(InputManager::current()->get_num_users()))
    push_player();

  while(m_player_sprite.size() > static_cast<size_t>(InputManager::current()->get_num_users()))
    pop_player();

  for (int i = 0; i < InputManager::current()->get_num_users(); i++)
  {
    if (!InputManager::current()->has_corresponsing_controller(i)
        && !InputManager::current()->m_uses_keyboard[i] && i != 0)
      continue;

    auto bonus_prefix = m_player_status.get_bonus_prefix(i);
    if (m_player_status.bonus[i] == BONUS_FIRE && g_config->christmas_mode)
    {
      bonus_prefix = "big";
    }

    m_player_sprite_py[i] += m_player_sprite_vy[i] * dt_sec;
    m_player_sprite_vy[i] += 100 * dt_sec * Sector::get().get_gravity();
    if (m_player_sprite_py[i] >= 0) {
      m_player_sprite_py[i] = 0;
      m_player_sprite_vy[i] = 0;
      m_player_sprite[i]->set_action(bonus_prefix + "-walk-right");
    } else {

      m_player_sprite[i]->set_action(bonus_prefix + "-jump-right");
    }
    if (m_player_sprite_jump_timer[i]->check()) {
      m_player_sprite_vy[i] = -300;
      m_player_sprite_jump_timer[i]->start(graphicsRandom.randf(2,3));
    }
  }
}

void LevelIntro::draw_stats_line(DrawingContext& context, int& py, const std::string& name, const std::string& stat, bool isPerfect)
{
  std::stringstream ss;
  ss << name << ": " << stat;
  Color tcolor = isPerfect ? s_stat_perfect_color : s_stat_color;
  context.color().draw_center_text(Resources::normal_font, ss.str(), Vector(0, static_cast<float>(py)),
                                   LAYER_FOREGROUND1, tcolor);
  py += static_cast<int>(Resources::normal_font->get_height());
}

void
LevelIntro::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();

  const Statistics& stats = m_level.m_stats;
  int py = static_cast<int>(context.get_height() / 2.0f - Resources::normal_font->get_height() / 2.0f);

  context.set_ambient_color(Color(1.0f, 1.0f, 1.0f, 1.0f));
  context.color().draw_filled_rect(context.get_rect(), Color(0.0f, 0.0f, 0.0f, 1.0f), 0);

  {
    context.color().draw_center_text(Resources::normal_font, m_level.get_name(), Vector(0, static_cast<float>(py)), LAYER_FOREGROUND1, s_header_color);
    py += static_cast<int>(Resources::normal_font->get_height());
  }

  std::string author = m_level.get_author();
  if ((!author.empty()) && (author != "SuperTux Team")) {
    std::string author_text = fmt::format(fmt::runtime(_("contributed by {}")), author);
    context.color().draw_center_text(Resources::small_font, author_text, Vector(0, static_cast<float>(py)), LAYER_FOREGROUND1, s_author_color);
    py += static_cast<int>(Resources::small_font->get_height());
  }

  py += 96;

  for (int i = 0; i < static_cast<int>(m_player_sprite.size()); i++)
  {
    if (!InputManager::current()->has_corresponsing_controller(i)
        && !InputManager::current()->m_uses_keyboard[i] && i != 0)
      context.transform().alpha = 0.25f;

    float offset = (static_cast<float>(i) - static_cast<float>(m_player_sprite.size()) / 2.f + 0.5f) * 64.f;

    m_player_sprite[i]->draw(context.color(), Vector((context.get_width() - m_player_sprite[i]->get_current_hitbox_width()) / 2 - offset,
                                                static_cast<float>(py) + m_player_sprite_py[i] - m_player_sprite[i]->get_current_hitbox_height()), LAYER_FOREGROUND1);

    Color power_color = (m_player_status.bonus[i] == BONUS_FIRE ? Color(1.f, 0.7f, 0.5f) :
      m_player_status.bonus[i] == BONUS_ICE ? Color(0.7f, 1.f, 1.f) :
      m_player_status.bonus[i] == BONUS_AIR ? Color(0.7f, 1.f, 0.5f) :
      m_player_status.bonus[i] == BONUS_EARTH ? Color(1.f, 0.9f, 0.6f) :
      Color(1.f, 1.f, 1.f));

    m_player_sprite[i]->set_color(power_color);
    /*if (m_player_status.bonus[i] > GROWUP_BONUS) {
      m_santa_sprite[i]->draw(context.color(), Vector((context.get_width() - m_player_sprite[i]->get_current_hitbox_width()) / 2 - offset,
                                                  static_cast<float>(py) + m_player_sprite_py[i] - m_player_sprite[i]->get_current_hitbox_height()), LAYER_FOREGROUND1);
    }*/

    context.transform().alpha = 1.f;
  }

  py += 32;

  if (m_best_level_statistics)
  {
    context.color().draw_center_text(Resources::normal_font,
                                     std::string("- ") + _("Best Level Statistics") + std::string(" -"),
                                     Vector(0, static_cast<float>(py)),
                                     LAYER_FOREGROUND1, s_stat_hdr_color);

    py += static_cast<int>(Resources::normal_font->get_height());

    const Statistics::Preferences& preferences = m_level.m_stats.get_preferences();
    if (preferences.enable_coins)
    {
      draw_stats_line(context, py, _("Coins"),
                      Statistics::coins_to_string(m_best_level_statistics->get_coins(), stats.m_total_coins),
                      m_best_level_statistics->get_coins() >= stats.m_total_coins);
    }
    if (preferences.enable_badguys)
    {
      draw_stats_line(context, py, _("Badguys killed"),
                      Statistics::frags_to_string(m_best_level_statistics->get_badguys(), stats.m_total_badguys),
                      m_best_level_statistics->get_badguys() >= stats.m_total_badguys);
    }
    if (preferences.enable_secrets)
    {
      draw_stats_line(context, py, _("Secrets"),
                      Statistics::secrets_to_string(m_best_level_statistics->get_secrets(), stats.m_total_secrets),
                      m_best_level_statistics->get_secrets() >= stats.m_total_secrets);
    }

    bool targetTimeBeaten = m_level.m_target_time == 0.0f || (m_best_level_statistics->get_time() != 0.0f && m_best_level_statistics->get_time() < m_level.m_target_time);
    draw_stats_line(context, py, _("Best time"),
                    Statistics::time_to_string(m_best_level_statistics->get_time()), targetTimeBeaten);

    if (m_level.m_target_time != 0.0f) {
      draw_stats_line(context, py, _("Level target time"),
                      Statistics::time_to_string(m_level.m_target_time), targetTimeBeaten);
    }
  }
  py += 32;
  if (!m_level.m_note.empty()) {
    context.color().draw_center_text(Resources::normal_font, m_level.m_note, Vector(0, py), LAYER_FOREGROUND1);
  }
  
}

IntegrationStatus
LevelIntro::get_status() const
{
  IntegrationStatus status;
  status.m_details.push_back("Watching a cutscene");
  status.m_details.push_back("In level: " + m_level.get_name());
  return status;
}

void
LevelIntro::push_player()
{
  int i = static_cast<int>(m_player_sprite.size());

  if (i > InputManager::current()->get_num_users())
  {
    log_warning << "Attempt to push more players in intro scene than connected" << std::endl;
    return;
  }

  m_player_sprite.push_back(SpriteManager::current()->create("images/creatures/tux/tux.sprite"));
  m_santa_sprite.push_back(SpriteManager::current()->create("images/creatures/tux/santahat.sprite"));
  m_player_sprite_py.push_back(0);
  m_player_sprite_vy.push_back(0);
  m_player_sprite_jump_timer.push_back(std::make_unique<Timer>());

  //Show appropriate tux animation for player status.
  if (m_player_status.bonus[i] == BONUS_FIRE && g_config->christmas_mode)
  {
    m_player_sprite[i]->set_action("big-walk-right");
    m_santa_sprite[i]->set_action("default");
  }
  else
  {
    m_player_sprite[i]->set_action(m_player_status.get_bonus_prefix(i) + "-walk-right");
  }

  m_player_sprite_jump_timer[i]->start(graphicsRandom.randf(5,10));

  /* Set Tux powerup sprite action */
  //m_santa_sprite[i]->set_action(m_player_sprite[i]->get_action());
}

void
LevelIntro::pop_player()
{
  if (m_player_sprite.size() <= 1)
  {
    log_warning << "Attempt to pop last player in intro scene" << std::endl;
    return;
  }

  m_player_sprite.pop_back();
  m_santa_sprite.pop_back();
  m_player_sprite_py.pop_back();
  m_player_sprite_vy.pop_back();
  m_player_sprite_jump_timer.pop_back();
}

/* EOF */
