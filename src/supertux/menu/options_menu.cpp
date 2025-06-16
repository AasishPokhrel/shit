//  SuperTux
//  Copyright (C) 2004 Tobas Glaesser <tobi.web@gmx.de>
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

#include "supertux/menu/options_menu.hpp"

#include "audio/sound_manager.hpp"
#include "gui/dialog.hpp"
#include "gui/item_floatfield.hpp"
#include "gui/item_goto.hpp"
#include "gui/item_stringselect.hpp"
#include "gui/item_toggle.hpp"
#include "gui/menu_item.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/game_session.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/title_screen.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

#include <sstream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

bool
OptionsMenu::less_than_volume(const std::string& lhs, const std::string& rhs)
{
  int lhs_i, rhs_i;
  if (sscanf(lhs.c_str(), "%i", &lhs_i) == 1 &&
      sscanf(rhs.c_str(), "%i", &rhs_i) == 1)
  {
    return lhs_i < rhs_i;
  }

  return false;
}

OptionsMenu::OptionsMenu(Type type, bool complete) :
  m_type(type),
  m_complete(complete),
  m_magnifications(),
  m_aspect_ratios(),
  m_window_resolutions(),
  m_resolutions(),
  m_vsyncs(),
  m_sound_volumes(),
  m_music_volumes(),
  m_flash_intensity_values(),
  m_mobile_control_scales()
{
  refresh();
}

void
OptionsMenu::refresh()
{
  clear();

  switch (m_type) // Insert label and menu items, appropriate for the chosen OptionsMenu type
  {
    case LOCALE: /** LOCALE */
    {
      insert_label(_("Locale"));

      if (m_complete)
      {
        add_submenu(_("Select Language"), MenuStorage::LANGUAGE_MENU)
          .set_help(_("Select a different language to display text in"));

        add_submenu(_("Language Packs"), MenuStorage::LANGPACK_MENU)
          .set_help(_("Language packs contain up-to-date translations"));
      }

      break;
    }

    case VIDEO: /** VIDEO */
    {
      insert_label(_("Video"));

#if !defined(HIDE_NONMOBILE_OPTIONS) && !defined(__EMSCRIPTEN__)
      add_toggle(MNID_FULLSCREEN,_("Window Resizable"), &g_config->window_resizable)
        .set_help(_("Allow window resizing, might require a restart to take effect"));

      add_window_resolutions();

      add_toggle(MNID_FULLSCREEN,_("Fullscreen"), &g_config->use_fullscreen)
        .set_help(_("Fill the entire screen"));

      add_resolutions();
#endif

#if 0
#ifdef __EMSCRIPTEN__
      add_toggle(MNID_FIT_WINDOW, _("Fit to browser"), &g_config->fit_window)
        .set_help(_("Fit the resolution to the size of your browser"));
#endif
#endif

      add_magnification();
      add_vsync();

      add_toggle(MNID_FRAME_PREDICTION, _("Frame prediction"), &g_config->frame_prediction)
        .set_help(_("Smooth camera motion, generating intermediate frames. This has a noticeable effect on monitors at >> 60Hz. Moving objects may be blurry."));

      add_flash_intensity();

#if !defined(HIDE_NONMOBILE_OPTIONS) && !defined(__EMSCRIPTEN__)
      add_aspect_ratio();
#endif

      add_floatfield(_("Camera Peek Multiplier"), &g_config->camera_peek_multiplier)
        .set_help(_("The fractional distance towards the camera peek position to move each frame.\n\n0 = No Peek, 1 = Instant Peek"));

      add_submenu(_("Change Video System"), MenuStorage::MenuId::VIDEO_SYSTEM_MENU)
        .set_help(_("Change video system used to render graphics"));
      break;
    }

    case AUDIO: /** AUDIO */
    {
      insert_label(_("Audio"));

      if (SoundManager::current()->is_audio_enabled())
      {
        add_toggle(MNID_SOUND, _("Sound"), &g_config->sound_enabled)
          .set_help(_("Disable all sound effects"));
        add_toggle(MNID_MUSIC, _("Music"), &g_config->music_enabled)
          .set_help(_("Disable all music"));

        add_sound_volume();
        add_music_volume();
      }
      else
      {
        add_inactive( _("Sound (disabled)"));
        add_inactive( _("Music (disabled)"));
      }

      break;
    }

    case CONTROLS: /** CONTROLS */
    {
      insert_label(_("Controls"));

      // Separated both translation strings so the latter can be removed if it is
      // no longer true, without requiring a new round of translating
      add_toggle(MNID_RUMBLING, _("Enable Rumbling Controllers"), &g_config->multiplayer_buzz_controllers)
        .set_help(_("Enable vibrating the game controllers.") + " " + _("This feature is currently only used in the multiplayer options menu."));

      add_submenu(_("Setup Keyboard"), MenuStorage::KEYBOARD_MENU)
        .set_help(_("Configure key-action mappings"));

#ifndef UBUNTU_TOUCH
      add_submenu(_("Setup Joystick"), MenuStorage::JOYSTICK_MENU)
        .set_help(_("Configure joystick control-action mappings"));
#endif

      break;
    }

    case EXTRAS: /** EXTRAS */
    {
      insert_label(_("Extras"));

      if (m_complete)
        add_submenu(_("Select Profile"), MenuStorage::PROFILE_MENU)
          .set_help(_("Select a profile to play with"));

#ifndef UBUNTU_TOUCH
      add_submenu(_("Multiplayer settings"), MenuStorage::MULTIPLAYER_MENU)
        .set_help(_("Configure settings specific to multiplayer"));
#endif

      add_toggle(MNID_TRANSITIONS, _("Enable transitions"), &g_config->transitions_enabled)
        .set_help(_("Enable screen transitions and smooth menu animation"));

      add_toggle(MNID_CUSTOM_TITLE_LEVELS, _("Custom title screen levels"), &g_config->custom_title_levels)
        .set_help(_("Allow overriding the title screen level, when loading certain worlds"));

      if (g_config->is_christmas() || g_config->christmas_mode)
        add_toggle(MNID_CHRISTMAS_MODE, _("Christmas Mode"), &g_config->christmas_mode);

      add_submenu(_("Integrations and presence"), MenuStorage::INTEGRATIONS_MENU)
      .set_help(_("Manage whether SuperTux should display the levels you play on your social media profiles (Discord)"));

      if (g_config->developer_mode)
        add_submenu(_("Menu Customization"), MenuStorage::CUSTOM_MENU_MENU)
          .set_help(_("Customize the appearance of the menus"));

      break;
    }

    case ADVANCED: /** ADVANCED */
    {
      insert_label(_("Advanced"));

#ifndef HIDE_NONMOBILE_OPTIONS
      if (g_config->developer_mode)
#endif
        add_toggle(MNID_DEVELOPER_MODE, _("Developer Mode"), &g_config->developer_mode);

      add_toggle(MNID_CONFIRMATION_DIALOG, _("Confirmation Dialog"), &g_config->confirmation_dialog).set_help(_("Confirm aborting level"));

      add_toggle(MNID_PAUSE_ON_FOCUSLOSS, _("Pause on focus loss"), &g_config->pause_on_focusloss)
        .set_help(_("Automatically pause the game when the window loses focus"));

      add_toggle(MNID_CUSTOM_CURSOR, _("Use custom mouse cursor"), &g_config->custom_mouse_cursor).set_help(_("Whether the game renders its own cursor or uses the system's cursor"));

#ifndef __EMSCRIPTEN__
      if (!g_config->disable_network)
        add_toggle(MNID_RELEASE_CHECK, _("Check for new releases"), &g_config->do_release_check)
          .set_help(_("Allows the game to perform checks for new SuperTux releases on startup and notify if any found."));

      add_toggle(MNID_DISABLE_NETWORK, _("Disable network"), &g_config->disable_network)
        .set_help(_("Prevents the game from connecting online"));
#endif

      break;
    }
  }

  add_hl();
  add_back(_("Back"));

  on_window_resize();
}

OptionsMenu::~OptionsMenu()
{
  g_config->save();
}

void
OptionsMenu::insert_label(const std::string& text)
{
  add_label(text);
  add_hl();
}


void
OptionsMenu::add_magnification()
{
  // These values go from screen:640/projection:1600 to
  // screen:1600/projection:640 (i.e. 640, 800, 1024, 1280, 1600)
  m_magnifications.list.push_back(_("auto"));
#ifndef HIDE_NONMOBILE_OPTIONS
  m_magnifications.list.push_back("40%");
  m_magnifications.list.push_back("50%");
  m_magnifications.list.push_back("62.5%");
  m_magnifications.list.push_back("80%");
#endif
  m_magnifications.list.push_back("100%");
  m_magnifications.list.push_back("125%");
  m_magnifications.list.push_back("160%");
  m_magnifications.list.push_back("200%");
  m_magnifications.list.push_back("250%");
  // Gets the actual magnification:
  if (g_config->magnification != 0.0f) //auto
  {
    std::ostringstream out;
    out << (g_config->magnification*100) << "%";
    std::string magn = out.str();
    int count = 0;
    for (const auto& magnification : m_magnifications.list)
    {
      if (magnification == magn)
      {
        m_magnifications.next = count;
        magn.clear();
        break;
      }
      ++count;
    }
    if (!magn.empty()) // Current magnification not found
    {
      m_magnifications.next = static_cast<int>(m_magnifications.list.size());
      m_magnifications.list.push_back(magn);
    }
  }

  add_string_select(MNID_MAGNIFICATION, _("Magnification"), &m_magnifications.next, m_magnifications.list)
    .set_help(_("Change the magnification of the game area"));
}

void
OptionsMenu::add_aspect_ratio()
{
  m_aspect_ratios.list.push_back(_("auto"));
  m_aspect_ratios.list.push_back("5:4");
  m_aspect_ratios.list.push_back("4:3");
  m_aspect_ratios.list.push_back("16:10");
  m_aspect_ratios.list.push_back("16:9");
  m_aspect_ratios.list.push_back("1368:768");
  // Gets the actual aspect ratio:
  if (g_config->aspect_size != Size(0, 0)) //auto
  {
    std::ostringstream out;
    out << g_config->aspect_size.width << ":" << g_config->aspect_size.height;
    std::string aspect_ratio = out.str();
    int count = 0;
    for (const auto& ratio : m_aspect_ratios.list)
    {
      if (ratio == aspect_ratio)
      {
        aspect_ratio.clear();
        m_aspect_ratios.next = count;
        break;
      }
      ++count;
    }
    if (!aspect_ratio.empty()) // Current aspect ratio not found
    {
      m_aspect_ratios.next = static_cast<int>(m_aspect_ratios.list.size());
      m_aspect_ratios.list.push_back(aspect_ratio);
    }
  }

  add_string_select(MNID_ASPECTRATIO, _("Aspect Ratio"), &m_aspect_ratios.next, m_aspect_ratios.list)
    .set_help(_("Adjust the aspect ratio"));
}

void
OptionsMenu::add_window_resolutions()
{
  m_window_resolutions.list = { "640x480", "854x480", "800x600", "1280x720", "1280x800",
                                "1440x900", "1920x1080", "1920x1200", "2560x1440" };
  m_window_resolutions.next = -1;
  Size window_size = VideoSystem::current()->get_window_size();
  std::ostringstream out;
  out << window_size.width << "x" << window_size.height;
  std::string window_size_text = out.str();
  for (size_t i = 0; i < m_window_resolutions.list.size(); ++i)
  {
    if (m_window_resolutions.list[i] == window_size_text)
    {
      m_window_resolutions.next = static_cast<int>(i);
      break;
    }
  }
  if (m_window_resolutions.next == -1) // Current window resolution not found
  {
    m_window_resolutions.list.insert(m_window_resolutions.list.begin(), window_size_text);
    m_window_resolutions.next = 0;
  }

  add_string_select(MNID_WINDOW_RESOLUTION, _("Window Resolution"), &m_window_resolutions.next, m_window_resolutions.list)
    .set_help(_("Resize the window to the given size"));
}

void
OptionsMenu::add_resolutions()
{
  int display_mode_count = SDL_GetNumDisplayModes(0);
  std::string last_display_mode;
  for (int i = 0; i < display_mode_count; ++i)
  {
    SDL_DisplayMode mode;
    int ret = SDL_GetDisplayMode(0, i, &mode);
    if (ret != 0)
    {
      log_warning << "failed to get display mode: " << SDL_GetError() << std::endl;
    }
    else
    {
      std::ostringstream out;
      out << mode.w << "x" << mode.h;
      if (mode.refresh_rate)
        out << "@" << mode.refresh_rate;
      if (last_display_mode == out.str())
        continue;
      last_display_mode = out.str();
      m_resolutions.list.insert(m_resolutions.list.begin(), out.str());
    }
  }
  m_resolutions.list.push_back("Desktop");

  std::string fullscreen_size_str = _("Desktop");
  std::ostringstream out;
  if (g_config->fullscreen_size != Size(0, 0))
  {
    out << g_config->fullscreen_size.width << "x" << g_config->fullscreen_size.height;
    if (g_config->fullscreen_refresh_rate)
       out << "@" << g_config->fullscreen_refresh_rate;
    fullscreen_size_str = out.str();
  }

  int count = 0;
  for (const auto& res : m_resolutions.list)
  {
    if (res == fullscreen_size_str)
    {
      fullscreen_size_str.clear();
      m_resolutions.next = count;
      break;
    }
    ++count;
  }
  if (!fullscreen_size_str.empty()) // Current resolution not found
  {
    m_resolutions.next = static_cast<int>(m_resolutions.list.size());
    m_resolutions.list.push_back(fullscreen_size_str);
  }

  add_string_select(MNID_FULLSCREEN_RESOLUTION, _("Fullscreen Resolution"), &m_resolutions.next, m_resolutions.list)
    .set_help(_("Determine the resolution used in fullscreen mode (you must toggle fullscreen to complete the change)"));
}

void
OptionsMenu::add_vsync()
{
  m_vsyncs.list.push_back(_("on"));
  m_vsyncs.list.push_back(_("off"));
  m_vsyncs.list.push_back(_("adaptive"));
  const int mode = VideoSystem::current()->get_vsync();
  switch (mode)
  {
    case -1:
      m_vsyncs.next = 2;
      break;
    case 0:
      m_vsyncs.next = 1;
      break;
    case 1:
      m_vsyncs.next = 0;
      break;
    default:
      log_warning << "Unknown swap mode: " << mode << std::endl;
      m_vsyncs.next = 0;
  }

  add_string_select(MNID_VSYNC, _("VSync"), &m_vsyncs.next, m_vsyncs.list)
    .set_help(_("Set the VSync mode"));
}

void
OptionsMenu::add_sound_volume()
{
  m_sound_volumes.list = { "0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" };

  std::ostringstream sound_vol_stream;
  sound_vol_stream << g_config->sound_volume << "%";
  std::string sound_vol_string = sound_vol_stream.str();

  if (std::find(m_sound_volumes.list.begin(),
                m_sound_volumes.list.end(), sound_vol_string) == m_sound_volumes.list.end())
  {
    m_sound_volumes.list.push_back(sound_vol_string);
  }

  std::sort(m_sound_volumes.list.begin(), m_sound_volumes.list.end(), less_than_volume);

  std::ostringstream out;
  out << g_config->sound_volume << "%";
  std::string sound_volume = out.str();
  int count = 0;
  for (const auto& volume : m_sound_volumes.list)
  {
    if (volume == sound_volume)
    {
      sound_volume.clear();
      m_sound_volumes.next = count;
      break;
    }
    ++count;
  }

  add_string_select(MNID_SOUND_VOLUME, _("Sound Volume"), &m_sound_volumes.next, m_sound_volumes.list)
    .set_help(_("Adjust sound volume"));
}

void
OptionsMenu::add_music_volume()
{
  m_music_volumes.list = { "0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" };

  std::ostringstream music_vol_stream;
  music_vol_stream << g_config->music_volume << "%";
  std::string music_vol_string = music_vol_stream.str();

  if (std::find(m_music_volumes.list.begin(),
               m_music_volumes.list.end(), music_vol_string) == m_music_volumes.list.end())
  {
    m_music_volumes.list.push_back(music_vol_string);
  }

  std::sort(m_music_volumes.list.begin(), m_music_volumes.list.end(), less_than_volume);

  std::ostringstream out;
  out << g_config->music_volume << "%";
  std::string music_volume = out.str();
  int count = 0;
  for (const auto& volume : m_music_volumes.list)
  {
    if (volume == music_volume)
    {
      music_volume.clear();
      m_music_volumes.next = count;
      break;
    }
    ++count;
  }

  add_string_select(MNID_MUSIC_VOLUME, _("Music Volume"), &m_music_volumes.next, m_music_volumes.list)
    .set_help(_("Adjust music volume"));
}

void
OptionsMenu::add_flash_intensity()
{
  m_flash_intensity_values.list = { "0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" };

  std::ostringstream flash_intensity_value_stream;
  flash_intensity_value_stream << g_config->flash_intensity << "%";
  std::string flash_intensity_string = flash_intensity_value_stream.str();

  if (std::find(m_flash_intensity_values.list.begin(),
    m_flash_intensity_values.list.end(), flash_intensity_string) == m_flash_intensity_values.list.end())
  {
    m_flash_intensity_values.list.push_back(flash_intensity_string);
  }

  std::sort(m_flash_intensity_values.list.begin(), m_flash_intensity_values.list.end(), less_than_volume);

  std::ostringstream out;
  out << g_config->flash_intensity << "%";
  std::string flash_intensity_value = out.str();
  int count = 0;
  for (const auto& value : m_flash_intensity_values.list)
  {
    if (value == flash_intensity_value)
    {
      flash_intensity_value.clear();
      m_flash_intensity_values.next = count;
      break;
    }
    ++count;
  }

  add_string_select(MNID_FLASH_INTENSITY, _("Flash Intensity"), &m_flash_intensity_values.next, m_flash_intensity_values.list)
    .set_help(_("Adjust the intensity of the flash produced by the thunderstorm"));
}

void
OptionsMenu::add_mobile_control_scales()
{
  for (unsigned i = 50; i <= 300; i += 25)
  {
    m_mobile_control_scales.list.push_back(std::to_string(i) + "%");
    if (i == static_cast<unsigned>(g_config->m_mobile_controls_scale * 100))
      m_mobile_control_scales.next = (i - 50) / 25;
  }

  add_string_select(MNID_MOBILE_CONTROLS_SCALE, _("On-screen controls scale"), &m_mobile_control_scales.next, m_mobile_control_scales.list);
}

void
OptionsMenu::on_window_resize()
{
  set_center_pos(static_cast<float>(SCREEN_WIDTH) / 2.0f,
                 static_cast<float>(SCREEN_HEIGHT) / 2.0f + 15.0f);
}

void
OptionsMenu::menu_action(MenuItem& item)
{
  switch (item.get_id()) {
    case MNID_ASPECTRATIO:
      {
        if (m_aspect_ratios.next == 0)
        {
          g_config->aspect_size = Size(0, 0); // Magic values
          VideoSystem::current()->apply_config();
          ScreenManager::current()->on_window_resize();
        }
        else if (sscanf(m_aspect_ratios.list[m_aspect_ratios.next].c_str(), "%d:%d",
                        &g_config->aspect_size.width, &g_config->aspect_size.height) == 2)
        {
          VideoSystem::current()->apply_config();
          ScreenManager::current()->on_window_resize();
        }
        else
        {
          log_fatal << "Invalid aspect ratio " << m_aspect_ratios.list[m_aspect_ratios.next] << " specified" << std::endl;
          assert(false);
        }
      }
      break;

    case MNID_MAGNIFICATION:
      if (m_magnifications.next == 0)
      {
        g_config->magnification = 0.0f; // Magic value
      }
      else if (sscanf(m_magnifications.list[m_magnifications.next].c_str(), "%f", &g_config->magnification) == 1)
      {
        g_config->magnification /= 100.0f;
      }
      VideoSystem::current()->apply_config();
      ScreenManager::current()->on_window_resize();
      break;

    case MNID_WINDOW_RESIZABLE:
      if (!g_config->window_resizable)
        m_resolutions.next = 0;
      break;

    case MNID_WINDOW_RESOLUTION:
      {
        int width;
        int height;
        if (sscanf(m_window_resolutions.list[m_window_resolutions.next].c_str(), "%dx%d",
                   &width, &height) != 2)
        {
          log_fatal << "can't parse " << m_window_resolutions.list[m_window_resolutions.next] << std::endl;
        }
        else
        {
          g_config->window_size = Size(width, height);
          VideoSystem::current()->apply_config();
          ScreenManager::current()->on_window_resize();
        }
      }
      break;

    case MNID_FULLSCREEN_RESOLUTION:
      {
        int width;
        int height;
        int refresh_rate;
        if (m_resolutions.list[m_resolutions.next] == "Desktop")
        {
          g_config->fullscreen_size.width = 0;
          g_config->fullscreen_size.height = 0;
          g_config->fullscreen_refresh_rate = 0;
        }
        else if (sscanf(m_resolutions.list[m_resolutions.next].c_str(), "%dx%d@%d",
                  &width, &height, &refresh_rate) == 3)
        {
          // do nothing, changes are only applied when toggling fullscreen mode
          g_config->fullscreen_size.width = width;
          g_config->fullscreen_size.height = height;
          g_config->fullscreen_refresh_rate = refresh_rate;
        }
        else if (sscanf(m_resolutions.list[m_resolutions.next].c_str(), "%dx%d",
                       &width, &height) == 2)
        {
            g_config->fullscreen_size.width = width;
            g_config->fullscreen_size.height = height;
            g_config->fullscreen_refresh_rate = 0;
        }
      }
      break;

#ifdef __EMSCRIPTEN__
    case MNID_FIT_WINDOW:
      {
        // Emscripten's Clang detects the "$" in the macro as part of C++ code
        // although it isn't even Javascript, it's Emscripten's way to pass
        // arguments from C++ to Javascript
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
        int resultds = EM_ASM_INT({
          if (window.supertux_setAutofit)
            window.supertux_setAutofit($0);

          return !!window.supertux_setAutofit;
        }, g_config->fit_window);
#pragma GCC diagnostic pop

        if (!resultds)
        {
          Dialog::show_message(_("The game couldn't detect your browser resolution.\n"
                                 "This most likely happens because it is not embedded\n"
                                 "in the SuperTux custom HTML template.\n"));
        }
      }
      break;
#endif

    case MNID_VSYNC:
    {
      int vsync = 0;
      switch (m_vsyncs.next)
      {
        case 2:
          vsync = -1;
          break;
        case 1:
          vsync = 0;
          break;
        case 0:
          vsync = 1;
          break;
        default:
          assert(false);
          break;
      }
      g_config->vsync = vsync;
      VideoSystem::current()->set_vsync(vsync);
    }
    break;

    case MNID_FULLSCREEN:
      VideoSystem::current()->apply_config();
      ScreenManager::current()->on_window_resize();
      g_config->save();
      break;

    case MNID_SOUND:
      SoundManager::current()->enable_sound(g_config->sound_enabled);
      g_config->save();
      break;

    case MNID_SOUND_VOLUME:
      if (sscanf(m_sound_volumes.list[m_sound_volumes.next].c_str(), "%i", &g_config->sound_volume) == 1)
      {
        bool sound_enabled = g_config->sound_volume > 0 ? true : false;
        SoundManager::current()->enable_sound(sound_enabled);
        SoundManager::current()->set_sound_volume(g_config->sound_volume);
        g_config->save();
      }
      break;

    case MNID_MUSIC:
      SoundManager::current()->enable_music(g_config->music_enabled);
      g_config->save();
      break;

    case MNID_MUSIC_VOLUME:
      if (sscanf(m_music_volumes.list[m_music_volumes.next].c_str(), "%i", &g_config->music_volume) == 1)
      {
        bool music_enabled = g_config->music_volume > 0 ? true : false;
        SoundManager::current()->enable_music(music_enabled);
        SoundManager::current()->set_music_volume(g_config->music_volume);
        g_config->save();
      }
      break;

    case MNID_FLASH_INTENSITY:
      if (sscanf(m_flash_intensity_values.list[m_flash_intensity_values.next].c_str(), "%i", &g_config->flash_intensity) == 1)
      {
        g_config->save();
      }
      break;

    case MNID_CUSTOM_TITLE_LEVELS:
      TitleScreen::current()->refresh_level();
      break;

    case MNID_CUSTOM_CURSOR:
      SDL_ShowCursor(g_config->custom_mouse_cursor ? 0 : 1);
      break;

    case MNID_MOBILE_CONTROLS_SCALE:
      if (sscanf(m_mobile_control_scales.list[m_mobile_control_scales.next].c_str(), "%f", &g_config->m_mobile_controls_scale) == EOF)
        g_config->m_mobile_controls_scale = 1; // if sscanf fails revert to default scale
      else
        g_config->m_mobile_controls_scale /= 100.0f;
      break;

    case MNID_DISABLE_NETWORK:
      refresh();
      set_active_item(MNID_DISABLE_NETWORK);
      break;

    default:
      break;
  }
}

/* EOF */
