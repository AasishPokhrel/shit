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

#include "editor/object_option.hpp"

#include <string>
#include <utility>

#include <vector>
#include <sstream>

#include "editor/object_menu.hpp"
#include "gui/item_stringselect.hpp"
#include "gui/menu.hpp"
#include "gui/menu_manager.hpp"
#include "gui/menu_object_select.hpp"
#include "object/tilemap.hpp"
#include "supertux/direction.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/moving_object.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "util/reader_iterator.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "video/color.hpp"

namespace {

template<typename T>
std::string fmt_to_string(const T& v)
{
  std::ostringstream out;
  out << v;
  return out.str();
}

} // namespace

bool BaseObjectOption::s_allow_saving_defaults = false;

BaseObjectOption::BaseObjectOption(const std::string& text, const std::string& key, unsigned int flags) :
  m_text(text),
  m_key(key),
  m_flags(flags),
  m_last_state()
{
}

std::string
BaseObjectOption::save() const
{
  std::ostringstream stream;
  Writer writer(stream);
  save(writer);

  return stream.str();
}

void
BaseObjectOption::save_state()
{
  s_allow_saving_defaults = true;
  m_last_state = save();
  s_allow_saving_defaults = false;
}

bool
BaseObjectOption::has_state_changed() const
{
  s_allow_saving_defaults = true;
  const bool result = m_last_state != save();
  s_allow_saving_defaults = false;

  return result;
}

void
BaseObjectOption::parse_state(const ReaderMapping& reader)
{
  parse(reader);
}

void
BaseObjectOption::save_old_state(std::ostream& out) const
{
  out << m_last_state;
}

void
BaseObjectOption::save_new_state(Writer& writer) const
{
  s_allow_saving_defaults = true;
  save(writer);
  s_allow_saving_defaults = false;
}

template<typename T>
ObjectOption<T>::ObjectOption(const std::string& text, const std::string& key, unsigned int flags, T* pointer) :
  BaseObjectOption(text, key, flags),
  m_value_pointer(pointer)
{
}

BoolObjectOption::BoolObjectOption(const std::string& text, bool* pointer, const std::string& key,
                                   std::optional<bool> default_value,
                                   unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_default_value(std::move(default_value))
{
}

void
BoolObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_toggle(-1, get_text(), m_value_pointer);
}

void
BoolObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
BoolObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (!s_allow_saving_defaults && m_default_value && *m_default_value == *m_value_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_value_pointer);
    }
  }
}

std::string
BoolObjectOption::to_string() const
{
  return *m_value_pointer ? _("true") : _("false");
}

IntObjectOption::IntObjectOption(const std::string& text, int* pointer, const std::string& key,
                                 std::optional<int> default_value,
                                 unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_default_value(std::move(default_value))
{
}

void
IntObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
IntObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (!s_allow_saving_defaults && m_default_value && *m_default_value == *m_value_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_value_pointer);
    }
  }
}

std::string
IntObjectOption::to_string() const
{
  return fmt_to_string(*m_value_pointer);
}

void
IntObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_intfield(get_text(), m_value_pointer);
}

LabelObjectOption::LabelObjectOption(const std::string& text,
                                 unsigned int flags) :
  ObjectOption(text, "", flags)
{
}

std::string
LabelObjectOption::to_string() const
{
  return "";
}

void
LabelObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_label(m_text);
}

FloatObjectOption::FloatObjectOption(const std::string& text, float* pointer, const std::string& key,
                                     std::optional<float> default_value,
                                     unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_default_value(std::move(default_value))
{
}

void
FloatObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
FloatObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (!s_allow_saving_defaults && m_default_value && *m_default_value == *m_value_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_value_pointer);
    }
  }
}

std::string
FloatObjectOption::to_string() const
{
  return fmt_to_string(*m_value_pointer);
}

void
FloatObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_floatfield(get_text(), m_value_pointer);
}

StringObjectOption::StringObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                                       std::optional<std::string> default_value,
                                       unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_default_value(std::move(default_value))
{
}

void
StringObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
StringObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (!s_allow_saving_defaults &&
        ((m_default_value && *m_default_value == *m_value_pointer) ||
         m_value_pointer->empty())) {
      // skip
    } else {
      writer.write(get_key(), *m_value_pointer, (get_flags() & OPTION_TRANSLATABLE));
    }
  }
}

std::string
StringObjectOption::to_string() const
{
  return *m_value_pointer;
}

void
StringObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_textfield(get_text(), m_value_pointer);
}

StringMultilineObjectOption::StringMultilineObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                                       std::optional<std::string> default_value,
                                       unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_default_value(std::move(default_value))
{
}

void
StringMultilineObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
StringMultilineObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (!s_allow_saving_defaults &&
        ((m_default_value && *m_default_value == *m_value_pointer) ||
         m_value_pointer->empty())) {
      // skip
    } else {
      writer.write(get_key(), *m_value_pointer, (get_flags() & OPTION_TRANSLATABLE));
    }
  }
}

std::string
StringMultilineObjectOption::to_string() const
{
  if (!m_value_pointer->empty()) {
    return "...";
  }
  return "";
}

void
StringMultilineObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_script(get_text(), m_value_pointer);
}

StringSelectObjectOption::StringSelectObjectOption(const std::string& text, int* pointer,
                                                   const std::vector<std::string>& select,
                                                   std::optional<int> default_value,
                                                   const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_select(select),
  m_default_value(std::move(default_value))
{
}

void
StringSelectObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
StringSelectObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (!s_allow_saving_defaults && m_default_value && *m_default_value == *m_value_pointer) {
      // skip
    } else {
      writer.write(get_key(), *m_value_pointer);
    }
  }
}

std::string
StringSelectObjectOption::to_string() const
{
  int* selected_id = static_cast<int*>(m_value_pointer);
  if (*selected_id >= int(m_select.size()) || *selected_id < 0) {
    return _("invalid"); //Test whether the selected ID is valid
  } else {
    return m_select[*selected_id];
  }
}

void
StringSelectObjectOption::add_to_menu(Menu& menu) const
{
  int& selected_id = *m_value_pointer;
  if ( selected_id >= static_cast<int>(m_select.size()) || selected_id < 0 ) {
    selected_id = 0; // Set the option to zero when not selectable
  }
  menu.add_string_select(-1, get_text(), m_value_pointer, m_select);
}

EnumObjectOption::EnumObjectOption(const std::string& text, int* pointer,
                                   const std::vector<std::string>& labels,
                                   const std::vector<std::string>& symbols,
                                   std::optional<int> default_value,
                                   const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_labels(labels),
  m_symbols(symbols),
  m_default_value(std::move(default_value))
{
}

void
EnumObjectOption::parse(const ReaderMapping& reader)
{
  std::string symbol;
  if (reader.get(get_key().c_str(), symbol))
  {
    int i = 0;
    while (i < static_cast<int>(m_symbols.size()) && m_symbols[i] != symbol)
      i++;

    if (0 <= i && i < static_cast<int>(m_symbols.size()))
      *m_value_pointer = i;
  }
}

void
EnumObjectOption::save(Writer& writer) const
{
  if (0 <= *m_value_pointer && *m_value_pointer < static_cast<int>(m_symbols.size()) &&
      !get_key().empty())
  {
    if (!s_allow_saving_defaults && m_default_value && *m_default_value == *m_value_pointer) {
      // skip
    } else {
      writer.write(get_key(), m_symbols[*m_value_pointer]);
    }
  }
}

std::string
EnumObjectOption::to_string() const
{
  if (0 <= *m_value_pointer && *m_value_pointer < static_cast<int>(m_labels.size())) {
    return m_labels[*m_value_pointer];
  } else {
    return _("invalid");
  }
}

void
EnumObjectOption::add_to_menu(Menu& menu) const
{
  if (*m_value_pointer >= static_cast<int>(m_labels.size()) || *m_value_pointer < 0 ) {
    *m_value_pointer = 0; // Set the option to zero when not selectable
  }
  menu.add_string_select(-1, get_text(), m_value_pointer, m_labels);
}


ScriptObjectOption::ScriptObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                                       unsigned int flags) :
  ObjectOption(text, key, flags, pointer)
{
}

void
ScriptObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
ScriptObjectOption::save(Writer& writer) const
{
  auto& value = *m_value_pointer;
  if (s_allow_saving_defaults || !value.empty())
  {
    if (!get_key().empty()) {
      writer.write(get_key(), value);
    }
  }
}

std::string
ScriptObjectOption::to_string() const
{
  if (!m_value_pointer->empty()) {
    return "...";
  }
  return "";
}

void
ScriptObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_script(get_text(), m_value_pointer);
}

FileObjectOption::FileObjectOption(const std::string& text, std::string* pointer,
                                   std::optional<std::string> default_value,
                                   const std::string& key,
                                   std::vector<std::string> filter,
                                   const std::string& basedir,
                                   bool path_relative_to_basedir,
                                   unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_default_value(std::move(default_value)),
  m_filter(std::move(filter)),
  m_basedir(basedir),
  m_path_relative_to_basedir(path_relative_to_basedir)
{
}

void
FileObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
FileObjectOption::save(Writer& writer) const
{
  if (!s_allow_saving_defaults && m_default_value && *m_default_value == *m_value_pointer) {
    // skip
  } else {
    auto& value = *m_value_pointer;
    if (!value.empty())
    {
      if (!get_key().empty()) {
        writer.write(get_key(), value);
      }
    }
  }
}

std::string
FileObjectOption::to_string() const
{
  return *m_value_pointer;
}

void
FileObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_file(get_text(), m_value_pointer, m_filter, m_basedir, m_path_relative_to_basedir);
}

ColorObjectOption::ColorObjectOption(const std::string& text, Color* pointer, const std::string& key,
                                     std::optional<Color> default_value, bool use_alpha,
                                     unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_default_value(std::move(default_value)),
  m_use_alpha(use_alpha)
{
}

void
ColorObjectOption::parse(const ReaderMapping& reader)
{
  std::vector<float> v_color;
  if (reader.get(get_key().c_str(), v_color))
    *m_value_pointer = Color(v_color, m_use_alpha);
}

void
ColorObjectOption::save(Writer& writer) const
{
  if (!get_key().empty()) {
    if (!s_allow_saving_defaults && m_default_value && *m_default_value == *m_value_pointer) {
      // skip
    } else {
      auto vec = m_value_pointer->toVector();
      if (!m_use_alpha || vec.back() == 1.0f) {
        vec.pop_back();
      }
      writer.write(get_key(), vec);
    }
  }
}

std::string
ColorObjectOption::to_string() const
{
  return m_value_pointer->to_string();
}

void
ColorObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_color(get_text(), m_value_pointer);
}

ObjectSelectObjectOption::ObjectSelectObjectOption(const std::string& text, std::vector<std::unique_ptr<GameObject>>* pointer,
                                                   uint8_t get_objects_param, const std::function<void (std::unique_ptr<GameObject>)>& add_object_func,
                                                   const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags, pointer),
  m_get_objects_param(get_objects_param),
  m_add_object_function(add_object_func)
{
}

void
ObjectSelectObjectOption::parse(const ReaderMapping& reader)
{
  std::optional<ReaderMapping> objects_mapping;
  if (reader.get(get_key().c_str(), objects_mapping))
  {
    m_value_pointer->clear();

    auto iter = objects_mapping->get_iter();
    while (iter.next())
    {
      try
      {
        auto obj = GameObjectFactory::instance().create(iter.get_key(), iter.as_mapping());
        if (m_add_object_function)
          m_add_object_function(std::move(obj));
        else
          m_value_pointer->push_back(std::move(obj));
      }
      catch (const std::exception& err)
      {
        log_warning << "Error adding object select option object '" << iter.get_key()
                    << "': " << err.what() << std::endl;
      }
    }
  }
}

void
ObjectSelectObjectOption::save(Writer& writer) const
{
  if (get_key().empty())
    return;

  writer.start_list(get_key());
  for (auto it = m_value_pointer->begin(); it != m_value_pointer->end(); it++)
  {
    auto& obj = *it;
    writer.start_list(obj->get_class_name());

    // Rectangle properties should not be saved.
    auto settings = obj->get_settings();
    settings.remove("width");
    settings.remove("height");
    settings.remove("x");
    settings.remove("y");

    for (const auto& option : settings.get_options())
      option->save(writer);

    writer.end_list(obj->get_class_name());
  }
  writer.end_list(get_key());
}

std::string
ObjectSelectObjectOption::to_string() const
{
  return fmt_to_string(m_value_pointer->size());
}

void
ObjectSelectObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(get_text(), [pointer = m_value_pointer, get_objects_param = m_get_objects_param,
                              add_object_func = m_add_object_function]() {
    MenuManager::instance().push_menu(std::make_unique<ObjectSelectMenu>(*pointer, get_objects_param, add_object_func));
  });
}

TilesObjectOption::TilesState::TilesState() :
  width(),
  height(),
  tiles()
{
}

TilesObjectOption::TilesObjectOption(const std::string& text, TileMap* tilemap, const std::string& key,
                                     unsigned int flags) :
  ObjectOption(text, key, flags, tilemap),
  m_last_tiles_state()
{
}

void
TilesObjectOption::parse(const ReaderMapping& reader)
{
  m_value_pointer->parse_tiles(reader);
}

void
TilesObjectOption::save(Writer& writer) const
{
  m_value_pointer->write_tiles(writer);
}

std::string
TilesObjectOption::to_string() const
{
  return {};
}

void
TilesObjectOption::add_to_menu(Menu& menu) const
{
}

void
TilesObjectOption::save_state()
{
  BaseObjectOption::save_state();

  m_last_tiles_state.width = m_value_pointer->get_width();
  m_last_tiles_state.height = m_value_pointer->get_height();
  m_last_tiles_state.tiles = m_value_pointer->get_tiles();
}

void
TilesObjectOption::parse_state(const ReaderMapping& reader)
{
  parse(reader);

  std::vector<uint32_t> tile_changes; // Array of pairs (index, old/new tile ID).
  if (!reader.get("tile-changes", tile_changes))
    return;

  if (tile_changes.size() % 2 != 0)
    throw std::runtime_error("'tile-changes' does not contain number pairs.");

  for (size_t i = 0; i < tile_changes.size(); i += 2)
    m_value_pointer->change(static_cast<int>(tile_changes[i]), tile_changes[i + 1]);
}

void
TilesObjectOption::save_old_state(std::ostream& out) const
{
  Writer writer(out);
  save_tile_changes(writer, false);
}

void
TilesObjectOption::save_new_state(Writer& writer) const
{
  save_tile_changes(writer, true);
}

void
TilesObjectOption::save_tile_changes(Writer& writer, bool new_tiles) const
{
  writer.write("width", new_tiles ? m_value_pointer->get_width() : m_last_tiles_state.width);
  writer.write("height", new_tiles ? m_value_pointer->get_height() : m_last_tiles_state.height);

  assert(!m_last_tiles_state.tiles.empty());
  const auto& tiles = m_value_pointer->get_tiles();

  // Tiles have been resized. Save all tiles.
  if (m_last_tiles_state.tiles.size() != tiles.size())
  {
    writer.write("tiles", new_tiles ? tiles : m_last_tiles_state.tiles);
    return;
  }

  // Get and write old/new states of changed tiles in the array.
  std::vector<uint32_t> tile_changes; // Array of pairs (index, old/new tile ID).
  for (uint32_t i = 0; i < static_cast<uint32_t>(m_last_tiles_state.tiles.size()); i++)
  {
    if (m_last_tiles_state.tiles[i] != tiles[i])
    {
      tile_changes.push_back(i);
      tile_changes.push_back(new_tiles ? tiles[i] : m_last_tiles_state.tiles[i]);
    }
  }
  writer.write("tile-changes", tile_changes);
}

PathObjectOption::PathObjectOption(const std::string& text, Path* path, const std::string& key,
                                   unsigned int flags) :
  ObjectOption(text, key, flags, path)
{
}

void
PathObjectOption::parse(const ReaderMapping& reader)
{
  std::optional<ReaderMapping> path_mapping;
  if (reader.get("path", path_mapping))
    m_value_pointer->read(*path_mapping);
}

void
PathObjectOption::save(Writer& writer) const
{
  if (!m_value_pointer->is_valid()) return;

  writer.start_list("path");
  m_value_pointer->save(writer);
  writer.end_list("path");
}

std::string
PathObjectOption::to_string() const
{
  return {};
}

void
PathObjectOption::add_to_menu(Menu& menu) const
{
}

PathRefObjectOption::PathRefObjectOption(const std::string& text, PathObject& target, const std::string& path_ref,
                                         const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags, &target),
  m_path_ref(path_ref)
{
}

void
PathRefObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), m_path_ref);
}

void
PathRefObjectOption::save(Writer& writer) const
{
  if (!m_path_ref.empty()) {
    writer.write(get_key(), m_path_ref);
  }
}

std::string
PathRefObjectOption::to_string() const
{
  return m_path_ref;
}

void
PathRefObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_path_settings(m_text, *m_value_pointer, m_path_ref);
}

SExpObjectOption::SExpObjectOption(const std::string& text, const std::string& key, sexp::Value& value,
                                   unsigned int flags) :
  ObjectOption(text, key, flags, &value)
{
}

void
SExpObjectOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
SExpObjectOption::save(Writer& writer) const
{
  if (!m_value_pointer->is_nil()) {
    writer.write(get_key(), *m_value_pointer);
  }
}

std::string
SExpObjectOption::to_string() const
{
  return m_value_pointer->str();
}

void
SExpObjectOption::add_to_menu(Menu& menu) const
{
}

PathHandleOption::PathHandleOption(const std::string& text, PathWalker::Handle& handle,
                                   const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags),
  m_target(handle)
{
}

void
PathHandleOption::parse(const ReaderMapping& reader)
{
  std::optional<ReaderMapping> handle_mapping;
  if (reader.get(get_key().c_str(), handle_mapping))
  {
    handle_mapping->get("scale_x", m_target.m_scalar_pos.x);
    handle_mapping->get("scale_y", m_target.m_scalar_pos.y);
    handle_mapping->get("offset_x", m_target.m_pixel_offset.x);
    handle_mapping->get("offset_y", m_target.m_pixel_offset.y);
  }
}

void
PathHandleOption::save(Writer& writer) const
{
  writer.start_list(get_key());
  writer.write("scale_x", m_target.m_scalar_pos.x);
  writer.write("scale_y", m_target.m_scalar_pos.y);
  writer.write("offset_x", m_target.m_pixel_offset.x);
  writer.write("offset_y", m_target.m_pixel_offset.y);
  writer.end_list(get_key());
}

std::string
PathHandleOption::to_string() const
{
  return "("
        + std::to_string(m_target.m_scalar_pos.x) + ", "
        + std::to_string(m_target.m_scalar_pos.y) + "), ("
        + std::to_string(m_target.m_pixel_offset.x) + ", "
        + std::to_string(m_target.m_pixel_offset.y) + ")";
}

void
PathHandleOption::add_to_menu(Menu& menu) const
{
  menu.add_floatfield(get_text() + " (" + _("Scale X") + ")", &m_target.m_scalar_pos.x);
  menu.add_floatfield(get_text() + " (" + _("Scale Y") + ")", &m_target.m_scalar_pos.y);
  menu.add_floatfield(get_text() + " (" + _("Offset X") + ")", &m_target.m_pixel_offset.x);
  menu.add_floatfield(get_text() + " (" + _("Offset Y") + ")", &m_target.m_pixel_offset.y);
}

RemoveObjectOption::RemoveObjectOption() :
  ObjectOption(_("Remove"), "", 0)
{
}

std::string
RemoveObjectOption::to_string() const
{
  return {};
}

void
RemoveObjectOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(ObjectMenu::MNID_REMOVE, get_text());
}

TestFromHereOption::TestFromHereOption() :
  ObjectOption(_("Test from here"), "", 0)
{
}

std::string
TestFromHereOption::to_string() const
{
  return {};
}

void
TestFromHereOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(ObjectMenu::MNID_TEST_FROM_HERE, get_text());
}

ParticleEditorOption::ParticleEditorOption() :
  ObjectOption(_("Open Particle Editor"), "", 0)
{
}

std::string
ParticleEditorOption::to_string() const
{
  return {};
}

void
ParticleEditorOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(ObjectMenu::MNID_OPEN_PARTICLE_EDITOR, get_text());
}

ButtonOption::ButtonOption(const std::string& text, std::function<void()> callback) :
  ObjectOption(text, "", 0),
  m_callback(std::move(callback))
{
}

std::string
ButtonOption::to_string() const
{
  return {};
}

void
ButtonOption::add_to_menu(Menu& menu) const
{
  menu.add_entry(get_text(), m_callback);
}

StringArrayOption::StringArrayOption(const std::string& text, const std::string& key, std::vector<std::string>& items) :
  ObjectOption(text, key, 0),
  m_items(items)
{}

void
StringArrayOption::parse(const ReaderMapping& reader)
{
  reader.get("strings", m_items);
}

void
StringArrayOption::save(Writer& writer) const
{
  writer.write("strings", m_items);
}

void
StringArrayOption::add_to_menu(Menu& menu) const
{
  menu.add_string_array(get_text(), m_items);
}

ListOption::ListOption(const std::string& text, const std::string& key, const std::vector<std::string>& items, std::string* value_ptr) :
  ObjectOption(text, key, 0, value_ptr),
  m_items(items)
{}

void
ListOption::parse(const ReaderMapping& reader)
{
  reader.get(get_key().c_str(), *m_value_pointer);
}

void
ListOption::save(Writer& writer) const
{
  writer.write(get_key(), *m_value_pointer);
}

void
ListOption::add_to_menu(Menu& menu) const
{
  menu.add_list(get_text(), m_items, m_value_pointer);
}

DirectionOption::DirectionOption(const std::string& text, Direction* value_ptr,
                                 std::vector<Direction> possible_directions,
                                 const std::string& key, unsigned int flags) :
  ObjectOption(text, key, flags, value_ptr),
  m_possible_directions(std::move(possible_directions))
{
  if (m_possible_directions.empty())
    m_possible_directions = { Direction::AUTO, Direction::NONE, Direction::LEFT,
                              Direction::RIGHT, Direction::UP, Direction::DOWN };
}

void
DirectionOption::parse(const ReaderMapping& reader)
{
  std::string dir_string;
  if (reader.get(get_key().c_str(), dir_string))
    *m_value_pointer = string_to_dir(dir_string);
}

void
DirectionOption::save(Writer& writer) const
{
  if (*m_value_pointer == m_possible_directions.at(0))
    return;

  writer.write(get_key(), dir_to_string(*m_value_pointer));
}

std::string
DirectionOption::to_string() const
{
  return dir_to_translated_string(*m_value_pointer);
}

void
DirectionOption::add_to_menu(Menu& menu) const
{
  int selected = 0;
  std::vector<std::string> labels;
  for (size_t i = 0; i < m_possible_directions.size(); i++)
  {
    const auto& dir = m_possible_directions.at(i);
    labels.push_back(dir_to_translated_string(dir));

    if (dir == *m_value_pointer)
      selected = static_cast<int>(i);
  }

  menu.add_string_select(-1, get_text(), selected, labels)
    .set_callback([value_ptr = m_value_pointer, possible_directions = m_possible_directions](int index) {
                    *value_ptr = possible_directions.at(index);
                  });
}

/* EOF */
