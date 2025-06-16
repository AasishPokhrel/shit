//  SuperTux -  A Jump'n Run
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

#include "supertux/gameconfig.hpp"

#include <ctime>

#include "editor/overlay_widget.hpp"
#include "math/util.hpp"
#include "supertux/colorscheme.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "util/log.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

Config::Config() :
  m_initial(true),
  profile(1),
  fullscreen_size(0, 0),
  fullscreen_refresh_rate(0),
  window_size(1280, 800),
  window_resizable(true),
  aspect_size(0, 0), // Auto detect.
#ifdef __EMSCRIPTEN__
  fit_window(true),
#endif
  magnification(0.0f),
  // Ubuntu Touch supports windowed apps.
#ifdef __ANDROID__
  use_fullscreen(true),
#else
  use_fullscreen(false),
#endif
  video(VideoSystem::VIDEO_SDL),
  vsync(1),
  frame_prediction(false),
  show_fps(false),
  show_player_pos(false),
  show_controller(false),
  camera_peek_multiplier(0.03f),
  sound_enabled(true),
  music_enabled(true),
  sound_volume(100),
  music_volume(50),
  flash_intensity(50),
  random_seed(0), // Set by time(), by default (unless in config).
  enable_script_debugger(false),
  tux_spawn_pos(),
  locale(),
  keyboard_config(),
  joystick_config(),
  mobile_controls(SDL_GetNumTouchDevices() > 0),
  m_mobile_controls_scale(1),
  addons(),
  developer_mode(false),
  christmas_mode(false),
  transitions_enabled(true),
  confirmation_dialog(false),
  pause_on_focusloss(true),
  custom_mouse_cursor(true),
  do_release_check(false),
  disable_network(true),
  custom_title_levels(true),
#ifdef ENABLE_DISCORD
  enable_discord(false),
#endif
  hide_editor_levelnames(false),
  notifications(),
  menubackcolor(ColorScheme::Menu::back_color),
  menufrontcolor(ColorScheme::Menu::front_color),
  menuhelpbackcolor(ColorScheme::Menu::help_back_color),
  menuhelpfrontcolor(ColorScheme::Menu::help_front_color),
  labeltextcolor(ColorScheme::Menu::label_color),
  activetextcolor(ColorScheme::Menu::active_color),
  hlcolor(ColorScheme::Menu::hl_color),
  editorcolor(ColorScheme::Editor::default_color),
  editorhovercolor(ColorScheme::Editor::hover_color),
  editorgrabcolor(ColorScheme::Editor::grab_color),
  menuroundness(16.f),
  editor_selected_snap_grid_size(3),
  editor_render_grid(true),
  editor_snap_to_grid(true),
  editor_render_background(true),
  editor_render_lighting(false),
  editor_autotile_mode(false),
  editor_autotile_help(true),
  editor_autosave_frequency(5),
  editor_undo_tracking(true),
  editor_undo_stack_size(20),
  editor_show_deprecated_tiles(false),
  multiplayer_auto_manage_players(true),
  multiplayer_multibind(false),
#if SDL_VERSION_ATLEAST(2, 0, 9)
  multiplayer_buzz_controllers(true),
#else
  // Will be loaded and saved anyways, to retain the setting. This is helpful
  // for users who frequently switch between versions compiled with a newer SDL
  // and those with an older SDL; they won't have to check the setting each time.
  multiplayer_buzz_controllers(false),
#endif
  repository_url()
{
}

void
Config::load()
{
#ifdef __EMSCRIPTEN__
  EM_ASM({
    supertux_loadFiles();
  }, 0); // EM_ASM is a variadic macro and Clang requires at least 1 value for the variadic argument.
#endif

  auto doc = ReaderDocument::from_file("config");
  auto root = doc.get_root();
  if (root.get_name() != "supertux-config")
  {
    throw std::runtime_error("File is not a supertux-config file");
  }

  auto config_mapping = root.get_mapping();
  config_mapping.get("profile", profile);

  config_mapping.get("flash_intensity", flash_intensity);
  config_mapping.get("frame_prediction", frame_prediction);
  config_mapping.get("show_fps", show_fps);
  config_mapping.get("show_player_pos", show_player_pos);
  config_mapping.get("show_controller", show_controller);
  config_mapping.get("camera_peek_multiplier", camera_peek_multiplier);
  config_mapping.get("developer", developer_mode);
  config_mapping.get("confirmation_dialog", confirmation_dialog);
  config_mapping.get("pause_on_focusloss", pause_on_focusloss);
  config_mapping.get("custom_mouse_cursor", custom_mouse_cursor);
  config_mapping.get("do_release_check", do_release_check);
  config_mapping.get("disable_network", disable_network);
  config_mapping.get("custom_title_levels", custom_title_levels);

  std::optional<ReaderMapping> config_integrations_mapping;
  if (config_mapping.get("integrations", config_integrations_mapping))
  {
    config_integrations_mapping->get("hide_editor_levelnames", hide_editor_levelnames);
#ifdef ENABLE_DISCORD
    config_integrations_mapping->get("enable_discord", enable_discord);
#endif
  }

  std::optional<ReaderCollection> config_notifications_mapping;
  if (config_mapping.get("notifications", config_notifications_mapping))
  {
    for (auto const& notification_node : config_notifications_mapping->get_objects())
    {
      if (notification_node.get_name() == "notification")
      {
        auto notification = notification_node.get_mapping();

        std::string id;
        bool disabled = false;
        if (notification.get("id", id) &&
            notification.get("disabled", disabled))
        {
          notifications.push_back({id, disabled});
        }
      }
      else
      {
        log_warning << "Unknown token in config file: " << notification_node.get_name() << std::endl;
      }
    }
  }

  // Menu colors.

  std::vector<float> menubackcolor_, menufrontcolor_, menuhelpbackcolor_, menuhelpfrontcolor_,
    labeltextcolor_, activetextcolor_, hlcolor_, editorcolor_, editorhovercolor_, editorgrabcolor_;

  std::optional<ReaderMapping> interface_colors_mapping;
  if (config_mapping.get("interface_colors", interface_colors_mapping))
  {
    interface_colors_mapping->get("menubackcolor", menubackcolor_, ColorScheme::Menu::back_color.toVector());
    interface_colors_mapping->get("menufrontcolor", menufrontcolor_, ColorScheme::Menu::front_color.toVector());
    interface_colors_mapping->get("menuhelpbackcolor", menuhelpbackcolor_, ColorScheme::Menu::help_back_color.toVector());
    interface_colors_mapping->get("menuhelpfrontcolor", menuhelpfrontcolor_, ColorScheme::Menu::help_back_color.toVector());
    interface_colors_mapping->get("labeltextcolor", labeltextcolor_, ColorScheme::Menu::label_color.toVector());
    interface_colors_mapping->get("activetextkcolor", activetextcolor_, ColorScheme::Menu::active_color.toVector());
    interface_colors_mapping->get("hlcolor", hlcolor_, ColorScheme::Menu::hl_color.toVector());
    interface_colors_mapping->get("editorcolor", editorcolor_, ColorScheme::Editor::default_color.toVector());
    interface_colors_mapping->get("editorhovercolor", editorhovercolor_, ColorScheme::Editor::hover_color.toVector());
    interface_colors_mapping->get("editorgrabcolor", editorgrabcolor_, ColorScheme::Editor::grab_color.toVector());
    menubackcolor = Color(menubackcolor_);
    menufrontcolor = Color(menufrontcolor_);
    menuhelpbackcolor = Color(menuhelpbackcolor_);
    menuhelpfrontcolor = Color(menuhelpfrontcolor_);
    labeltextcolor = Color(labeltextcolor_);
    activetextcolor = Color(activetextcolor_);
    hlcolor = Color(hlcolor_);
    editorcolor = Color(editorcolor_);
    editorhovercolor = Color(editorhovercolor_);
    editorgrabcolor = Color(editorgrabcolor_);
    interface_colors_mapping->get("menuroundness", menuroundness, 16.f);
  }

  // Compatibility; will be overwritten by the "editor" category.
  
  config_mapping.get("editor_autosave_frequency", editor_autosave_frequency);

  editor_autotile_help = !developer_mode;

  std::optional<ReaderMapping> editor_mapping;
  if (config_mapping.get("editor", editor_mapping))
  {
    editor_mapping->get("autosave_frequency", editor_autosave_frequency);
    editor_mapping->get("autotile_help", editor_autotile_help);
    editor_mapping->get("autotile_mode", editor_autotile_mode);
    editor_mapping->get("render_background", editor_render_background);
    editor_mapping->get("render_grid", editor_render_grid);
    editor_mapping->get("render_lighting", editor_render_lighting);
    editor_mapping->get("selected_snap_grid_size", editor_selected_snap_grid_size);
    editor_mapping->get("snap_to_grid", editor_snap_to_grid);
    editor_mapping->get("undo_tracking", editor_undo_tracking);
    editor_mapping->get("undo_stack_size", editor_undo_stack_size);
    if (editor_undo_stack_size < 1)
    {
      log_warning << "Undo stack size could not be lower than 1. Setting to lowest possible value (1)." << std::endl;
      editor_undo_stack_size = 1;
    }
    editor_mapping->get("show_deprecated_tiles", editor_show_deprecated_tiles);
  }

  if (is_christmas()) {
    config_mapping.get("christmas", christmas_mode, true);
  }
  config_mapping.get("transitions_enabled", transitions_enabled);
  config_mapping.get("locale", locale);
  config_mapping.get("random_seed", random_seed);
  config_mapping.get("repository_url", repository_url);

  config_mapping.get("multiplayer_auto_manage_players", multiplayer_auto_manage_players);
  config_mapping.get("multiplayer_multibind", multiplayer_multibind);
  config_mapping.get("multiplayer_buzz_controllers", multiplayer_buzz_controllers);

  std::optional<ReaderMapping> config_video_mapping;
  if (config_mapping.get("video", config_video_mapping))
  {
    config_video_mapping->get("fullscreen", use_fullscreen);
    std::string video_string;
    config_video_mapping->get("video", video_string);
    video = VideoSystem::get_video_system(video_string);
    config_video_mapping->get("vsync", vsync);

    config_video_mapping->get("fullscreen_width",  fullscreen_size.width);
    config_video_mapping->get("fullscreen_height", fullscreen_size.height);
    if (fullscreen_size.width < 0 || fullscreen_size.height < 0)
    {
      // Somehow, an invalid size got entered into the config file,
      // let's use the "auto" setting instead.
      fullscreen_size = Size(0, 0);
    }
    config_video_mapping->get("fullscreen_refresh_rate", fullscreen_refresh_rate);

    config_video_mapping->get("window_width",  window_size.width);
    config_video_mapping->get("window_height", window_size.height);

    config_video_mapping->get("window_resizable", window_resizable);

    config_video_mapping->get("aspect_width",  aspect_size.width);
    config_video_mapping->get("aspect_height", aspect_size.height);

    config_video_mapping->get("magnification", magnification);

#ifdef __EMSCRIPTEN__
    // Forcibly set autofit to true.
    // TODO: Remove the autofit parameter entirely - it should always be true.

    //config_video_mapping->get("fit_window", fit_window);
    fit_window = true;
#endif
  }

  std::optional<ReaderMapping> config_audio_mapping;
  if (config_mapping.get("audio", config_audio_mapping))
  {
    config_audio_mapping->get("sound_enabled", sound_enabled);
    config_audio_mapping->get("music_enabled", music_enabled);
    config_audio_mapping->get("sound_volume", sound_volume);
    config_audio_mapping->get("music_volume", music_volume);
  }

  std::optional<ReaderMapping> config_control_mapping;
  if (config_mapping.get("control", config_control_mapping))
  {
    std::optional<ReaderMapping> keymap_mapping;
    if (config_control_mapping->get("keymap", keymap_mapping))
    {
      keyboard_config.read(*keymap_mapping);
    }

    std::optional<ReaderMapping> joystick_mapping;
    if (config_control_mapping->get("joystick", joystick_mapping))
    {
      joystick_config.read(*joystick_mapping);
    }

    config_control_mapping->get("mobile_controls", mobile_controls, SDL_GetNumTouchDevices() > 0);
    config_control_mapping->get("mobile_controls_scale", m_mobile_controls_scale, 1);
  }

  std::optional<ReaderCollection> config_addons_mapping;
  if (config_mapping.get("addons", config_addons_mapping))
  {
    for (auto const& addon_node : config_addons_mapping->get_objects())
    {
      if (addon_node.get_name() == "addon")
      {
        auto addon = addon_node.get_mapping();

        std::string id;
        bool enabled = false;
        if (addon.get("id", id) &&
            addon.get("enabled", enabled))
        {
          addons.push_back({id, enabled});
        }
      }
      else
      {
        log_warning << "Unknown token in config file: " << addon_node.get_name() << std::endl;
      }
    }
  }

  check_values();
  m_initial = false;
}

void
Config::save()
{
  check_values();

  Writer writer("config");

  writer.start_list("supertux-config");

  writer.write("profile", profile);

  writer.write("frame_prediction", frame_prediction);
  writer.write("show_fps", show_fps);
  writer.write("show_player_pos", show_player_pos);
  writer.write("show_controller", show_controller);
  writer.write("camera_peek_multiplier", camera_peek_multiplier);
  writer.write("developer", developer_mode);
  writer.write("confirmation_dialog", confirmation_dialog);
  writer.write("pause_on_focusloss", pause_on_focusloss);
  writer.write("custom_mouse_cursor", custom_mouse_cursor);
  writer.write("do_release_check", do_release_check);
  writer.write("disable_network", disable_network);
  writer.write("custom_title_levels", custom_title_levels);

  writer.start_list("integrations");
  {
    writer.write("hide_editor_levelnames", hide_editor_levelnames);
#ifdef ENABLE_DISCORD
    writer.write("enable_discord", enable_discord);
#endif
  }
  writer.end_list("integrations");

  writer.start_list("notifications");
  for (const auto& notification : notifications)
  {
    writer.start_list("notification");
    writer.write("id", notification.id);
    writer.write("disabled", notification.disabled);
    writer.end_list("notification");
  }
  writer.end_list("notifications");

  writer.write("editor_autosave_frequency", editor_autosave_frequency);

  if (is_christmas()) {
    writer.write("christmas", christmas_mode);
  }
  writer.write("transitions_enabled", transitions_enabled);
  writer.write("locale", locale);
  writer.write("repository_url", repository_url);
  writer.write("multiplayer_auto_manage_players", multiplayer_auto_manage_players);
  writer.write("multiplayer_multibind", multiplayer_multibind);
  writer.write("multiplayer_buzz_controllers", multiplayer_buzz_controllers);

  writer.start_list("interface_colors");
  writer.write("menubackcolor", menubackcolor.toVector());
  writer.write("menufrontcolor", menufrontcolor.toVector());
  writer.write("menuhelpbackcolor", menuhelpbackcolor.toVector());
  writer.write("menuhelpfrontcolor", menuhelpfrontcolor.toVector());
  writer.write("labeltextcolor", labeltextcolor.toVector());
  writer.write("activetextcolor", activetextcolor.toVector());
  writer.write("hlcolor", hlcolor.toVector());
  writer.write("editorcolor", editorcolor.toVector());
  writer.write("editorhovercolor", editorhovercolor.toVector());
  writer.write("editorgrabcolor", editorgrabcolor.toVector());
  writer.write("menuroundness", menuroundness);
  writer.end_list("interface_colors");

  writer.start_list("video");
  writer.write("fullscreen", use_fullscreen);
  if (video == VideoSystem::VIDEO_NULL) {
    // Avoid saving a NULL renderer to the configuration, as starting SuperTux without
    // getting a window is rather confusing.
  } else {
    writer.write("video", VideoSystem::get_video_string(video));
  }
  writer.write("vsync", vsync);

  writer.write("fullscreen_width",  fullscreen_size.width);
  writer.write("fullscreen_height", fullscreen_size.height);
  writer.write("fullscreen_refresh_rate", fullscreen_refresh_rate);

  writer.write("window_width",  window_size.width);
  writer.write("window_height", window_size.height);

  writer.write("window_resizable", window_resizable);

  writer.write("aspect_width",  aspect_size.width);
  writer.write("aspect_height", aspect_size.height);

  writer.write("flash_intensity", flash_intensity);

#ifdef __EMSCRIPTEN__
  // Forcibly set autofit to true
  // TODO: Remove the autofit parameter entirely - it should always be true
  writer.write("fit_window", true /* fit_window */);
#endif

  writer.write("magnification", magnification);

  writer.end_list("video");

  writer.start_list("audio");
  writer.write("sound_enabled", sound_enabled);
  writer.write("music_enabled", music_enabled);
  writer.write("sound_volume", sound_volume);
  writer.write("music_volume", music_volume);
  writer.end_list("audio");

  writer.start_list("control");
  {
    writer.start_list("keymap");
    keyboard_config.write(writer);
    writer.end_list("keymap");

    writer.start_list("joystick");
    joystick_config.write(writer);
    writer.end_list("joystick");

    writer.write("mobile_controls", mobile_controls);
    writer.write("mobile_controls_scale", m_mobile_controls_scale);
  }
  writer.end_list("control");

  writer.start_list("addons");
  for (const auto& addon : addons)
  {
    writer.start_list("addon");
    writer.write("id", addon.id);
    writer.write("enabled", addon.enabled);
    writer.end_list("addon");
  }
  writer.end_list("addons");

  writer.start_list("editor");
  {
    writer.write("autosave_frequency", editor_autosave_frequency);
    writer.write("autotile_help", editor_autotile_help);
    writer.write("autotile_mode", editor_autotile_mode);
    writer.write("render_background", editor_render_background);
    writer.write("render_grid", editor_render_grid);
    writer.write("render_lighting", editor_render_lighting);
    writer.write("selected_snap_grid_size", editor_selected_snap_grid_size);
    writer.write("snap_to_grid", editor_snap_to_grid);
    writer.write("undo_tracking", editor_undo_tracking);
    writer.write("undo_stack_size", editor_undo_stack_size);
    writer.write("show_deprecated_tiles", editor_show_deprecated_tiles);
  }
  writer.end_list("editor");

  writer.end_list("supertux-config");
}

void
Config::check_values()
{
  camera_peek_multiplier = math::clamp(camera_peek_multiplier, 0.f, 1.f);
}

bool
Config::is_christmas() const
{
  if (christmas_mode)
    return true;

  std::time_t time = std::time(nullptr);
  const std::tm* now = std::localtime(&time);

  /* Activate Christmas mode from Dec 6th until Dec 31st. */
  return now->tm_mday >= 6 && now->tm_mon == 11;
}

/* EOF */
