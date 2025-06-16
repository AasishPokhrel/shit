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

#include "supertux/command_line_arguments.hpp"

#include <fmt/format.h>
#include <config.h>
#include <physfs.h>

#include "editor/overlay_widget.hpp"
#include "physfs/ifile_stream.hpp"
#include "supertux/gameconfig.hpp"
#include "util/gettext.hpp"
#include "version.h"

CommandLineArguments::CommandLineArguments() :
  m_action(NO_ACTION),
  m_log_level(LOG_WARNING),
  datadir(),
  userdir(),
  fullscreen_size(),
  fullscreen_refresh_rate(),
  window_size(),
  aspect_size(),
  use_fullscreen(),
  video(),
  show_fps(),
  show_player_pos(),
  sound_enabled(),
  music_enabled(),
  filenames(),
  enable_script_debugger(),
  tux_spawn_pos(),
  sector(),
  spawnpoint(),
  developer_mode(),
  christmas_mode(),
  repository_url(),
  editor(),
  resave()
{
}

void
CommandLineArguments::print_datadir() const
{
  // Print the datadir searchpath to stdout, one path per
  // line. Then exit. Intended for use by the supertux-editor.
  char **sp;
  sp = PHYSFS_getSearchPath();
  if (sp)
    for (size_t sp_index = 0; sp[sp_index]; sp_index++)
      std::cout << sp[sp_index] << std::endl;
  PHYSFS_freeList(sp);
}

void
CommandLineArguments::print_acknowledgements() const
{
  IFileStream in("ACKNOWLEDGEMENTS.txt");
  if (in.good())
  {
    std::string line;
    while (std::getline(in, line))
    {
      std::cout << line << std::endl;
    }
  }
  else
  {
    std::cout << "Could not open acknowledgements file" << std::endl;
  }
}

void
CommandLineArguments::print_help(const char* arg0) const
{
  std::cerr
    << fmt::format(fmt::runtime(_("Usage: {} [OPTIONS] [LEVELFILE]")), arg0) << "\n" << "\n"
    << _("General Options:") << "\n"
    << _("  -h, --help                   Show this help message and quit") << "\n"
    << _("  -v, --version                Show SuperTux version and quit") << "\n"
    << _("  --verbose                    Print verbose messages") << "\n"
    << _("  --debug                      Print extra verbose messages") << "\n"
    << _("  --print-datadir              Print SuperTux's primary data directory.") << "\n"
    << _("  --acknowledgements           Print the licenses of libraries used by SuperTux.") << "\n"
    << "\n"
    << _("Video Options:") << "\n"
    << _("  -f, --fullscreen             Run in fullscreen mode") << "\n"
    << _("  -w, --window                 Run in window mode") << "\n"
    << _("  -g, --geometry WIDTHxHEIGHT  Run SuperTux in given resolution") << "\n"
    << _("  -a, --aspect WIDTH:HEIGHT    Run SuperTux with given aspect ratio") << "\n"
    << _("  -d, --default                Reset video settings to default values") << "\n"
    << _("  --renderer RENDERER          Use sdl, opengl, or auto to render") << "\n"
    << "\n"
    << _("Audio Options:") << "\n"
    << _("  --disable-sound              Disable sound effects") << "\n"
    << _("  --disable-music              Disable music") << "\n"
    << "\n"
    << _("Game Options:") << "\n"
    << _("  --edit-level                 Open given level in editor") << "\n"
    << _("  --resave                     Loads given level and saves it") << "\n"
    << _("  --show-fps                   Display framerate in levels") << "\n"
    << _("  --no-show-fps                Do not display framerate in levels") << "\n"
    << _("  --show-pos                   Display player's current position") << "\n"
    << _("  --no-show-pos                Do not display player's position") << "\n"
    << _("  --developer                  Switch on developer feature") << "\n"
    << _("  -s, --debug-scripts          Enable script debugger.") << "\n"
    << _("  --spawn-pos X,Y              Where in the level to spawn Tux. Only used if level is specified.") << "\n"
    << _("  --sector SECTOR              Spawn Tux in SECTOR\n") << "\n"
    << _("  --spawnpoint SPAWNPOINT      Spawn Tux at SPAWNPOINT\n") << "\n"
    << "\n"
    << _("Directory Options:") << "\n"
    << _("  --datadir DIR                Set the directory for the games datafiles") << "\n"
    << _("  --userdir DIR                Set the directory for user data (savegames, etc.)") << "\n"
    << "\n"
    << _("Add-On Options:") << "\n"
    << _("  --repository-url URL         Set the URL to the Add-On repository") << "\n"
    << "\n"
    << _("Environment variables:") << "\n"
    << _("  SUPERTUX2_USER_DIR           Directory for user data (savegames, etc.)" ) << "\n"
    << _("  SUPERTUX2_DATA_DIR           Directory for the games datafiles" ) << "\n"
    << "\n"
    << std::flush;
}

void
CommandLineArguments::print_version() const
{
  std::cout << PACKAGE_NAME << " " << PACKAGE_VERSION << std::endl;
}

void
CommandLineArguments::parse_args(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if (arg == "--version" || arg == "-v")
    {
      m_action = PRINT_VERSION;

    }
    else if (arg == "--help" || arg == "-h")
    {
      m_action = PRINT_HELP;
    }
    else if (arg == "--print-datadir")
    {
      m_action = PRINT_DATADIR;
    }
    else if (arg == "--acknowledgements")
    {
      m_action = PRINT_ACKNOWLEDGEMENTS;
    }
    else if (arg == "--debug")
    {
      m_log_level = LOG_DEBUG;
    }
    else if (arg == "--verbose")
    {
      if (m_log_level < LOG_INFO)
      {
        m_log_level = LOG_INFO;
      }
    }
    else if (arg == "--datadir")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a directory for --datadir");
      }
      else
      {
        datadir = argv[++i];
      }
    }
    else if (arg == "--userdir")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a directory for --userdir");
      }
      else
      {
        userdir = argv[++i];
      }
    }
    else if (arg == "--fullscreen" || arg == "-f")
    {
      use_fullscreen = true;
    }
    else if (arg == "--default" || arg == "-d")
    {
      use_fullscreen = false;

      window_size = Size(1280, 800);
      fullscreen_size = Size(1280, 800);
      fullscreen_refresh_rate = 0;
      aspect_size = Size(0, 0);  // Auto detect.
    }
    else if (arg == "--window" || arg == "-w")
    {
      use_fullscreen = false;
    }
    else if (arg == "--geometry" || arg == "-g")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a size (WIDTHxHEIGHT) for geometry argument");
      }
      else
      {
        int width, height;
        if (sscanf(argv[i], "%9dx%9d", &width, &height) != 2)
        {
          throw std::runtime_error("Invalid geometry spec, should be WIDTHxHEIGHT");
        }
        else
        {
          window_size     = Size(width, height);
          fullscreen_size = Size(width, height);
          fullscreen_refresh_rate = 0;
        }
      }
    }
    else if (arg == "--aspect" || arg == "-a")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a ratio (WIDTH:HEIGHT) for aspect ratio");
      }
      else if (strcmp(argv[i], "auto") != 0)
      {
        int aspect_width  = 0;
        int aspect_height = 0;
        if (sscanf(argv[i], "%9d:%9d", &aspect_width, &aspect_height) != 2)
        {
          throw std::runtime_error("Invalid aspect spec, should be WIDTH:HEIGHT or auto");
        }
        else
        {
          // Use aspect ratio to calculate logical resolution.
          if (aspect_width / aspect_height > 1) {
            aspect_size = Size(600 * aspect_width / aspect_height, 600);
          } else {
            aspect_size = Size(600, 600 * aspect_height / aspect_width);
          }
        }
      }
    }
    else if (arg == "--renderer")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a renderer for renderer argument");
      }
      else
      {
        video = VideoSystem::get_video_system(argv[i]);
      }
    }
    else if (arg == "--show-fps")
    {
      show_fps = true;
    }
    else if (arg == "--no-show-fps")
    {
      show_fps = false;
    }
    else if (arg == "--show-pos")
    {
      show_player_pos = true;
    }
    else if (arg == "--no-show-pos")
    {
      show_player_pos = false;
    }
    else if (arg == "--developer")
    {
      developer_mode = true;
    }
    else if (arg == "--christmas")
    {
      christmas_mode = true;
    }
    else if (arg == "--no-christmas")
    {
      christmas_mode = false;
    }
    else if (arg == "--disable-sound" || arg == "--disable-sfx")
    {
      sound_enabled = false;
    }
    else if (arg == "--disable-music")
    {
      music_enabled = false;
    }
    else if (arg == "--spawn-pos")
    {
      Vector spawn_pos(0.0f, 0.0f);

      if (++i >= argc)
        throw std::runtime_error("Need to specify a spawn-pos X,Y");
      else
      {
        int x, y;
        if (sscanf(argv[i], "%9d,%9d", &x, &y) != 2)
          throw std::runtime_error("Invalid spawn-pos, should be X,Y");
        spawn_pos.x = static_cast<float>(x);
        spawn_pos.y = static_cast<float>(y);
      }

      tux_spawn_pos = spawn_pos;
    }
    else if (arg == "--sector") {
      if (++i >= argc) {
        throw std::runtime_error("--sector SECTOR needs an argument");
      } else {
        sector = argv[i];
      }
    }
    else if (arg == "--spawnpoint") {
      if (++i >= argc) {
        throw std::runtime_error("--spawnpoint SPAWNPOINT needs an argument");
      } else {
        spawnpoint = argv[i];
      }
    }
    else if (arg == "--debug-scripts" || arg == "-s")
    {
      enable_script_debugger = true;
    }
    else if (arg == "--repository-url")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a repository URL");
      }
      else
      {
        repository_url = argv[++i];
      }
    }
    else if (arg == "--editor" || arg == "--edit-level")
    {
      editor = true;
    }
    else if (arg == "--resave")
    {
      resave = true;
    }
    else if (arg[0] != '-')
    {
      filenames.push_back(arg);
    }
    else
    {
      throw std::runtime_error(fmt::format("Unknown option '{}'. Use --help to see a list of options", arg));
    }
  }

  // some final checks
  if (filenames.size() > 1 && !(resave && *resave)) {
    throw std::runtime_error("Only one filename allowed for the given options");
  }
}

void
CommandLineArguments::merge_into(Config& config)
{
#define merge_option(x) if (x) { config.x = *(x); }

  merge_option(fullscreen_size)
  merge_option(fullscreen_refresh_rate)
  merge_option(window_size)
  merge_option(aspect_size)
  merge_option(use_fullscreen)
  merge_option(video)
  merge_option(show_fps)
  merge_option(show_player_pos)
  merge_option(sound_enabled)
  merge_option(music_enabled)
  merge_option(enable_script_debugger)
  merge_option(tux_spawn_pos)
  merge_option(developer_mode)
  merge_option(christmas_mode)
  merge_option(repository_url)

#undef merge_option
}

/* EOF */
