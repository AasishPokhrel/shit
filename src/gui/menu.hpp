//  SuperTux
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

#ifndef HEADER_SUPERTUX_GUI_MENU_HPP
#define HEADER_SUPERTUX_GUI_MENU_HPP

#include <functional>
#include <memory>
#include <SDL.h>

#include "gui/menu_action.hpp"
#include "math/vector.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

class ItemAction;
class ItemBack;
class ItemColor;
class ItemColorChannelRGBA;
class ItemColorPicker2D;
class ItemColorDisplay;
class ItemControlField;
class ItemFloatField;
class ItemGoTo;
class ItemHorizontalLine;
class ItemHorizontalMenu;
class ItemInactive;
class ItemIntField;
class ItemLabel;
class ItemPaths;
class ItemScript;
class ItemList;
class ItemStringSelect;
class ItemTextField;
class ItemToggle;
class ItemStringArray;
class ItemImages;
class MenuItem;
class PathObject;

class Menu
{
public:
  Menu();
  virtual ~Menu();

  virtual void menu_action(MenuItem& item) = 0;

  /** Executed before the menu is exited
      @return true if it should perform the back action, false if it shouldn't */
  virtual bool on_back_action() { return true; }

  /** Perform actions to bring the menu up to date with configuration changes */
  virtual void refresh() {}

  virtual void on_window_resize();

  void draw(DrawingContext& context);

  virtual void event(const SDL_Event& event);
  virtual void process_action(const MenuAction& action);

  ItemHorizontalLine& add_hl();
  ItemLabel& add_label(const std::string& text);
  ItemAction& add_entry(int id, const std::string& text);
  ItemAction& add_entry(int id, const std::string& text, const Color& text_color);
  ItemAction& add_entry(const std::string& text, const std::function<void()>& callback);
  ItemAction& add_entry(const std::string& text, const std::function<void()>& callback, const Color& text_color);
  ItemToggle& add_toggle(int id, const std::string& text, bool* toggled, bool center_text = false);
  ItemToggle& add_toggle(int id, const std::string& text,
                         const std::function<bool()>& get_func,
                         const std::function<void(bool)>& set_func,
                         bool center_text = false);
  ItemInactive& add_inactive(const std::string& text, bool default_color = false);
  ItemBack& add_back(const std::string& text, int id = -1);
  ItemGoTo& add_submenu(const std::string& text, int submenu, int id = -1);
  ItemControlField& add_controlfield(int id, const std::string& text, const std::string& mapping = "");
  ItemStringSelect& add_string_select(int id, const std::string& text, int* selected, const std::vector<std::string>& strings);
  ItemStringSelect& add_string_select(int id, const std::string& text, int default_item, const std::vector<std::string>& strings);
  ItemTextField& add_textfield(const std::string& text, std::string* input, int id = -1);
  ItemScript& add_script(const std::string& text, std::string* script, int id = -1);
  ItemIntField& add_intfield(const std::string& text, int* input, int id = -1, bool positive = false);
  ItemFloatField& add_floatfield(const std::string& text, float* input, int id = -1, bool positive = false);
  ItemAction& add_file(const std::string& text, std::string* input, const std::vector<std::string>& extensions,
                       const std::string& basedir, bool path_relative_to_basedir,
                       const std::function<void (MenuItem&)>& item_processor = {}, int id = -1);

  ItemColor& add_color(const std::string& text, Color* color, int id = -1);
  ItemColorDisplay& add_color_display(Color* color, int id = -1);
  ItemColorChannelRGBA& add_color_channel_rgba(float* input, Color channel, int id = -1,
    bool is_linear = false);
  ItemColorPicker2D& add_color_picker_2d(Color& color);
  ItemPaths& add_path_settings(const std::string& text, PathObject& target, const std::string& path_ref);
  ItemStringArray& add_string_array(const std::string& text, std::vector<std::string>& items, int id = -1);
  ItemImages& add_images(const std::string& image_path, int max_image_width = 0, int max_image_height = 0, int id = -1);
  ItemImages& add_images(const std::vector<std::string>& image_paths, int max_image_width = 0, int max_image_height = 0, int id = -1);
  ItemList& add_list(const std::string& text, const std::vector<std::string>& items, std::string* value_ptr, int id = -1);
  ItemHorizontalMenu& add_horizontalmenu(int id, float height, float min_item_width = -1.f);

  /** Remove all entries from the menu */
  void clear();

  MenuItem& get_item(int index) { return *(m_items[index]); }

  MenuItem& get_item_by_id(int id);
  const MenuItem& get_item_by_id(int id) const;

  int get_active_item_id() const;
  void set_active_item(int id);

  inline Vector get_center_pos() const { return m_pos; }
  inline void set_center_pos(float x, float y) { m_pos.x = x; m_pos.y = y; }

  float get_width() const;
  float get_height() const;

  /** returns true when the text is more important than action */
  virtual bool is_sensitive() const { return false; }

protected:
  MenuItem& add_item(std::unique_ptr<MenuItem> menu_item);
  MenuItem& add_item(std::unique_ptr<MenuItem> menu_item, int pos_);
  template<typename T, typename... Args>
  T& add_item(Args&&... args)
  {
    auto item = std::make_unique<T>(std::forward<Args>(args)...);
    auto item_ptr = item.get();
    add_item(std::move(item));
    return *item_ptr;
  }
  void delete_item(int pos_);

  /** Recalculates the width for this menu */
  void calculate_width();
  /** Recalculates the height for this menu */
  void calculate_height();

private:
  void check_controlfield_change_event(const SDL_Event& event);
  void draw_item(DrawingContext& context, int index, float y_pos);

private:
  /** position of the menu (ie. center of the menu, not top/left) */
  Vector m_pos;

  /* input implementation variables */
  int m_delete_character;
  char m_mn_input_char;
  float m_menu_width;
  float m_menu_height;
  float m_menu_help_height;

public:
  std::vector<std::unique_ptr<MenuItem> > m_items;

private:
  int m_arrange_left;

protected:
  int m_active_item;

private:
  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;
};

#endif

/* EOF */
