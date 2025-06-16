//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "supertux/title_screen.hpp"

#include <version.h>

#include "gui/menu_manager.hpp"
#include "object/camera.hpp"
#include "object/music_object.hpp"
#include "object/player.hpp"
#include "sdk/integration.hpp"
#include "supertux/constants.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/game_session.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/profile.hpp"
#include "supertux/profile_manager.hpp"
#include "supertux/resources.hpp"
#include "supertux/savegame.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/world.hpp"
#include "video/compositor.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"

static const std::string DEFAULT_TITLE_LEVEL = "levels/misc/menu.stl";

TitleScreen::TitleScreen(Savegame& savegame, bool christmas) :
  m_savegame(savegame),
  m_christmas(christmas),
  m_logo(Surface::from_file("images/engine/menu/" + std::string(LOGO_FILE))),
  m_santahat(christmas ? Surface::from_file("images/engine/menu/logo_santahat.png") : nullptr),
  m_frame(Surface::from_file("images/engine/menu/frame.png")),
  m_controller(new CodeController()),
  m_titlesession(),
  m_copyright_text(),
  m_videosystem_name(VideoSystem::current()->get_name()),
  m_jump_was_released(false)
{
  refresh_copyright_text();
}

void
TitleScreen::setup()
{
  refresh_level();
  MenuManager::instance().set_menu(MenuStorage::MAIN_MENU);
  ScreenManager::current()->set_screen_fade(std::make_unique<FadeToBlack>(FadeToBlack::FADEIN, 0.25f));
}

void
TitleScreen::leave()
{
  m_titlesession->get_current_sector().deactivate();
  m_titlesession->leave();
  MenuManager::instance().clear_menu_stack();
}

void
TitleScreen::refresh_level()
{
  bool level_init = false;

  if (g_config->custom_title_levels)
  {
    /** Check the savegame of the previously entered world on the current profile.
        If there is a title screen level set, use it. Otherwise, look for such level in the world's properties.
        If a level still isn't specified, or it cannot be entered, use the default. */
    std::string title_level;

    const std::string last_world = ProfileManager::current()->get_current_profile().get_last_world();
    if (!last_world.empty())
    {
      const std::string savegame_title_level = Savegame::from_current_profile(last_world, true)->get_player_status().title_level;
      if (savegame_title_level.empty())
      {
        const auto world = World::from_directory("levels/" + last_world);
        if (world)
          title_level = world->get_title_level();
      }
      else
      {
        title_level = savegame_title_level;
      }
    }

    if (title_level.empty())
      title_level = DEFAULT_TITLE_LEVEL;

    /** Attempt to switch to the custom title level, using the default one as a fallback. */
    if (!m_titlesession || m_titlesession->get_level_file() != title_level)
    {
      std::unique_ptr<GameSession> new_session;
      try
      {
        new_session = std::make_unique<GameSession>(title_level, m_savegame, nullptr);
      }
      catch (const std::exception& err)
      {
        log_warning << "Error loading custom title screen level '" << title_level << "': " << err.what() << std::endl;

        if (!m_titlesession || m_titlesession->get_level_file() != DEFAULT_TITLE_LEVEL)
        {
          new_session = std::make_unique<GameSession>(DEFAULT_TITLE_LEVEL, m_savegame, nullptr);
        }
      }
      new_session->restart_level(false, true);
      if (new_session)
      {
        m_titlesession = std::move(new_session);
        level_init = true;
      }
    }
  }
  else if (!m_titlesession || m_titlesession->get_level_file() != DEFAULT_TITLE_LEVEL)
  {
    m_titlesession = std::make_unique<GameSession>(DEFAULT_TITLE_LEVEL, m_savegame, nullptr);
    m_titlesession->restart_level(false, true);
    level_init = true;
  }

  /** Initialize the main sector. */
  Sector& sector = m_titlesession->get_current_sector();
  if (level_init || Sector::current() != &sector)
    setup_sector(sector);
}

void
TitleScreen::setup_sector(Sector& sector)
{
  auto& music = sector.get_singleton_by_type<MusicObject>();
  music.resume_music(true);

  Player& player = *(sector.get_players()[0]);

  // sector.activate(Vector) expects position calculated for big tux, but tux
  // might be small on the title screen
  sector.activate(player.get_pos() - Vector(0.f, player.is_big() ? 0.f : 32.f));

  player.set_controller(m_controller.get());
  player.set_speedlimit(230.f);
}

void
TitleScreen::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();

  m_titlesession->get_current_sector().draw(context);

  context.color().draw_surface(m_logo,
                               Vector(context.get_width() / 2 - static_cast<float>(m_logo->get_width()) / 2,
                                      context.get_height() / 2 - static_cast<float>(m_logo->get_height()) / 2 - 200.f),
                               LAYER_GUI + 1);
  if (m_christmas)
  {
    context.color().draw_surface(m_santahat,
                                 Vector(context.get_width() / 2 - static_cast<float>(m_santahat->get_width()) / 2 + 35.f,
                                        context.get_height() / 2 - static_cast<float>(m_santahat->get_height()) / 2 - 255.f),
                                 LAYER_GUI + 2);
  }

  context.color().draw_surface_scaled(m_frame, context.get_rect(), LAYER_GUI + 3);

  context.color().draw_text(Resources::small_font,
                            m_copyright_text,
                            Vector(5.0f, context.get_height() - 50.0f),
                            ALIGN_LEFT, LAYER_GUI + 4);

  context.color().draw_text(Resources::small_font,
                            m_videosystem_name,
                            Vector(context.get_width() - 5.0f,
                                   context.get_height() - 14.0f),
                            ALIGN_RIGHT, LAYER_GUI + 4);
}

void
TitleScreen::update(float dt_sec, const Controller& controller)
{
  ScreenManager::current()->set_speed(0.6f);

  update_level(dt_sec);

  // Re-open main menu, if it was closed
  if (!MenuManager::instance().is_active() && !ScreenManager::current()->has_pending_fadeout())
    MenuManager::instance().set_menu(MenuStorage::MAIN_MENU);
}

void
TitleScreen::update_level(float dt_sec)
{
  using RaycastResult = CollisionSystem::RaycastResult;

  Sector& sector = m_titlesession->get_current_sector();
  Player& player = *(sector.get_players()[0]);

  // Restart level if Tux is dying
  if (player.is_dying())
  {
    m_titlesession->restart_level(true, true);
    setup_sector(m_titlesession->get_current_sector());
    return;
  }

  BIND_SECTOR(sector);
  sector.update(dt_sec);

  m_controller->update();
  m_controller->press(Control::RIGHT);

  // Check if we should press the jump button
  const Rectf& bbox = player.get_bbox();
  const Vector eye(bbox.get_right(), bbox.get_top() + bbox.get_height() / 2);
  const Vector end(eye.x + 46.f, eye.y);

  RaycastResult result = sector.get_first_line_intersection(eye, end, false, player.get_collision_object());

  bool shouldjump = result.is_valid;
  if (shouldjump)
  {
    if (auto tile = std::get_if<const Tile*>(&result.hit))
      shouldjump = !(*tile)->is_slope();
    else if (auto obj = std::get_if<CollisionObject*>(&result.hit))
      shouldjump = ((*obj)->get_group() == COLGROUP_STATIC ||
                    (*obj)->get_group() == COLGROUP_MOVING_STATIC);
  }

  if (player.m_fall_mode == Player::FallMode::JUMPING ||
      (m_jump_was_released && shouldjump))
  {
    m_controller->press(Control::JUMP);
    m_jump_was_released = false;
  }
  else
  {
    m_jump_was_released = true;
  }

  // Wrap around at the end of the level back to the beginning
  if (sector.get_width() - 320.f < player.get_pos().x)
  {
    sector.activate(DEFAULT_SECTOR_NAME);
    sector.get_camera().reset(player.get_pos());
  }
}

void
TitleScreen::refresh_copyright_text()
{
  // cppcheck-suppress unknownMacro
  m_copyright_text = "SuperTux " PACKAGE_VERSION "\n" +
    _("Copyright") + " (c) 2003-2025 SuperTux Devel Team\n" +
    _("This game comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to\n"
      "redistribute it under certain conditions; see the license file for details.\n");
}

void
TitleScreen::set_frame(const std::string& image)
{
  m_frame = Surface::from_file(image);
}

IntegrationStatus
TitleScreen::get_status() const
{
  IntegrationStatus status;
  status.m_details.push_back("In main menu");
  return status;
}

/* EOF */
