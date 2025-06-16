//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "editor/object_settings.hpp"

#include <assert.h>
#include <sexp/value.hpp>

#include "util/gettext.hpp"
#include "util/log.hpp"
#include "video/color.hpp"

ObjectSettings::ObjectSettings(const std::string& name) :
  m_name(name),
  m_options()
{
}

ObjectSettings::ObjectSettings(ObjectSettings&& other) :
  m_name(other.m_name),
  m_options(std::move(other.m_options))
{
}

void
ObjectSettings::add_option(std::unique_ptr<BaseObjectOption> option)
{
  if (!option->get_key().empty())
  {
    // Make sure no option with the same key exists
    assert(std::none_of(m_options.begin(), m_options.end(),
                        [key = option->get_key()](const auto& opt)
                        {
                          return key == opt->get_key();
                        }));
  }

  m_options.push_back(std::move(option));
}

void
ObjectSettings::add_objects(const std::string& text, std::vector<std::unique_ptr<GameObject>>* value_ptr,
                            uint8_t get_objects_param, const std::function<void (std::unique_ptr<GameObject>)>& add_object_func,
                            const std::string& key, unsigned int flags)
{
  add_option(std::make_unique<ObjectSelectObjectOption>(text, value_ptr, get_objects_param, add_object_func, key, flags));
}

void
ObjectSettings::add_color(const std::string& text, Color* value_ptr,
                          const std::string& key,
                          const std::optional<Color>& default_value,
                          unsigned int flags)
{
  add_option(std::make_unique<ColorObjectOption>(text, value_ptr, key, default_value, true, flags));
}

void
ObjectSettings::add_rgba(const std::string& text, Color* value_ptr,
                         const std::string& key,
                         const std::optional<Color>& default_value,
                         unsigned int flags)
{
  add_option(std::make_unique<ColorObjectOption>(text, value_ptr, key, default_value, true, flags));
}

void
ObjectSettings::add_rgb(const std::string& text, Color* value_ptr,
                        const std::string& key,
                        const std::optional<Color>& default_value,
                        unsigned int flags)
{
  add_option(std::make_unique<ColorObjectOption>(text, value_ptr, key, default_value, false, flags));
}

void
ObjectSettings::add_bool(const std::string& text, bool* value_ptr,
                         const std::string& key,
                         const std::optional<bool>& default_value,
                         unsigned int flags)
{
  add_option(std::make_unique<BoolObjectOption>(text, value_ptr, key, default_value, flags));
}

void
ObjectSettings::add_float(const std::string& text, float* value_ptr,
                          const std::string& key,
                          const std::optional<float>& default_value,
                          unsigned int flags)
{
  add_option(std::make_unique<FloatObjectOption>(text, value_ptr, key, default_value, flags));
}

void
ObjectSettings::add_int(const std::string& text, int* value_ptr,
                        const std::string& key,
                        const std::optional<int>& default_value,
                        unsigned int flags)
{
  add_option(std::make_unique<IntObjectOption>(text, value_ptr, key, default_value, flags));
}

void
ObjectSettings::add_label(const std::string& text,
                          unsigned int flags)
{
  add_option(std::make_unique<LabelObjectOption>(text, flags));
}

void
ObjectSettings::add_direction(const std::string& text, Direction* value_ptr,
                              std::vector<Direction> possible_directions,
                              const std::string& key, unsigned int flags)
{
  add_option(std::make_unique<DirectionOption>(text, value_ptr, std::move(possible_directions),
                                               key, flags));
}

void
ObjectSettings::add_worldmap_direction(const std::string& text, worldmap::Direction* value_ptr,
                                       std::optional<worldmap::Direction> default_value,
                                       const std::string& key, unsigned int flags)
{
  add_enum(text, reinterpret_cast<int*>(value_ptr),
           {_("None"), _("West"), _("East"), _("North"), _("South")},
           {"none", "west", "east", "north", "south"},
           default_value ? static_cast<int>(*default_value) : std::optional<int>(),
           key, flags);
}

void
ObjectSettings::add_walk_mode(const std::string& text, WalkMode* value_ptr,
                              const std::optional<WalkMode>& default_value,
                              const std::string& key, unsigned int flags)
{
  add_option(std::make_unique<StringSelectObjectOption>(
               text, reinterpret_cast<int*>(value_ptr),
               std::vector<std::string>{_("One shot"), _("Ping-pong"), _("Circular")},
               std::nullopt, key, flags));
}

void
ObjectSettings::add_remove()
{
  add_option(std::make_unique<RemoveObjectOption>());
}

void
ObjectSettings::add_script(const std::string& text, std::string* value_ptr,
                           const std::string& key, unsigned int flags)
{
  add_option(std::make_unique<ScriptObjectOption>(text, value_ptr, key, flags));
}

void
ObjectSettings::add_text(const std::string& text, std::string* value_ptr,
                         const std::string& key,
                         const std::optional<std::string>& default_value,
                         unsigned int flags)
{
  add_option(std::make_unique<StringObjectOption>(text, value_ptr, key, default_value, flags));
}

void
ObjectSettings::add_translatable_text(const std::string& text, std::string* value_ptr,
                                      const std::string& key,
                                      const std::optional<std::string>& default_value,
                                      unsigned int flags)
{
  add_option(std::make_unique<StringObjectOption>(text, value_ptr, key, default_value,
                                                  flags | OPTION_TRANSLATABLE));
}

void
ObjectSettings::add_multiline_text(const std::string& text, std::string* value_ptr,
                         const std::string& key,
                         const std::optional<std::string>& default_value,
                         unsigned int flags)
{
  add_option(std::make_unique<StringMultilineObjectOption>(text, value_ptr, key, default_value, flags));
}

void
ObjectSettings::add_multiline_translatable_text(const std::string& text, std::string* value_ptr,
                                      const std::string& key,
                                      const std::optional<std::string>& default_value,
                                      unsigned int flags)
{
  add_option(std::make_unique<StringMultilineObjectOption>(text, value_ptr, key, default_value,
                                                  flags | OPTION_TRANSLATABLE));
}

void
ObjectSettings::add_string_select(const std::string& text, int* value_ptr, const std::vector<std::string>& select,
                                  const std::optional<int>& default_value,
                                  const std::string& key, unsigned int flags)
{
  add_option(std::make_unique<StringSelectObjectOption>(text, value_ptr, select, default_value, key, flags));
}

void
ObjectSettings::add_enum(const std::string& text, int* value_ptr,
                         const std::vector<std::string>& labels,
                         const std::vector<std::string>& symbols,
                         const std::optional<int>& default_value,
                         const std::string& key, unsigned int flags)
{
  add_option(std::make_unique<EnumObjectOption>(text, value_ptr, labels, symbols, default_value, key, flags));
}

void
ObjectSettings::add_file(const std::string& text, std::string* value_ptr,
                         const std::string& key,
                         const std::optional<std::string>& default_value,
                         const std::vector<std::string>& filter,
                         const std::string& basedir,
                         bool path_relative_to_basedir,
                         unsigned int flags)
{
  add_option(std::make_unique<FileObjectOption>(text, value_ptr, default_value, key, filter, basedir, path_relative_to_basedir, flags));
}

void
ObjectSettings::add_tiles(const std::string& text, TileMap* value_ptr, const std::string& key,
                          unsigned int flags)
{
  add_option(std::make_unique<TilesObjectOption>(text, value_ptr, key, flags));
}

void
ObjectSettings::add_path(const std::string& text, Path* path, const std::string& key,
                         unsigned int flags)
{
  add_option(std::make_unique<PathObjectOption>(text, path, key, flags));
}

void
ObjectSettings::add_path_ref(const std::string& text, PathObject& target, const std::string& path_ref,
                             const std::string& key, unsigned int flags)
{
  add_option(std::make_unique<PathRefObjectOption>(text, target, path_ref, key, flags));

  if (!path_ref.empty()) {
    m_options.erase(std::remove_if(m_options.begin(), m_options.end(),
                                   [](const std::unique_ptr<BaseObjectOption>& obj) {
                                     return obj->get_key() == "x" || obj->get_key() == "y";
                                   }),
                    m_options.end());
  }
}

void
ObjectSettings::add_level(const std::string& text, std::string* value_ptr, const std::string& key,
                          const std::string& basedir,
                          unsigned int flags)
{
  add_file(text, value_ptr, key, {}, {".stl"}, basedir, true, flags);
}

void
ObjectSettings::add_sprite(const std::string& text, std::string* value_ptr,
                           const std::string& key,
                           std::optional<std::string> default_value,
                           unsigned int flags)
{
  add_file(text, value_ptr, key, std::move(default_value), {".jpg", ".png", ".sprite"}, {}, true, flags);
}

void
ObjectSettings::add_surface(const std::string& text, std::string* value_ptr,
                            const std::string& key,
                            std::optional<std::string> default_value,
                            unsigned int flags)
{
  add_file(text, value_ptr, key, std::move(default_value), {".jpg", ".png", ".surface"}, {}, true, flags);
}

void
ObjectSettings::add_sound(const std::string& text, std::string* value_ptr,
                          const std::string& key,
                          std::optional<std::string> default_value,
                          unsigned int flags)
{
  add_file(text, value_ptr, key, std::move(default_value), {".wav", ".ogg"}, {}, true, flags);
}

void
ObjectSettings::add_music(const std::string& text, std::string* value_ptr,
                          const std::string& key,
                          std::optional<std::string> default_value,
                          unsigned int flags)
{
  add_file(text, value_ptr, key, std::move(default_value), {".music"}, {"/music"}, false, flags);
}

void
ObjectSettings::add_worldmap(const std::string& text, std::string* value_ptr, const std::string& key,
                             unsigned int flags)
{
  add_file(text, value_ptr, key, {}, {".stwm"}, {}, true, flags);
}

void
ObjectSettings::add_sexp(const std::string& text, const std::string& key, sexp::Value& value, unsigned int flags)
{
  add_option(std::make_unique<SExpObjectOption>(text, key, value, flags));
}

void
ObjectSettings::add_string_array(const std::string& text, const std::string& key, std::vector<std::string>& items)
{
  add_option(std::make_unique<StringArrayOption>(text, key, items));
}

void
ObjectSettings::add_test_from_here()
{
  add_option(std::make_unique<TestFromHereOption>());
}

void
ObjectSettings::add_particle_editor()
{
  add_option(std::make_unique<ParticleEditorOption>());
}


void
ObjectSettings::add_path_handle(const std::string& text,
                                PathWalker::Handle& handle,
                                const std::string& key,
                                unsigned int flags)
{
  add_option(std::make_unique<PathHandleOption>(text, handle, key, flags));
}

void
ObjectSettings::add_button(const std::string& text, const std::function<void()>& callback)
{
  add_option(std::make_unique<ButtonOption>(text, callback));
}

void
ObjectSettings::add_list(const std::string& text, const std::string& key, const std::vector<std::string>& items, std::string* value_ptr)
{
  add_option(std::make_unique<ListOption>(text, key, items, value_ptr));
}

void
ObjectSettings::reorder(const std::vector<std::string>& order)
{
  std::vector<std::unique_ptr<BaseObjectOption> > new_options;

  // put all items not in 'order' into 'new_options'
  for(auto& option : m_options) {
    if (option) {
      auto it = std::find(order.begin(), order.end(), option->get_key());
      if (it == order.end())
      {
        new_options.push_back(std::move(option));
      }
    }
  }

  // put all other items in 'order' into 'new_options' in the order of 'order'
  for(const auto& option_name : order) {
    auto it = std::find_if(m_options.begin(), m_options.end(),
                           [option_name](const std::unique_ptr<BaseObjectOption>& option){
                             return option && option->get_key() == option_name;
                           });
    if (it != m_options.end()) {
      new_options.push_back(std::move(*it));
    }
  }

  assert(m_options.size() == new_options.size());

  m_options = std::move(new_options);
}

void
ObjectSettings::remove(const std::string& key)
{
  m_options.erase(std::remove_if(m_options.begin(), m_options.end(),
                                 [key](const std::unique_ptr<BaseObjectOption>& option){
                                   return option->get_key() == key;
                                 }),
                  m_options.end());
}

void
ObjectSettings::parse(const ReaderMapping& reader)
{
  for (const auto& option : m_options)
  {
    try
    {
      option->parse(reader);
    }
    catch (const std::exception& err)
    {
      log_warning << "Error processing data for option '" << option->get_key()
                  << "': " << err.what() << std::endl;
    }
  }
}

void
ObjectSettings::save_state()
{
  for (const auto& option : m_options)
    option->save_state();
}

bool
ObjectSettings::has_state_changed() const
{
  for (const auto& option : m_options)
    if (option->has_state_changed())
      return true;

  return false;
}

void
ObjectSettings::parse_state(const ReaderMapping& reader)
{
  for (const auto& option : m_options)
  {
    try
    {
      option->parse_state(reader);
    }
    catch (const std::exception& err)
    {
      log_warning << "Error processing state data for option '" << option->get_key()
                  << "': " << err.what() << std::endl;
    }
  }
}

void
ObjectSettings::save_old_state(std::ostream& out) const
{
  for (const auto& option : m_options)
    if (option->has_state_changed())
      option->save_old_state(out);
}

void
ObjectSettings::save_new_state(Writer& writer) const
{
  for (const auto& option : m_options)
    if (option->has_state_changed())
      option->save_new_state(writer);
}

/* EOF */
