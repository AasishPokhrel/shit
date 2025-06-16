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

#include "gui/menu.hpp"

#include "control/input_manager.hpp"
#include "gui/item_action.hpp"
#include "gui/item_back.hpp"
#include "gui/item_color.hpp"
#include "gui/item_colorchannel_rgba.hpp"
#include "gui/item_colordisplay.hpp"
#include "gui/item_color_picker_2d.hpp"
#include "gui/item_controlfield.hpp"
#include "gui/item_floatfield.hpp"
#include "gui/item_goto.hpp"
#include "gui/item_hl.hpp"
#include "gui/item_horizontalmenu.hpp"
#include "gui/item_inactive.hpp"
#include "gui/item_intfield.hpp"
#include "gui/item_label.hpp"
#include "gui/item_paths.hpp"
#include "gui/item_script.hpp"
#include "gui/item_script_line.hpp"
#include "gui/item_stringselect.hpp"
#include "gui/item_textfield.hpp"
#include "gui/item_list.hpp"
#include "gui/item_toggle.hpp"
#include "gui/item_string_array.hpp"
#include "gui/item_images.hpp"
#include "gui/menu_filesystem.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "gui/mousecursor.hpp"
#include "math/util.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

#include "supertux/error_handler.hpp"

Menu::Menu() :
  m_pos(Vector(static_cast<float>(SCREEN_WIDTH) / 2.0f,
               static_cast<float>(SCREEN_HEIGHT) / 2.0f)),
  m_delete_character(0),
  m_mn_input_char('\0'),
  m_menu_width(),
  m_menu_height(),
  m_menu_help_height(0.0f),
  m_items(),
  m_arrange_left(0),
  m_active_item(-1)
{
}

Menu::~Menu()
{
}

/* Add an item to a menu */
MenuItem&
Menu::add_item(std::unique_ptr<MenuItem> new_item)
{
  m_items.push_back(std::move(new_item));
  MenuItem& item = *m_items.back();

  /* If a new menu is being built, the active item shouldn't be set to
   * something that isn't selectable. Set the active_item to the first
   * selectable item added.
   */

  if (m_active_item == -1 && !item.skippable())
  {
    m_active_item = static_cast<int>(m_items.size()) - 1;
  }

  calculate_width();
  calculate_height();

  return item;
}

MenuItem&
Menu::add_item(std::unique_ptr<MenuItem> new_item, int pos_)
{
  m_items.insert(m_items.begin()+pos_,std::move(new_item));
  MenuItem& item = *m_items[pos_];

  // When the item is inserted before the selected item, the
  // same menu item should be still selected.

  if (m_active_item >= pos_)
  {
    m_active_item++;
  }

  calculate_width();
  calculate_height();

  return item;
}

void
Menu::delete_item(int pos_)
{
  m_menu_height -= static_cast<float>(m_items[pos_]->get_height()) + m_items[pos_]->get_distance() * 2;
  m_items.erase(m_items.begin()+pos_);

  // When the item is deleted before the selected item, the
  // same menu item should be still selected.

  if (m_active_item >= pos_)
  {
    do {
      if (m_active_item > 0)
        --m_active_item;
      else
        m_active_item = int(m_items.size())-1;
    } while (m_items[m_active_item]->skippable());
  }
}

ItemHorizontalLine&
Menu::add_hl()
{
  return add_item<ItemHorizontalLine>();
}

ItemLabel&
Menu::add_label(const std::string& text)
{
  return add_item<ItemLabel>(text);
}

ItemControlField&
Menu::add_controlfield(int id, const std::string& text,
                       const std::string& mapping)
{
  return add_item<ItemControlField>(text, mapping, id);
}

ItemTextField&
Menu::add_textfield(const std::string& text, std::string* input, int id)
{
  return add_item<ItemTextField>(text, input, id);
}

ItemScript&
Menu::add_script(const std::string& text, std::string* script, int id)
{
  return add_item<ItemScript>(text, script, id);
}

ItemIntField&
Menu::add_intfield(const std::string& text, int* input, int id, bool positive)
{
  return add_item<ItemIntField>(text, input, id, positive);
}

ItemFloatField&
Menu::add_floatfield(const std::string& text, float* input, int id, bool positive)
{
  return add_item<ItemFloatField>(text, input, id, positive);
}

ItemAction&
Menu::add_entry(int id, const std::string& text)
{
  return add_item<ItemAction>(text, id);
}

ItemAction&
Menu::add_entry(const std::string& text, const std::function<void()>& callback)
{
  return add_item<ItemAction>(text, -1, callback);
}

ItemAction&
Menu::add_entry(int id, const std::string& text, const Color& text_color)
{
  return add_item<ItemAction>(text, id, [](){}, text_color);
}

ItemAction&
Menu::add_entry(const std::string& text, const std::function<void()>& callback, const Color& text_color)
{
  return add_item<ItemAction>(text, -1, callback, text_color);
}

ItemInactive&
Menu::add_inactive(const std::string& text, bool default_color)
{
  return add_item<ItemInactive>(text, default_color);
}

ItemToggle&
Menu::add_toggle(int id, const std::string& text, bool* toggled, bool center_text)
{
  return add_item<ItemToggle>(text, toggled, id, center_text);
}

ItemToggle&
Menu::add_toggle(int id, const std::string& text,
                 const std::function<bool()>& get_func,
                 const std::function<void(bool)>& set_func,
                 bool center_text)
{
  return add_item<ItemToggle>(text, get_func, set_func, id, center_text);
}

ItemStringSelect&
Menu::add_string_select(int id, const std::string& text, int* selected, const std::vector<std::string>& strings)
{
  return add_item<ItemStringSelect>(text, strings, selected, id);
}

ItemStringSelect&
Menu::add_string_select(int id, const std::string& text, int default_item, const std::vector<std::string>& strings)
{
  return add_item<ItemStringSelect>(text, strings, default_item, id);
}

ItemAction&
Menu::add_file(const std::string& text, std::string* input, const std::vector<std::string>& extensions,
               const std::string& basedir, bool path_relative_to_basedir,
               const std::function<void (MenuItem&)>& item_processor, int id)
{
  return add_item<ItemAction>(text, id,
    [input, extensions, basedir, path_relative_to_basedir, item_processor]()
    {
      MenuManager::instance().push_menu(std::make_unique<FileSystemMenu>(input, extensions, basedir,
          path_relative_to_basedir, nullptr, item_processor));
    });
}

ItemBack&
Menu::add_back(const std::string& text, int id)
{
  return add_item<ItemBack>(text, id);
}

ItemGoTo&
Menu::add_submenu(const std::string& text, int submenu, int id)
{
  return add_item<ItemGoTo>(text, submenu, id);
}

ItemColorChannelRGBA&
Menu::add_color_channel_rgba(float* input, Color channel, int id, bool is_linear)
{
  return add_item<ItemColorChannelRGBA>(input, channel, id, is_linear);
}

ItemColorPicker2D&
Menu::add_color_picker_2d(Color& color) {
  return add_item<ItemColorPicker2D>(color);
}

ItemPaths&
Menu::add_path_settings(const std::string& text, PathObject& target, const std::string& path_ref)
{
  return add_item<ItemPaths>(text, target, path_ref);
}

ItemColorDisplay&
Menu::add_color_display(Color* color, int id)
{
  return add_item<ItemColorDisplay>(color, id);
}

ItemColor&
Menu::add_color(const std::string& text, Color* color, int id)
{
  return add_item<ItemColor>(text, color, id);
}

ItemStringArray&
Menu::add_string_array(const std::string& text, std::vector<std::string>& items, int id)
{
  return add_item<ItemStringArray>(text, items, id);
}

ItemImages&
Menu::add_images(const std::string& image_path, int max_image_width, int max_image_height, int id)
{
  return add_item<ItemImages>(image_path, max_image_width, max_image_height, id);
}

ItemImages&
Menu::add_images(const std::vector<std::string>& image_paths, int max_image_width, int max_image_height, int id)
{
  return add_item<ItemImages>(image_paths, max_image_width, max_image_height, id);
}

ItemList&
Menu::add_list(const std::string& text, const std::vector<std::string>& items, std::string* value_ptr, int id)
{
  return add_item<ItemList>(text, items, value_ptr, id);
}

ItemHorizontalMenu&
Menu::add_horizontalmenu(int id, float height, float min_item_width)
{
  return add_item<ItemHorizontalMenu>(id, height, min_item_width);
}

void
Menu::clear()
{
  m_items.clear();
  m_active_item = -1;
}

void
Menu::process_action(const MenuAction& action)
{
  { // Scrolling
    // Find the first and last selectable item in the current menu, so
    // that the top most selected item gives a scroll_pos of -1.0f and
    // the bottom most gives 1.0f, as otherwise the non-selectable
    // header would be cut off.
    size_t first_idx = m_items.size();
    size_t last_idx = m_items.size();
    for (size_t i = 0; i < m_items.size(); ++i) {
      if (!m_items[i]->skippable()) {
        if (first_idx == m_items.size()) {
          first_idx = i;
        }
        last_idx = i;
      }
    }

    const float screen_height = static_cast<float>(SCREEN_HEIGHT);
    const float menu_area = screen_height - m_menu_help_height;
    // get_height() doesn't include the border, so we manually add some
    const float menu_height = get_height() + 32.0f;
    const float center_y = menu_area / 2.0f;
    if (menu_height > menu_area)
    {
      const float scroll_range = (menu_height - menu_area) / 2.0f;
      const float scroll_pos = ((static_cast<float>(m_active_item - first_idx)
                                 / static_cast<float>(last_idx - first_idx)) - 0.5f) * 2.0f;

      m_pos.y = floorf(center_y - scroll_range * scroll_pos);
    }
  }

  if (m_items.size() == 0)
    return;


  const int last_active_item = m_active_item;

  switch (action) {
    case MenuAction::UP:
      do {
        if (m_active_item > 0)
          --m_active_item;
        else
          m_active_item = int(m_items.size())-1;
      } while (m_items[m_active_item]->skippable()
               && (m_active_item != last_active_item));
      break;

    case MenuAction::DOWN:
      do {
        if (m_active_item < int(m_items.size())-1 )
          ++m_active_item;
        else
          m_active_item = 0;
      } while (m_items[m_active_item]->skippable()
               && (m_active_item != last_active_item));
      break;

    case MenuAction::BACK:
      if (on_back_action()) {
        MenuManager::instance().pop_menu();
      }
      return;

    default:
      break;
  }

  if (last_active_item != m_active_item) {
    // Selection caused by Up or Down keyboard action
    if (last_active_item != -1)
      m_items[last_active_item]->process_action(MenuAction::UNSELECT);
    m_items[m_active_item]->process_action(MenuAction::SELECT);
  }

  bool last_action = m_items[m_active_item]->no_other_action();
  m_items[m_active_item]->process_action(action);
  if (last_action)
    return;

  // In case pop_menu() was called in the callback
  if (MenuManager::instance().current_menu() != this)
    return;

  if (m_items[m_active_item]->changes_width())
    calculate_width();
  if (action == MenuAction::HIT)
    menu_action(*m_items[m_active_item]);
}

void
Menu::draw_item(DrawingContext& context, int index, float y_pos)
{
  const float menu_width = get_width();

  MenuItem* pitem = m_items[index].get();

  const float x_pos = m_pos.x - menu_width / 2.0f;

  pitem->draw(context, Vector(x_pos, y_pos), static_cast<int>(menu_width), m_active_item == index);

  if (m_active_item == index && pitem->select_blink())
  {
    float blink = (sinf(g_real_time * math::PI * 1.0f)/2.0f + 0.5f) * 0.5f + 0.25f;
    context.color().draw_filled_rect(Rectf(Vector(m_pos.x - menu_width/2 + 10 - 2, y_pos - static_cast<float>(pitem->get_height())/2 - 2),
                                           Vector(m_pos.x + menu_width/2 - 10 + 2, y_pos + static_cast<float>(pitem->get_height())/2 + 2)),
                                     Color(1.0f, 1.0f, 1.0f, blink),
                                     std::max(0.f, g_config->menuroundness - 2.f),
                                     LAYER_GUI-10);
    context.color().draw_filled_rect(Rectf(Vector(m_pos.x - menu_width/2 + 10, y_pos - static_cast<float>(pitem->get_height())/2),
                                           Vector(m_pos.x + menu_width/2 - 10, y_pos + static_cast<float>(pitem->get_height())/2)),
                                     Color(1.0f, 1.0f, 1.0f, 0.5f),
                                     std::max(0.f, g_config->menuroundness - 4.f),
                                     LAYER_GUI-10);
  }
}

void
Menu::calculate_width()
{
  /* The width of the menu has to be more than the width of the text
     with the most characters */
  float max_width = 0;
  for (unsigned int i = 0; i < m_items.size(); ++i)
  {
    float w = static_cast<float>(m_items[i]->get_width());
    if (w > max_width)
      max_width = w;
  }
  m_menu_width = max_width;
}

void
Menu::calculate_height()
{
  float height = 0;
  for (unsigned i = 0; i < m_items.size(); i++)
  {
    height += static_cast<float>(m_items[i]->get_height()) + m_items[i]->get_distance() * 2;
    // If a help text is present, make some space at the bottom of the
    // menu so that the last few items don't overlap with the help
    // text.
    if (!m_items[i]->get_help().empty()) m_menu_help_height = 96.0f;
  }
  if (m_menu_help_height != 0.0f) m_pos.y = floorf((static_cast<float>(SCREEN_HEIGHT) - m_menu_help_height) / 2);
  m_menu_height = height;
}

float
Menu::get_width() const
{
  return m_menu_width + 24;
}

float
Menu::get_height() const
{
  return m_menu_height;
}

void
Menu::on_window_resize()
{
  m_pos.x = static_cast<float>(SCREEN_WIDTH) / 2.0f;
  m_pos.y = static_cast<float>(SCREEN_HEIGHT) / 2.0f;

  calculate_width();
  calculate_height();

  for (auto& item : m_items)
    item->on_window_resize();
}

void
Menu::draw(DrawingContext& context)
{
  const float menu_height = get_height();
  float y_pos = m_pos.y - menu_height / 2.0f;
  for (unsigned int i = 0; i < m_items.size(); ++i)
  {
    y_pos += m_items[i]->get_distance();
    draw_item(context, i, y_pos + static_cast<float>(m_items[i]->get_height())/2);
    y_pos += static_cast<float>(m_items[i]->get_height()) + m_items[i]->get_distance();
  }

  if (!m_items[m_active_item]->get_help().empty())
  {
    const int text_width = static_cast<int>(Resources::normal_font->get_text_width(m_items[m_active_item]->get_help()));
    const int text_height = static_cast<int>(Resources::normal_font->get_text_height(m_items[m_active_item]->get_help()));

    const Rectf text_rect(m_pos.x - static_cast<float>(text_width) / 2.0f - 8.0f,
                          static_cast<float>(SCREEN_HEIGHT) - 48.0f - static_cast<float>(text_height) / 2.0f - 4.0f,
                          m_pos.x + static_cast<float>(text_width) / 2.0f + 8.0f,
                          static_cast<float>(SCREEN_HEIGHT) - 48.0f + static_cast<float>(text_height) / 2.0f + 4.0f);

    context.color().draw_filled_rect(Rectf(text_rect.p1() - Vector(4,4),
                                           text_rect.p2() + Vector(4,4)),
                                     g_config->menuhelpbackcolor,
                                     g_config->menuroundness + 4.f,
                                     LAYER_GUI);

    context.color().draw_filled_rect(text_rect,
                                     g_config->menuhelpfrontcolor,
                                     g_config->menuroundness,
                                     LAYER_GUI);

    context.color().draw_text(Resources::normal_font, m_items[m_active_item]->get_help(),
                              Vector(m_pos.x, static_cast<float>(SCREEN_HEIGHT) - 48.0f - static_cast<float>(text_height) / 2.0f),
                              ALIGN_CENTER, LAYER_GUI);
  }
}

MenuItem&
Menu::get_item_by_id(int id)
{
  auto item = std::find_if(m_items.begin(), m_items.end(), [id](const std::unique_ptr<MenuItem>& i)
  {
    return i->get_id() == id;
  });

  if(item != m_items.end())
    return *item->get();

  throw std::runtime_error("MenuItem not found: " + std::to_string(id));
}

const MenuItem&
Menu::get_item_by_id(int id) const
{
  auto item = std::find_if(m_items.begin(), m_items.end(), [id](const std::unique_ptr<MenuItem>& i)
  {
    return i->get_id() == id;
  });

  if(item != m_items.end())
    return *item->get();

  throw std::runtime_error("MenuItem not found: " + std::to_string(id));
}

int Menu::get_active_item_id() const
{
  return m_items[m_active_item]->get_id();
}

void
Menu::event(const SDL_Event& ev)
{
  m_items[m_active_item]->event(ev);
  switch (ev.type)
  {
    case SDL_KEYDOWN:
    case SDL_TEXTINPUT:
      if (((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_BACKSPACE) ||
         ev.type == SDL_TEXTINPUT) && m_items[m_active_item]->changes_width())
      {
        // Changed item value? Let's recalculate width:
        calculate_width();
      }
    break;

    case SDL_MOUSEBUTTONDOWN:
    if (ev.button.button == SDL_BUTTON_LEFT)
    {
      Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(ev.motion.x, ev.motion.y);

      if (mouse_pos.x > m_pos.x - get_width() / 2.0f &&
          mouse_pos.x < m_pos.x + get_width() / 2.0f &&
          mouse_pos.y > m_pos.y - get_height() / 2.0f &&
          mouse_pos.y < m_pos.y + get_height() / 2.0f)
      {
        process_action(MenuAction::HIT);
      }
    }
    break;

    case SDL_MOUSEMOTION:
    {
      Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(ev.motion.x, ev.motion.y);
      float x = mouse_pos.x;
      float y = mouse_pos.y;

      if (x > m_pos.x - get_width()/2 &&
         x < m_pos.x + get_width()/2 &&
         y > m_pos.y - get_height()/2 &&
         y < m_pos.y + get_height()/2)
      {
        int new_active_item = 0;
        // This is probably not the most efficient way of finding active item
        // but I can't think of something better right now ~ mrkubax10
        float item_y = m_pos.y - get_height()/2;
        for (unsigned i = 0; i < m_items.size(); i++)
        {
          if (y >= item_y && y <= item_y + static_cast<float>(m_items[i]->get_height()))
          {
            new_active_item = i;
            break;
          }
          item_y += static_cast<float>(m_items[i]->get_height());
        }

        /* only change the mouse focus to a selectable item */
        if (!m_items[new_active_item]->skippable() &&
            new_active_item != m_active_item) {
          // Selection caused by mouse movement
          if (m_active_item != -1)
            process_action(MenuAction::UNSELECT);
          m_active_item = new_active_item;
          process_action(MenuAction::SELECT);
        }

        if (MouseCursor::current())
          MouseCursor::current()->set_state(MouseCursorState::LINK);
      }
      else
      {
        if (MouseCursor::current())
          MouseCursor::current()->set_state(MouseCursorState::NORMAL);
      }
    }
    break;

    default:
      break;
  }
}

void
Menu::set_active_item(int id)
{
  for (size_t i = 0; i < m_items.size(); ++i) {
    if (m_items[i]->get_id() == id) {
      m_active_item = static_cast<int>(i);
      break;
    }
  }
}

/* EOF */
