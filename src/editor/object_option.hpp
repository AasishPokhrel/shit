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

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_OPTION_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_OPTION_HPP

#include <optional>
#include <string>
#include <vector>

#include <sexp/value.hpp>

#include "gui/menu_action.hpp"
#include "object/path_walker.hpp"
#include "video/color.hpp"

enum ObjectOptionFlag {
  /** Set if the value is a hidden implementation detail that
      shouldn't be exposed to the user */
  OPTION_HIDDEN = (1 << 0),

  /** Set if the text should be saved as translatable */
  OPTION_TRANSLATABLE = (1 << 1)
};

namespace sexp {
class Value;
} // namespace sexp
class Color;
enum class Direction;
class GameObject;
class Menu;
class Path;
class PathObject;
class ReaderMapping;
class TileMap;
class Writer;

class BaseObjectOption
{
protected:
  /** If set, options with their default value set will be saved. */
  static bool s_allow_saving_defaults;

public:
  BaseObjectOption(const std::string& text, const std::string& key, unsigned int flags);
  virtual ~BaseObjectOption() = default;

  virtual void parse(const ReaderMapping& reader) = 0;
  virtual void save(Writer& writer) const = 0;
  virtual std::string to_string() const = 0;
  virtual void add_to_menu(Menu& menu) const = 0;

  std::string save() const;

  virtual void save_state();
  bool has_state_changed() const;
  virtual void parse_state(const ReaderMapping& reader);
  virtual void save_old_state(std::ostream& out) const;
  virtual void save_new_state(Writer& writer) const;

  inline const std::string& get_key() const { return m_key; }
  inline const std::string& get_text() const { return m_text; }
  inline unsigned int get_flags() const { return m_flags; }

protected:
  const std::string m_text;
  const std::string m_key;
  const unsigned int m_flags;

  std::string m_last_state;

private:
  BaseObjectOption(const BaseObjectOption&) = delete;
  BaseObjectOption& operator=(const BaseObjectOption&) = delete;
};

template <typename T = void>
class ObjectOption: public BaseObjectOption
{
public:
  ObjectOption(const std::string& text, const std::string& key, unsigned int flags, T* pointer = nullptr);
  virtual ~ObjectOption() override = default;

  inline T* get_value() const { return m_value_pointer; }

protected:
  T* const m_value_pointer;

private:
  ObjectOption(const ObjectOption&) = delete;
  ObjectOption& operator=(const ObjectOption&) = delete;
};

class BoolObjectOption final : public ObjectOption<bool>
{
public:
  BoolObjectOption(const std::string& text, bool* pointer, const std::string& key,
                   std::optional<bool> default_value,
                   unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  const std::optional<bool> m_default_value;

private:
  BoolObjectOption(const BoolObjectOption&) = delete;
  BoolObjectOption& operator=(const BoolObjectOption&) = delete;
};

class IntObjectOption final : public ObjectOption<int>
{
public:
  IntObjectOption(const std::string& text, int* pointer, const std::string& key,
                  std::optional<int> default_value,
                  unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  const std::optional<int> m_default_value;

private:
  IntObjectOption(const IntObjectOption&) = delete;
  IntObjectOption& operator=(const IntObjectOption&) = delete;
};

class LabelObjectOption final : public ObjectOption<>
{
public:
  LabelObjectOption(const std::string& text,
                  unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override {}
  virtual void save(Writer& writer) const override {}
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  LabelObjectOption(const LabelObjectOption&) = delete;
  LabelObjectOption& operator=(const LabelObjectOption&) = delete;
};

class FloatObjectOption final : public ObjectOption<float>
{
public:
  FloatObjectOption(const std::string& text, float* pointer, const std::string& key,
                    std::optional<float> default_value,
                    unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  const std::optional<float> m_default_value;

private:
  FloatObjectOption(const FloatObjectOption&) = delete;
  FloatObjectOption& operator=(const FloatObjectOption&) = delete;
};

class StringObjectOption final : public ObjectOption<std::string>
{
public:
  StringObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                     std::optional<std::string> default_value,
                     unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  std::optional<std::string> m_default_value;

private:
  StringObjectOption(const StringObjectOption&) = delete;
  StringObjectOption& operator=(const StringObjectOption&) = delete;
};

class StringMultilineObjectOption final : public ObjectOption<std::string>
{
public:
  StringMultilineObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                     std::optional<std::string> default_value,
                     unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  std::optional<std::string> m_default_value;

private:
  StringMultilineObjectOption(const StringMultilineObjectOption&) = delete;
  StringMultilineObjectOption& operator=(const StringMultilineObjectOption&) = delete;
};

class StringSelectObjectOption final : public ObjectOption<int>
{
public:
  StringSelectObjectOption(const std::string& text, int* pointer, const std::vector<std::string>& select,
                           std::optional<int> default_value,
                           const std::string& key, unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  const std::vector<std::string> m_select;
  const std::optional<int> m_default_value;

private:
  StringSelectObjectOption(const StringSelectObjectOption&) = delete;
  StringSelectObjectOption& operator=(const StringSelectObjectOption&) = delete;
};

class EnumObjectOption final : public ObjectOption<int>
{
public:
  EnumObjectOption(const std::string& text, int* pointer,
                   const std::vector<std::string>& labels,
                   const std::vector<std::string>& symbols,
                   std::optional<int> default_value,
                   const std::string& key, unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  const std::vector<std::string> m_labels;
  const std::vector<std::string> m_symbols;
  const std::optional<int> m_default_value;

private:
  EnumObjectOption(const EnumObjectOption&) = delete;
  EnumObjectOption& operator=(const EnumObjectOption&) = delete;
};

class ScriptObjectOption final : public ObjectOption<std::string>
{
public:
  ScriptObjectOption(const std::string& text, std::string* pointer, const std::string& key,
                     unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  ScriptObjectOption(const ScriptObjectOption&) = delete;
  ScriptObjectOption& operator=(const ScriptObjectOption&) = delete;
};

class FileObjectOption final : public ObjectOption<std::string>
{
public:
  FileObjectOption(const std::string& text, std::string* pointer,
                   std::optional<std::string> default_value,
                   const std::string& key,
                   std::vector<std::string> filter,
                   const std::string& basedir,
                   bool path_relative_to_basedir,
                   unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  std::optional<std::string> m_default_value;
  const std::vector<std::string> m_filter;
  std::string m_basedir;
  bool m_path_relative_to_basedir;

private:
  FileObjectOption(const FileObjectOption&) = delete;
  FileObjectOption& operator=(const FileObjectOption&) = delete;
};

class ColorObjectOption final : public ObjectOption<Color>
{
public:
  ColorObjectOption(const std::string& text, Color* pointer, const std::string& key,
                    std::optional<Color> default_value, bool use_alpha,
                    unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  const std::optional<Color> m_default_value;
  bool m_use_alpha;

private:
  ColorObjectOption(const ColorObjectOption&) = delete;
  ColorObjectOption& operator=(const ColorObjectOption&) = delete;
};

class ObjectSelectObjectOption final : public ObjectOption<std::vector<std::unique_ptr<GameObject>>>
{
public:
  ObjectSelectObjectOption(const std::string& text, std::vector<std::unique_ptr<GameObject>>* pointer,
                           uint8_t get_objects_param, const std::function<void (std::unique_ptr<GameObject>)>& add_object_func,
                           const std::string& key, unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  uint8_t m_get_objects_param;
  const std::function<void (std::unique_ptr<GameObject>)> m_add_object_function;

private:
  ObjectSelectObjectOption(const ObjectSelectObjectOption&) = delete;
  ObjectSelectObjectOption& operator=(const ObjectSelectObjectOption&) = delete;
};

class TilesObjectOption final : public ObjectOption<TileMap>
{
public:
  TilesObjectOption(const std::string& text, TileMap* tilemap, const std::string& key,
                    unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

  virtual void save_state() override;
  virtual void parse_state(const ReaderMapping& reader) override;
  virtual void save_old_state(std::ostream& out) const override;
  virtual void save_new_state(Writer& writer) const override;

private:
  void save_tile_changes(Writer& writer, bool new_tiles) const;

private:
  struct TilesState final
  {
    TilesState();

    int width;
    int height;
    std::vector<uint32_t> tiles;
  };
  TilesState m_last_tiles_state;

private:
  TilesObjectOption(const TilesObjectOption&) = delete;
  TilesObjectOption& operator=(const TilesObjectOption&) = delete;
};

class PathObjectOption final : public ObjectOption<Path>
{
public:
  PathObjectOption(const std::string& text, Path* path, const std::string& key,
                    unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  PathObjectOption(const PathObjectOption&) = delete;
  PathObjectOption& operator=(const PathObjectOption&) = delete;
};

class PathRefObjectOption final : public ObjectOption<PathObject>
{
public:
  PathRefObjectOption(const std::string& text, PathObject& target, const std::string& path_ref,
                      const std::string& key, unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  std::string m_path_ref;

private:
  PathRefObjectOption(const PathRefObjectOption&) = delete;
  PathRefObjectOption& operator=(const PathRefObjectOption&) = delete;
};

class SExpObjectOption final : public ObjectOption<sexp::Value>
{
public:
  SExpObjectOption(const std::string& text, const std::string& key, sexp::Value& value, unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  SExpObjectOption(const SExpObjectOption&) = delete;
  SExpObjectOption& operator=(const SExpObjectOption&) = delete;
};

class PathHandleOption final : public ObjectOption<PathWalker::Handle>
{
public:
  PathHandleOption(const std::string& text, PathWalker::Handle& handle,
                   const std::string& key, unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  PathWalker::Handle& m_target;

private:
  PathHandleOption(const PathHandleOption&) = delete;
  PathHandleOption& operator=(const PathHandleOption&) = delete;
};

class RemoveObjectOption final : public ObjectOption<>
{
public:
  RemoveObjectOption();

  virtual void parse(const ReaderMapping& reader) override {}
  virtual void save(Writer& writer) const override {}
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  RemoveObjectOption(const RemoveObjectOption&) = delete;
  RemoveObjectOption& operator=(const RemoveObjectOption&) = delete;
};

class TestFromHereOption final : public ObjectOption<>
{
public:
  TestFromHereOption();

  virtual void parse(const ReaderMapping& reader) override {}
  virtual void save(Writer& writer) const override {}
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  TestFromHereOption(const TestFromHereOption&) = delete;
  TestFromHereOption& operator=(const TestFromHereOption&) = delete;
};

class ParticleEditorOption final : public ObjectOption<>
{
public:
  ParticleEditorOption();

  virtual void parse(const ReaderMapping& reader) override {}
  virtual void save(Writer& writer) const override {}
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  ParticleEditorOption(const ParticleEditorOption&) = delete;
  ParticleEditorOption& operator=(const ParticleEditorOption&) = delete;
};

class ButtonOption final : public ObjectOption<>
{
public:
  ButtonOption(const std::string& text, std::function<void()> callback);

  virtual void parse(const ReaderMapping& reader) override {}
  virtual void save(Writer& writer) const override {}
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  std::function<void()> m_callback;

private:
  ButtonOption(const ButtonOption&) = delete;
  ButtonOption& operator=(const ButtonOption&) = delete;
};

class StringArrayOption final : public ObjectOption<>
{
public:
  StringArrayOption(const std::string& text, const std::string& key, std::vector<std::string>& items);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override { return "text-area"; }
  virtual void add_to_menu(Menu& menu) const override;

private:
  std::vector<std::string>& m_items;

private:
  StringArrayOption(const StringArrayOption&) = delete;
  StringArrayOption& operator=(const StringArrayOption&) = delete;
};

class ListOption final : public ObjectOption<std::string>
{
public:
  ListOption(const std::string& text, const std::string& key, const std::vector<std::string>& items, std::string* value_ptr);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override { return *m_value_pointer; }
  virtual void add_to_menu(Menu& menu) const override;

private:
  const std::vector<std::string>& m_items;

private:
  ListOption(const ListOption&) = delete;
  ListOption& operator=(const ListOption&) = delete;
};

class DirectionOption final : public ObjectOption<Direction>
{
public:
  DirectionOption(const std::string& text, Direction* value_ptr,
                  std::vector<Direction> possible_directions,
                  const std::string& key, unsigned int flags);

  virtual void parse(const ReaderMapping& reader) override;
  virtual void save(Writer& writer) const override;
  virtual std::string to_string() const override;
  virtual void add_to_menu(Menu& menu) const override;

private:
  std::vector<Direction> m_possible_directions;

private:
  DirectionOption(const DirectionOption&) = delete;
  DirectionOption& operator=(const DirectionOption&) = delete;
};

#endif

/* EOF */
