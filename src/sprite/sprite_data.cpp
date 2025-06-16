//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023-2024 Vankata453
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

#include "sprite/sprite_data.hpp"

#include <algorithm>
#include <stdexcept>
#include <sstream>

#include <sexp/io.hpp>
#include <sexp/value.hpp>

#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/reader_object.hpp"
#include "util/string_util.hpp"
#include "video/surface.hpp"
#include "video/texture_manager.hpp"

SpriteData::Action::Action() :
  name(),
  x_offset(0),
  y_offset(0),
  hitbox_w(0),
  hitbox_h(0),
  hitbox_unisolid(false),
  fps(10),
  loops(-1),
  loop_frame(1),
  has_custom_loops(false),
  family_name(),
  surfaces()
{
}

void
SpriteData::Action::reset(SurfacePtr surface)
{
  x_offset = 0;
  y_offset = 0;
  hitbox_w = static_cast<float>(surface->get_width());
  hitbox_h = static_cast<float>(surface->get_height());
  hitbox_unisolid = false;
  fps = 10;
  loops = -1;
  loop_frame = 1;
  has_custom_loops = false;
  family_name.clear();
  surfaces = { surface };
}


SpriteData::SpriteData(const std::string& filename) :
  m_filename(filename),
  m_load_successful(false),
  actions()
{
  load();
}

void
SpriteData::load()
{
  // Reset all existing actions to a dummy texture
  if (!actions.empty())
  {
    auto surface = Surface::from_texture(TextureManager::current()->create_dummy_texture());
    for (const auto& action : actions)
      action.second->reset(surface);
  }

  if (StringUtil::has_suffix(m_filename, ".sprite"))
  {
    try
    {
      auto doc = ReaderDocument::from_file(m_filename);
      auto root = doc.get_root();

      if (root.get_name() != "supertux-sprite")
      {
        std::ostringstream msg;
        msg << "'" << m_filename << "' is not a 'supertux-sprite' file!";
        throw std::runtime_error(msg.str());
      }
      else
      {
        // Load ".sprite" file
        parse(root.get_mapping());
      }
    }
    catch (const std::exception& err)
    {
      log_warning << "Parse error when trying to load sprite '" << m_filename
                  << "': " << err.what() << std::endl;

      // Load initial dummy texture
      if (actions.empty())
      {
        auto surface = Surface::from_texture(TextureManager::current()->create_dummy_texture());
        auto action = std::make_unique<Action>();
        action->name = "default";
        action->reset(surface);
        actions[action->name] = std::move(action);
      }

      m_load_successful = false;
      return;
    }
  }
  else
  {
    // Load single image
    auto surface = Surface::from_file(m_filename);
    if (!TextureManager::current()->last_load_successful())
      throw std::runtime_error("Cannot load image.");

    // Create action, if it doesn't exist
    {
      auto i = actions.find("default");
      if (i == actions.end())
      {
        auto action = std::make_unique<Action>();
        action->name = "default";
        actions["default"] = std::move(action);
      }
    }
    actions["default"]->reset(surface);
  }

  m_load_successful = true;
}

void
SpriteData::parse(const ReaderMapping& mapping)
{
  auto iter = mapping.get_iter();
  while (iter.next())
  {
    if (iter.get_key() == "action")
      parse_action(iter.as_mapping());
    else
      log_warning << "Unknown sprite field: " << iter.get_key() << std::endl;
  }

  if (actions.empty())
    throw std::runtime_error("Error: Sprite without actions.");
}

void
SpriteData::parse_action(const ReaderMapping& mapping)
{
  std::string name;
  mapping.get("name", name);

  // Create action, if it doesn't exist
  {
    auto i = actions.find(name);
    if (i == actions.end())
    {
      auto action = std::make_unique<Action>();
      action->name = name;
      actions[name] = std::move(action);
    }
  }
  Action* action = actions[name].get();

  // Reset action
  action->hitbox_w = 0;
  action->hitbox_h = 0;
  action->surfaces.clear();

  std::vector<float> hitbox;
  if (mapping.get("hitbox", hitbox))
  {
    switch (hitbox.size())
    {
      case 4:
        action->hitbox_h = hitbox[3];
        action->hitbox_w = hitbox[2];
        [[fallthrough]];
      case 2:
        action->y_offset = hitbox[1];
        action->x_offset = hitbox[0];
        break;

      default:
        throw std::runtime_error("Hitbox should specify 2/4 coordinates!");
    }
  }
  mapping.get("unisolid", action->hitbox_unisolid);
  mapping.get("fps", action->fps);
  if (mapping.get("loops", action->loops))
  {
    action->has_custom_loops = true;
  }
  if (mapping.get("loop-frame", action->loop_frame))
  {
    if (action->loop_frame < 1)
    {
      log_warning << "'loop-frame' of action '" << action->name << "' in sprite '" << m_filename << "' set to a value below 1." << std::endl;
      action->loop_frame = 1;
    }
  }

  if (!mapping.get("family_name", action->family_name))
  {
    action->family_name = "::" + action->name;
  }

  std::string mirror_action;
  std::string flip_action;
  std::string clone_action;
  if (mapping.get("mirror-action", mirror_action))
  {
    const auto act_tmp = get_action(mirror_action);
    if (act_tmp == nullptr)
    {
      std::ostringstream msg;
      msg << "Could not mirror action. Action not found: \"" << mirror_action << "\"\n"
          << "Mirror actions must be defined after the real one!";
      throw std::runtime_error(msg.str());
    }
    else
    {
      float max_w = 0;
      float max_h = 0;
      for (const auto& surf : act_tmp->surfaces)
      {
        auto surface = surf->clone(HORIZONTAL_FLIP);
        max_w = std::max(max_w, static_cast<float>(surface->get_width()));
        max_h = std::max(max_h, static_cast<float>(surface->get_height()));
        action->surfaces.push_back(surface);
      }

      if (action->hitbox_w < 1 && action->hitbox_h < 1)
      {
        action->hitbox_w = act_tmp->hitbox_w;
        action->hitbox_h = act_tmp->hitbox_h;
        action->x_offset = act_tmp->x_offset;
        action->y_offset = act_tmp->y_offset;
      }

      if (!action->has_custom_loops && act_tmp->has_custom_loops)
      {
        action->has_custom_loops = act_tmp->has_custom_loops;
        action->loops = act_tmp->loops;
      }

      if (action->fps == 0)
      {
        action->fps = act_tmp->fps;
      }

      if (action->family_name == "::" + action->name)
      {
        action->family_name = act_tmp->family_name;
      }
    }
  }
  else if (mapping.get("flip-action", flip_action))
  {
    const auto act_tmp = get_action(flip_action);
    if (act_tmp == nullptr)
    {
      std::ostringstream msg;
      msg << "Could not flip action. Action not found: \"" << flip_action << "\"\n"
          << "Flip actions must be defined after the real one!";
      throw std::runtime_error(msg.str());
    }
    else
    {
      float max_w = 0;
      float max_h = 0;
      for (const auto& surf : act_tmp->surfaces)
      {
        auto surface = surf->clone(VERTICAL_FLIP);
        max_w = std::max(max_w, static_cast<float>(surface->get_width()));
        max_h = std::max(max_h, static_cast<float>(surface->get_height()));
        action->surfaces.push_back(surface);
      }

      if (action->hitbox_w < 1 && action->hitbox_h < 1)
      {
        action->hitbox_w = act_tmp->hitbox_w;
        action->hitbox_h = act_tmp->hitbox_h;
        action->x_offset = act_tmp->x_offset;
        action->y_offset = act_tmp->y_offset;
      }

      if (!action->has_custom_loops && act_tmp->has_custom_loops)
      {
        action->has_custom_loops = act_tmp->has_custom_loops;
        action->loops = act_tmp->loops;
      }

      if (action->fps == 0)
      {
        action->fps = act_tmp->fps;
      }

      if (action->family_name == "::" + action->name) {
        action->family_name = act_tmp->family_name;
      }
    }
  }
  else if (mapping.get("clone-action", clone_action))
  {
    const auto* act_tmp = get_action(clone_action);
    if (act_tmp == nullptr)
    {
      std::ostringstream msg;
      msg << "Could not clone action. Action not found: \"" << clone_action << "\"\n"
          << "Clone actions must be defined after the real one!";
      throw std::runtime_error(msg.str());
    }
    else
    {
      // copy everything except the name (Semphris: and the family name)
      const std::string oldname = action->name;
      const std::string oldfam = action->family_name;
      *action = *act_tmp;
      action->name = oldname;
      action->family_name = oldfam;

      if (action->family_name == "::" + action->name) {
        action->family_name = act_tmp->family_name;
      }
    }
  }
  else
  { // Load images
    std::optional<ReaderCollection> surfaces_collection;
    std::vector<std::string> images;
    std::optional<ReaderMapping> regions_mapping;
    if (mapping.get("regions", regions_mapping)) // Regions from images
    {
      float max_w = 0;
      float max_h = 0;

      auto iter = regions_mapping->get_iter();
      while (iter.next())
      {
        if (iter.get_key() != "region")
        {
          log_warning << "Unknown field '" << iter.get_key() << "' under 'regions'." << std::endl;
          continue;
        }

        const auto& sx = iter.as_mapping().get_sexp();
        const auto& arr = sx.as_array();
        if (arr.size() != 6)
        {
          log_warning << "(region IMAGE_FILE X Y WIDTH HEIGHT) tag malformed: " << sx << std::endl;
          continue;
        }

        Rect region;
        region.left = arr[2].as_int();
        region.top = arr[3].as_int();
        const int w = arr[4].as_int();
        const int h = arr[5].as_int();
        region.right = region.left + w;
        region.bottom = region.top + h;

        max_w = std::max(max_w, static_cast<float>(w));
        max_h = std::max(max_w, static_cast<float>(h));

        auto surface = Surface::from_file(FileSystem::join(mapping.get_doc().get_directory(),
                                                           arr[1].as_string()),
                                          region);
        action->surfaces.push_back(surface);
      }

      if (action->hitbox_w < 1) action->hitbox_w = max_w - action->x_offset;
      if (action->hitbox_h < 1) action->hitbox_h = max_h - action->y_offset;
    }
    else if (mapping.get("images", images))
    {
      float max_w = 0;
      float max_h = 0;
      for (const auto& image : images)
      {
        auto surface = Surface::from_file(FileSystem::join(mapping.get_doc().get_directory(), image));
        max_w = std::max(max_w, static_cast<float>(surface->get_width()));
        max_h = std::max(max_h, static_cast<float>(surface->get_height()));
        action->surfaces.push_back(surface);
      }
      if (action->hitbox_w < 1) action->hitbox_w = max_w - action->x_offset;
      if (action->hitbox_h < 1) action->hitbox_h = max_h - action->y_offset;
    }
    else if (mapping.get("surfaces", surfaces_collection))
    {
      for (const auto& i : surfaces_collection->get_objects())
      {
        if (i.get_name() == "surface")
        {
          action->surfaces.push_back(Surface::from_reader(i.get_mapping()));
        }
        else
        {
          std::stringstream msg;
          msg << "Sprite '" << m_filename << "' unknown tag in 'surfaces' << " << i.get_name();
          throw std::runtime_error(msg.str());
        }
      }

      // calculate hitbox
      float max_w = 0;
      float max_h = 0;
      for (const auto& surface : action->surfaces)
      {
        max_w = std::max(max_w, static_cast<float>(surface->get_width()));
        max_h = std::max(max_h, static_cast<float>(surface->get_height()));
      }
      if (action->hitbox_w < 1) action->hitbox_w = max_w - action->x_offset;
      if (action->hitbox_h < 1) action->hitbox_h = max_h - action->y_offset;
    }
    else
    {
      std::stringstream msg;
      msg << "Sprite '" << m_filename << "' contains no images in action '"
          << action->name << "'.";
      throw std::runtime_error(msg.str());
    }
  }

  // Reset loop-frame, if it's specified in current action, but not-in-range of total frames.
  const int frames = static_cast<int>(action->surfaces.size());
  if (action->loop_frame > frames && frames > 0)
  {
    log_warning << "'loop-frame' of action '" << action->name << "' in sprite '" << m_filename << "' not-in-range of total frames." << std::endl;
    action->loop_frame = 1;
  }
}

const SpriteData::Action*
SpriteData::get_action(const std::string& act) const
{
  Actions::const_iterator i = actions.find(act);
  if (i == actions.end()) {
    return nullptr;
  }
  return i->second.get();
}

/* EOF */
