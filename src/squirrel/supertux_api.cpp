//  SuperTux
//  Copyright (C) 2024 Vankata453
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

#include "squirrel/supertux_api.hpp"

#include <simplesquirrel/table.hpp>
#include <simplesquirrel/vm.hpp>
#include <sqstdaux.h>

#include "audio/sound_manager.hpp"
#include "math/anchor_point.hpp"
#include "math/random.hpp"
#include "object/camera.hpp"
#include "object/player.hpp"
#include "physfs/ifile_stream.hpp"
#include "squirrel/squirrel_virtual_machine.hpp"
#include "supertux/console.hpp"
#include "supertux/debug.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/game_session.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/textscroller_screen.hpp"
#include "supertux/title_screen.hpp"
#include "worldmap/worldmap.hpp"

namespace scripting {

/**
 * @scripting
 * @summary This module contains global methods.
 */
namespace Globals {

/**
 * @scripting
 * @description Displays the value of an argument. This is useful for inspecting tables.
 * @param ANY $object
 */
static void display(const ssq::Object& object)
{
  ConsoleBuffer::output << squirrel_to_string(object) << std::endl;
}
/**
 * @scripting
 * @description Displays the contents of the current stack.
 */
static void print_stacktrace(HSQUIRRELVM vm)
{
  sqstd_printcallstack(vm);
}
/**
 * @scripting
 * @description Returns the currently running thread.
 * @returns Thread
 */
static SQInteger get_current_thread(HSQUIRRELVM vm)
{
  sq_pushthread(vm, vm);
  return 1;
}

/**
 * @scripting
 * @description Returns whether the game is in christmas mode.
 */
static bool is_christmas()
{
  return g_config->christmas_mode;
}

/**
 * @scripting
 * @description Starts a skippable cutscene.
 */
static void start_cutscene()
{
  auto session = GameSession::current();
  if (session == nullptr)
  {
    log_info << "No game session" << std::endl;
    return;
  }

  if (session->get_current_level().m_is_in_cutscene)
  {
    log_warning << "start_cutscene(): starting a new cutscene above another one, ending preceding cutscene (use end_cutscene() in scripts!)" << std::endl;

    // Remove all sounds that started playing while skipping.
    if (session->get_current_level().m_skip_cutscene)
      SoundManager::current()->stop_sounds();
  }

  session->get_current_level().m_is_in_cutscene = true;
  session->get_current_level().m_skip_cutscene = false;
}
/**
 * @scripting
 * @description Ends a skippable cutscene.
 */
static void end_cutscene()
{
  auto session = GameSession::current();
  if (session == nullptr)
  {
    log_info << "No game session." << std::endl;
    return;
  }

  if (!session->get_current_level().m_is_in_cutscene)
  {
    log_warning << "end_cutscene(): no cutscene to end, resetting status anyways." << std::endl;
  }

  // Remove all sounds that started playing while skipping.
  if (session->get_current_level().m_skip_cutscene)
    SoundManager::current()->stop_sounds();

  session->get_current_level().m_is_in_cutscene = false;
  session->get_current_level().m_skip_cutscene = false;
}
/**
 * @scripting
 * @description Checks if a skippable cutscene is currently running.
 */
static bool check_cutscene()
{
  auto session = GameSession::current();
  if (session == nullptr)
  {
    log_info << "No game session" << std::endl;
    return false;
  }

  return session->get_current_level().m_is_in_cutscene;
}

/**
 * @scripting
 * @description Suspends the script execution for a specified number of seconds.
 * @param float $seconds
 * @param bool $forced Optional, enforces waiting while a cutscene is being skipped.
 * @returns void
 */
static SQInteger wait(HSQUIRRELVM vm, float seconds, bool forced = false)
{
  ssq::VM* ssq_vm = ssq::VM::get(vm);
  assert(ssq_vm);
  if (ssq_vm && !ssq_vm->isThread()) return 0;

  if (!forced)
  {
    auto session = GameSession::current();
    if (session && session->get_current_level().m_skip_cutscene)
    {
      if (ssq_vm && ssq_vm->getForeignPtr())
      {
        auto squirrelenv = ssq_vm->getForeignPtr<SquirrelEnvironment>();
        // Wait anyways, to prevent scripts like `while (true) {wait(0.1); ...}`.
        return squirrelenv->wait_for_seconds(vm, 0);
      }
      auto squirrelvm = ssq::VM::getMain(vm).getForeignPtr<SquirrelVirtualMachine>();
      return squirrelvm->wait_for_seconds(vm, 0);
    }
    if (session && session->get_current_level().m_is_in_cutscene)
    {
      if (ssq_vm && ssq_vm->getForeignPtr())
      {
        auto squirrelenv = ssq_vm->getForeignPtr<SquirrelEnvironment>();
        // Wait anyways, to prevent scripts like `while (true) {wait(0.1); ...}` from freezing the game.
        return squirrelenv->skippable_wait_for_seconds(vm, seconds);
      }
      auto squirrelvm = ssq::VM::getMain(vm).getForeignPtr<SquirrelVirtualMachine>();
      return squirrelvm->skippable_wait_for_seconds(vm, seconds);
    }
  }
  if (ssq_vm && ssq_vm->getForeignPtr())
  {
    auto squirrelenv = ssq_vm->getForeignPtr<SquirrelEnvironment>();
    return squirrelenv->wait_for_seconds(vm, seconds);
  }
  auto squirrelvm = ssq::VM::getMain(vm).getForeignPtr<SquirrelVirtualMachine>();
  return squirrelvm->wait_for_seconds(vm, seconds);
}

/**
 * @scripting
 * @description Suspends the script execution until the current screen has been changed.
 * @returns void
 */
static SQInteger wait_for_screenswitch(HSQUIRRELVM vm)
{
  auto squirrelvm = ssq::VM::getMain(vm).getForeignPtr<SquirrelVirtualMachine>();
  return squirrelvm->wait_for_screenswitch(vm);
}

/**
 * @scripting
 * @description Translates a text into the user's language (by looking in the "".po"" files).
 * @param string $text
 */
static std::string translate(const std::string& text)
{
  return g_dictionary_manager->get_dictionary().translate(text);
}
#ifdef DOXYGEN_SCRIPTING
/**
 * @scripting
 * @description Same function as ""translate()"".
 * @param string $text
 */
static std::string _(const std::string& text)
{
}
#endif
/**
 * @scripting
 * @description Translates a text into the user's language (by looking in the "".po"" files).
                Returns ""text"" or ""text_plural"", depending on ""num"" and the locale.
 * @param string $text
 * @param string $text_plural
 * @param int $num
 */
static std::string translate_plural(const std::string& text, const std::string& text_plural, int num)
{
  return g_dictionary_manager->get_dictionary().translate_plural(text, text_plural, num);
}
#ifdef DOXYGEN_SCRIPTING
/**
 * @scripting
 * @description Same function as ""translate_plural()"".
 * @param string $text
 * @param string $text_plural
 * @param int $num
 */
static std::string __(const std::string& text, const std::string& text_plural, int num)
{
}
#endif

/**
 * @scripting
 * @description Displays a text file and scrolls it over the screen (on next screenswitch).
 * @param string $filename
 */
static void display_text_file(const std::string& filename)
{
  ScreenManager::current()->push_screen(std::make_unique<TextScrollerScreen>(filename));
}

/**
 * @scripting
 * @description Loads and displays a worldmap (on next screenswitch), using the savegame of the current worldmap.
 * @param string $filename
 * @param string $sector Forced sector to spawn in the worldmap on. Leave empty to use last sector from savegame.
 * @param string $spawnpoint Forced spawnpoint to spawn in the worldmap on. Leave empty to use last position from savegame.
 */
static void load_worldmap(const std::string& filename, const std::string& sector, const std::string& spawnpoint)
{
  using namespace worldmap;

  if (!WorldMap::current())
  {
    throw std::runtime_error("Can't start Worldmap without active WorldMap");
  }
  else
  {
    WorldMap::current()->change(filename, sector, spawnpoint);
  }
}
/**
 * @scripting
 * @description Loads and displays a level (on next screenswitch), using the savegame of the current level.
 * @param string $filename
 */
static void load_level(const std::string& filename)
{
  if (!GameSession::current())
  {
    throw std::runtime_error("Can't start level without active level.");
  }
  else
  {
    ScreenManager::current()->push_screen(std::make_unique<GameSession>(filename, GameSession::current()->get_savegame()));
  }
}

/**
 * @scripting
 * @description Loads a script file and executes it. This is typically used to import functions from external files.
 * @param string $filename
 */
static void import(HSQUIRRELVM vm, const std::string& filename)
{
  ssq::VM* ssq_vm = ssq::VM::get(vm);
  assert(ssq_vm);

  IFileStream in(filename);
  ssq_vm->run(ssq_vm->compileSource(in, filename.c_str()));
}

/**
 * @scripting
 * @description Enables/disables drawing of collision rectangles.
 * @param bool $enable
 */
static void debug_collrects(bool enable)
{
  g_debug.show_collision_rects = enable;
}
/**
 * @scripting
 * @description Enables/disables drawing of FPS.
 * @param bool $enable
 */
static void debug_show_fps(bool enable)
{
  g_config->show_fps = enable;
}
/**
 * @scripting
 * @description Enables/disables drawing of non-solid layers.
 * @param bool $enable
 */
static void debug_draw_solids_only(bool enable)
{
  ::Sector::s_draw_solids_only = enable;
}
/**
 * @scripting
 * @description Enables/disables drawing of editor images.
 * @param bool $enable
 */
static void debug_draw_editor_images(bool enable)
{
  Tile::draw_editor_images = enable;
}
/**
 * @scripting
 * @description Enables/disables worldmap ghost mode.
 * @param bool $enable
 */
static void debug_worldmap_ghost(bool enable)
{
  auto worldmap_sector = worldmap::WorldMapSector::current();

  if (worldmap_sector == nullptr)
    throw std::runtime_error("Can't change ghost mode without active WorldMapSector.");

  auto& tux = worldmap_sector->get_singleton_by_type<worldmap::Tux>();
  tux.set_ghost_mode(enable);
}
/**
 * @scripting
 * @description Sets the game speed to ""speed"".
 * @param float $speed
 */
static void set_game_speed(float speed)
{
  if (speed < 0.05f)
  {
    // Always put a minimum speed above 0 - if the user enabled transitions,
    // executing transitions would take an unreaonably long time if we allow
    // game speeds like 0.00001.
    log_warning << "Cannot set game speed to less than 0.05" << std::endl;
    throw std::runtime_error("Cannot set game speed to less than 0.05");
  }

  ::g_debug.set_game_speed_multiplier(speed);
}

/**
 * @scripting
 * @description Saves world state to scripting table.
 */
static void save_state()
{
  auto worldmap = worldmap::WorldMap::current();

  if (!worldmap)
  {
    throw std::runtime_error("Can't save state without active Worldmap.");
  }
  else
  {
    worldmap->save_state();
  }
}
/**
 * @scripting
 * @description Loads world state from scripting table.
 */
static void load_state()
{
  auto worldmap = worldmap::WorldMap::current();

  if (!worldmap)
  {
    throw std::runtime_error("Can't save state without active Worldmap.");
  }
  else
  {
    worldmap->load_state();
  }
}

/**
 * @scripting
 * @description Changes the music to ""musicfile"".
 * @param string $musicfile
 */
static void play_music(const std::string& filename)
{
  SoundManager::current()->play_music(filename);
}
/**
 * @scripting
 * @description Fades in the music from ""musicfile"" for ""fadetime"" seconds.
 * @param string $musicfile
 * @param float $fadetime
 */
static void fade_in_music(const std::string& filename, float fadetime)
{
  SoundManager::current()->play_music(filename, fadetime);
}
/**
 * @scripting
 * @description Fades out the music for ""fadetime"" seconds.
 * @param float $fadetime Set to "0" for no fade-out.
 */
static void stop_music(float fadetime)
{
  SoundManager::current()->stop_music(fadetime);
}
/**
 * @scripting
 * @description Resumes and fades in the music for ""fadetime"" seconds.
 * @param float $fadetime Set to "0" for no fade-in.
 */
static void resume_music(float fadetime)
{
  SoundManager::current()->resume_music(fadetime);
}
/**
 * @scripting
 * @description Pauses the music with a fade-out for ""fadetime"" seconds.
 * @param float $fadetime Set to "0" for no fade-out.
 */
static void pause_music(float fadetime)
{
  SoundManager::current()->pause_music(fadetime);
}
/**
 * @scripting
 * @description Plays ""soundfile"" as a sound.
 * @param string $soundfile
 */
static void play_sound(const std::string& filename)
{
  SoundManager::current()->play(filename);
}

/**
 * @scripting
 * @description Speeds Tux up.
 */
static void grease()
{
  if (!Sector::current()) return;
  // FIXME: This only has effect on the first player.
  ::Player& tux = *(::Sector::get().get_players()[0]); // scripting::Player != ::Player
  tux.get_physic().set_velocity_x(tux.get_physic().get_velocity_x()*3);
}
/**
 * @scripting
 * @description Makes Tux invincible for 10000 units of time.
 */
static void invincible()
{
  if (!Sector::current()) return;
  // FIXME: This only has effect on the first player.
  ::Player& tux = *(::Sector::get().get_players()[0]);
  tux.m_invincible_timer.start(10000);
}
/**
 * @scripting
 * @description Makes Tux a ghost, i.e. lets him float around and through solid objects.
 */
static void ghost()
{
  if (!Sector::current()) return;
  // FIXME: This only has effect on the first player.
  ::Player& tux = *(::Sector::get().get_players()[0]);
  tux.set_ghost_mode(true);
}
/**
 * @scripting
 * @description Recalls Tux's invincibility and ghost status.
 */
static void mortal()
{
  if (!Sector::current()) return;
  // FIXME: This only has effect on the first player.
  ::Player& tux = *(::Sector::get().get_players()[0]);
  tux.m_invincible_timer.stop();
  tux.set_ghost_mode(false);
}
/**
 * @scripting
 * @description Re-initializes and respawns Tux at the beginning of the current level.
 */
static void restart()
{
  auto session = GameSession::current();
  if (session == nullptr)
  {
    log_info << "No game session." << std::endl;
    return;
  }
  session->reset_button = true;
}
/**
 * @scripting
 * @description Moves Tux near the end of the current level.
 */
static void gotoend()
{
  if (!Sector::current()) return;
  // FIXME: This only has effect on the first player.
  ::Player& tux = *(::Sector::get().get_players()[0]);
  tux.set_pos(Vector((::Sector::get().get_width()) - (static_cast<float>(SCREEN_WIDTH) * 2.0f), 0));
  ::Sector::get().get_camera().reset(
    Vector(tux.get_pos().x, tux.get_pos().y));
}
/**
 * @scripting
 * @description Moves Tux to the X and Y blocks, relative to his position.
 * @param float $offset_x
 * @param float $offset_y
 */
static void warp(float offset_x, float offset_y)
{
  if (!Sector::current()) return;
  // FIXME: This only has effect on the first player.
  ::Player& tux = *(::Sector::get().get_players()[0]);
  tux.set_pos(Vector(tux.get_pos().x + (offset_x*32), tux.get_pos().y - (offset_y*32)));
  ::Sector::get().get_camera().reset(
    Vector(tux.get_pos().x, tux.get_pos().y));
}

/**
 * @scripting
 * @description Returns a random integer.
 */
static int rand()
{
  return gameRandom.rand();
}

/**
 * @scripting
 * @description Sets the frame, displayed on the title screen.
 * @param string $image
 */
static void set_title_frame(const std::string& image)
{
  auto title_screen = TitleScreen::current();
  if (!title_screen)
  {
    log_info << "No title screen loaded." << std::endl;
    return;
  }
  title_screen->set_frame(image);
}

} // namespace Globals


/**
 * @scripting
 * @summary The ""Level"" table provides basic controlling functions for the current level.
 * @scope global
 */
namespace Level {

/**
 * @scripting
 * @description Ends the current level.
 * @param bool $win If ""true"", the level is marked as completed if launched from a worldmap.
 */
static void finish(bool win)
{
  if (!GameSession::current()) return;
  GameSession::current()->finish(win);
}
/**
 * @scripting
 * @description Returns whether an end sequence has started. (AKA when the stats at the end are visible)
 */
static bool has_active_sequence()
{
  if (!GameSession::current()) return false;
  return GameSession::current()->has_active_sequence();
}

/**
 * @scripting
 * @description Respawns Tux in sector named ""sector"" at spawnpoint named ""spawnpoint"".${SRG_TABLENEWPARAGRAPH}
                Exceptions: If ""sector"" or ""spawnpoint"" are empty, or the specified sector does not exist, the function will bail out the first chance it gets.
                If the specified spawnpoint doesn't exist, Tux will be spawned at the spawnpoint named “main”.
                If that spawnpoint doesn't exist either, Tux will simply end up at the origin (top-left 0, 0).
 * @param string $sector
 * @param string $spawnpoint
 * @param string $transition Valid transitions are ""circle"" and ""fade"". If any other value is specified, no transition effect is drawn.
                             Optional, empty by default.
 */
static void spawn(const std::string& sector, const std::string& spawnpoint, const std::string& transition = "")
{
  if (!GameSession::current()) return;

  if (transition.empty())
  {
    GameSession::current()->respawn(sector, spawnpoint);
    return;
  }

  ScreenFade::FadeType fade_type = ScreenFade::FadeType::NONE;

  if (transition == "fade")
    fade_type = ScreenFade::FadeType::FADE;
  else if (transition == "circle")
    fade_type = ScreenFade::FadeType::CIRCLE;
  else
    log_warning << "Invalid transition type '" << transition << "'." << std::endl;

  GameSession::current()->respawn_with_fade(sector, spawnpoint, fade_type, {0.0f, 0.0f}, true);
}
#ifdef DOXYGEN_SCRIPTING
/**
 * @scripting
 * @deprecated Use ""spawn()"" instead!
 * @description Respawns Tux in sector named ""sector"" at spawnpoint named ""spawnpoint"" with the given transition ""transition"".${SRG_TABLENEWPARAGRAPH}
                Exceptions: If ""sector"" or ""spawnpoint"" are empty, or the specified sector does not exist, the function will bail out the first chance it gets.
                If the specified spawnpoint doesn't exist, Tux will be spawned at the spawnpoint named “main”.
                If that spawnpoint doesn't exist either, Tux will simply end up at the origin (top-left 0, 0).
 * @param string $sector
 * @param string $spawnpoint
 * @param string $transition Valid transitions are ""circle"" and ""fade"". If any other value is specified, no transition effect is drawn.
 */
static void spawn_transition(const std::string& sector, const std::string& spawnpoint, const std::string& transition)
{
}
#endif

/**
 * @scripting
 * @description Sets the default start spawnpoint of the level.
 * @param string $sector
 * @param string $spawnpoint
 */
static void set_start_point(const std::string& sector, const std::string& spawnpoint)
{
  if (!GameSession::current()) return;
  GameSession::current()->set_start_point(sector, spawnpoint);
}
/**
 * @scripting
 * @description Sets the default start spawn position of the level.
 * @param string $sector
 * @param float $x
 * @param float $y
 */
static void set_start_pos(const std::string& sector, float x, float y)
{
  if (!GameSession::current()) return;
  GameSession::current()->set_start_pos(sector, Vector(x, y));
}
/**
 * @scripting
 * @description Sets the default respawn spawnpoint of the level.
 * @param string $sector
 * @param string $spawnpoint
 */
static void set_respawn_point(const std::string& sector, const std::string& spawnpoint)
{
  if (!GameSession::current()) return;
  GameSession::current()->set_respawn_point(sector, spawnpoint);
}
/**
 * @scripting
 * @description Sets the default respawn position of the level.
 * @param string $sector
 * @param float $x
 * @param float $y
 */
static void set_respawn_pos(const std::string& sector, float x, float y)
{
  if (!GameSession::current()) return;
  GameSession::current()->set_respawn_pos(sector, Vector(x, y));
}

/**
 * @scripting
 * @description Flips the level vertically (i.e. top is now bottom and vice versa).
                Call again to revert the effect. Make sure the player can land on something after the level is flipped!
 */
static void flip_vertically()
{
  if (!GameSession::current()) return;
  BIND_SECTOR(::Sector::get());
  FlipLevelTransformer flip_transformer;
  flip_transformer.transform(GameSession::current()->get_current_level());
}

/**
 * @scripting
 * @description Toggles pause.
 */
static void toggle_pause()
{
  if (!GameSession::current()) return;
  GameSession::current()->toggle_pause();
}

/**
 * @scripting
 * @description Pauses the target timer.
 */
static void pause_target_timer()
{
  if (!GameSession::current()) return;
  GameSession::current()->set_target_timer_paused(true);
}
/**
 * @scripting
 * @description Resumes the target timer.
 */
static void resume_target_timer()
{
  if (!GameSession::current()) return;
  GameSession::current()->set_target_timer_paused(false);
}

/**
 * @scripting
 * @description Override the Item Pocket setting in the Level.
 * @param string $allow Can be "on", "off", or "inherit" (use the setting in the Level).
 */
static void override_item_pocket(const std::string& allow)
{
  if (!GameSession::current()) return;
  GameSession::current()->get_savegame().get_player_status().m_override_item_pocket = ::Level::get_setting_from_name(allow);
}

/**
 * @scripting
 * @description Get the override value for the Item Pocket.
 * Can be "on", "off", or "inherit" (use the setting in the Level).
 */
static std::string is_item_pocket_overridden()
{
  if (!GameSession::current()) return "off";
  return ::Level::get_setting_name(GameSession::current()->get_savegame().get_player_status().m_override_item_pocket);
}

/**
 * @scripting
 * @description Check if the Item Pocket is allowed in this level.
 */
static bool is_item_pocket_allowed()
{
  if (!GameSession::current()) return false;
  return GameSession::current()->get_savegame().get_player_status().is_item_pocket_allowed();
}

} // namespace Level

} // namespace scripting


void register_supertux_scripting_api(ssq::VM& vm)
{
  /* GameObject-s */
  GameObjectFactory::register_objects(vm);

  /* Other classes */
  GameObjectManager::register_class(vm);
  Sector::register_class(vm);
  worldmap::WorldMapSector::register_class(vm);

  /* Global constants */
  vm.setConst<int>("ANCHOR_TOP_LEFT", AnchorPoint::ANCHOR_TOP_LEFT);
  vm.setConst<int>("ANCHOR_TOP", AnchorPoint::ANCHOR_TOP);
  vm.setConst<int>("ANCHOR_TOP_RIGHT", AnchorPoint::ANCHOR_TOP_RIGHT);
  vm.setConst<int>("ANCHOR_LEFT", AnchorPoint::ANCHOR_LEFT);
  vm.setConst<int>("ANCHOR_MIDDLE", AnchorPoint::ANCHOR_MIDDLE);
  vm.setConst<int>("ANCHOR_RIGHT", AnchorPoint::ANCHOR_RIGHT);
  vm.setConst<int>("ANCHOR_BOTTOM_LEFT", AnchorPoint::ANCHOR_BOTTOM_LEFT);
  vm.setConst<int>("ANCHOR_BOTTOM", AnchorPoint::ANCHOR_BOTTOM);
  vm.setConst<int>("ANCHOR_BOTTOM_RIGHT", AnchorPoint::ANCHOR_BOTTOM_RIGHT);

  vm.setConst<int>("BONUS_NONE", BonusType::BONUS_NONE);
  vm.setConst<int>("BONUS_GROWUP", BonusType::BONUS_GROWUP);
  vm.setConst<int>("BONUS_FIRE", BonusType::BONUS_FIRE);
  vm.setConst<int>("BONUS_AIR", BonusType::BONUS_AIR);
  vm.setConst<int>("BONUS_EARTH", BonusType::BONUS_EARTH);
  vm.setConst<int>("BONUS_ICE", BonusType::BONUS_ICE);

  /* Global functions */
  vm.addFunc("display", &scripting::Globals::display);
  vm.addFunc("print_stacktrace", &scripting::Globals::print_stacktrace);
  vm.addFunc("get_current_thread", &scripting::Globals::get_current_thread);
  vm.addFunc("is_christmas", &scripting::Globals::is_christmas);
  vm.addFunc("start_cutscene", &scripting::Globals::start_cutscene);
  vm.addFunc("end_cutscene", &scripting::Globals::end_cutscene);
  vm.addFunc("check_cutscene", &scripting::Globals::check_cutscene);
  vm.addFunc("wait", &scripting::Globals::wait, ssq::DefaultArguments<bool>(false));
  vm.addFunc("wait_for_screenswitch", &scripting::Globals::wait_for_screenswitch);
  vm.addFunc("translate", &scripting::Globals::translate);
  vm.addFunc("_", &scripting::Globals::translate);
  vm.addFunc("translate_plural", &scripting::Globals::translate_plural);
  vm.addFunc("__", &scripting::Globals::translate_plural);
  vm.addFunc("display_text_file", &scripting::Globals::display_text_file);
  vm.addFunc("load_worldmap", &scripting::Globals::load_worldmap);
  vm.addFunc("load_level", &scripting::Globals::load_level);
  vm.addFunc("import", &scripting::Globals::import);
  vm.addFunc("debug_collrects", &scripting::Globals::debug_collrects);
  vm.addFunc("debug_show_fps", &scripting::Globals::debug_show_fps);
  vm.addFunc("debug_draw_solids_only", &scripting::Globals::debug_draw_solids_only);
  vm.addFunc("debug_draw_editor_images", &scripting::Globals::debug_draw_editor_images);
  vm.addFunc("debug_worldmap_ghost", &scripting::Globals::debug_worldmap_ghost);
  vm.addFunc("set_game_speed", &scripting::Globals::set_game_speed);
  vm.addFunc("save_state", &scripting::Globals::save_state);
  vm.addFunc("load_state", &scripting::Globals::load_state);
  vm.addFunc("play_music", &scripting::Globals::play_music);
  vm.addFunc("fade_in_music", &scripting::Globals::fade_in_music);
  vm.addFunc("stop_music", &scripting::Globals::stop_music);
  vm.addFunc("resume_music", &scripting::Globals::resume_music);
  vm.addFunc("pause_music", &scripting::Globals::pause_music);
  vm.addFunc("play_sound", &scripting::Globals::play_sound);
  vm.addFunc("grease", &scripting::Globals::grease);
  vm.addFunc("invincible", &scripting::Globals::invincible);
  vm.addFunc("ghost", &scripting::Globals::ghost);
  vm.addFunc("mortal", &scripting::Globals::mortal);
  vm.addFunc("restart", &scripting::Globals::restart);
  vm.addFunc("gotoend", &scripting::Globals::gotoend);
  vm.addFunc("warp", &scripting::Globals::warp);
  vm.addFunc("rand", &scripting::Globals::rand);
  vm.addFunc("set_title_frame", &scripting::Globals::set_title_frame);

  /* "Level" global functions */
  ssq::Table level = vm.addTable("Level");
  level.addFunc("finish", &scripting::Level::finish);
  level.addFunc("has_active_sequence", &scripting::Level::has_active_sequence);
  level.addFunc("spawn", &scripting::Level::spawn, ssq::DefaultArguments<const std::string&>(""));
  level.addFunc("spawn_transition", &scripting::Level::spawn); // Deprecated
  level.addFunc("set_start_point", &scripting::Level::set_start_point);
  level.addFunc("set_start_pos", &scripting::Level::set_start_pos);
  level.addFunc("set_respawn_point", &scripting::Level::set_respawn_point);
  level.addFunc("set_respawn_pos", &scripting::Level::set_respawn_pos);
  level.addFunc("flip_vertically", &scripting::Level::flip_vertically);
  level.addFunc("toggle_pause", &scripting::Level::toggle_pause);
  level.addFunc("pause_target_timer", &scripting::Level::pause_target_timer);
  level.addFunc("resume_target_timer", &scripting::Level::resume_target_timer);
  level.addFunc("override_item_pocket", &scripting::Level::override_item_pocket);
  level.addFunc("is_item_pocket_overridden", &scripting::Level::is_item_pocket_overridden);
  level.addFunc("is_item_pocket_allowed", &scripting::Level::is_item_pocket_allowed);

  /* "Level" global functions (0.6.3 backward compatibility) */
  vm.addFunc("Level_finish", &scripting::Level::finish);
  vm.addFunc("Level_spawn", &scripting::Level::spawn, ssq::DefaultArguments<const std::string&>(""));
  vm.addFunc("Level_flip_vertically", &scripting::Level::flip_vertically);
  vm.addFunc("Level_toggle_pause", &scripting::Level::toggle_pause);
}

/* EOF */
