//  SuperTux
//  Copyright (C) 2022-2023 Vankata453
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

#include "gui/item_horizontalmenu.hpp"

#include "gui/menu_manager.hpp"
#include "math/util.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "video/viewport.hpp"

float ItemHorizontalMenu::s_max_width = 0.f; // Dynamically set

const float ItemHorizontalMenu::s_width_offset = 80.f;
const float ItemHorizontalMenu::s_menu_width_offset = 30.f;
const float ItemHorizontalMenu::s_item_spacing = 30.f;

const float ItemHorizontalMenu::s_icon_y = 3.f;

ItemHorizontalMenu::ItemHorizontalMenu(int id, float height, float min_item_width) :
  MenuItem("", id),
  m_items(),
  m_selected_item(0),
  m_item_range_begin(0),
  m_item_range_end(0),
  m_width(0.f),
  m_height(height),
  m_min_item_width(min_item_width),
  m_rect()
{
  on_window_resize();
  calculate_width();
}

ItemHorizontalMenu::Item&
ItemHorizontalMenu::get_selected_item()
{
  return m_items[m_selected_item];
}

void
ItemHorizontalMenu::add_item(const std::string& text, const std::string& description,
                             const std::string& icon_file, int id)
{
  m_items.push_back({ id, text, description, Surface::from_file(icon_file) });
  calculate_width();
}

float
ItemHorizontalMenu::get_item_width(const std::string& text) const
{
  const float item_text_width = Resources::normal_font->get_text_width(text);
  return m_min_item_width < 0 || item_text_width > m_min_item_width ? item_text_width : m_min_item_width;
}

void
ItemHorizontalMenu::calculate_width(int begin)
{
  const float leftover_spacing = std::fabs(10.0f - s_item_spacing);

  const int total = static_cast<int>(m_items.size());
  int end = total - 1;

  float width = s_menu_width_offset;
  for (int i = begin; i < total; i++)
  {
    float item_width = get_item_width(m_items[i].text) + (i == total - 1 ? s_menu_width_offset : s_item_spacing);
    width += item_width;
    if (width > s_max_width - s_menu_width_offset)
    {
      // Try removing the leftover spacing after the last item.
      width -= leftover_spacing;
      item_width -= leftover_spacing;

      if (width > s_max_width) // The width of the item is still bigger.
      {
        end = i - 1;
        width -= item_width - leftover_spacing; // Remove the leftover spacing after the last item.
      }
      else // The item can fit.
      {
        end = i;
      }
      break;
    }
  }

  m_item_range_begin = begin;
  m_item_range_end = end;
  m_width = width;
}

void
ItemHorizontalMenu::calculate_rect(const Vector& pos)
{
  m_rect = Rectf(Vector(pos.x + (MenuManager::instance().current_menu()->get_width() - m_width) / 2,
                        pos.y - m_height / 2 - 2.5f),
                 Sizef(m_width, m_height));
}

void
ItemHorizontalMenu::on_window_resize()
{
  s_max_width = static_cast<float>(SCREEN_WIDTH) - s_width_offset * 2;
  calculate_width();
}

void
ItemHorizontalMenu::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active)
{
  calculate_rect(pos);
  const float& pos_y = m_rect.p1().y;

  // Draw items.
  float pos_x = m_rect.get_left() + s_menu_width_offset;
  for (int i = m_item_range_begin; i <= m_item_range_end; i++)
  {
    const float item_width = get_item_width(m_items[i].text);
    draw_item(context, m_items[i], active ? i == m_selected_item : false,
              Vector(pos_x, pos_y), item_width);

    pos_x += item_width + s_item_spacing;
  }

  // Draw arrows.
  if (active)
  {
    if (m_item_range_begin > 0)
      context.color().draw_text(Resources::big_font, "<",
                                Vector(m_rect.get_left() + 5.0f, pos_y + m_height / 2.5f),
                                ALIGN_CENTER, LAYER_GUI);
    if (m_item_range_end < static_cast<int>(m_items.size()) - 1)
      context.color().draw_text(Resources::big_font, ">",
                                Vector(m_rect.get_right() - 5.0f, pos_y + m_height / 2.5f),
                                ALIGN_CENTER, LAYER_GUI);
  }
}

void
ItemHorizontalMenu::draw_item(DrawingContext& context, const ItemHorizontalMenu::Item& item, bool active,
                              const Vector& pos, const float& item_width)
{
  // Draw icon.
  const float icon_w = m_height / 1.5f;
  context.color().draw_surface_scaled(item.icon, Rectf(Vector(pos.x + item_width / 2 - icon_w / 2,
                                                              pos.y + s_icon_y),
                                                       Sizef(icon_w, icon_w)), LAYER_GUI);

  // Draw text.
  const float text_y = m_height - 35.f;
  context.color().draw_text(Resources::normal_font, item.text,
                            Vector(pos.x + item_width / 2, pos.y + text_y),
                            ALIGN_CENTER, LAYER_GUI);

  // Draw selection border, if active.
  if (active)
  {
    const float blink = (sinf(g_real_time * math::PI * 1.0f) / 2.0f + 0.5f) * 0.5f + 0.25f;

    const float text_height = Resources::normal_font->get_text_height(item.text);
    const Rectf item_rect(Vector(pos.x - 7.5f, pos.y + s_icon_y),
                          Sizef(item_width + 10.f, text_y + text_height));

    context.color().draw_filled_rect(item_rect,
                                     Color(1.0f, 1.0f, 1.0f, blink),
                                     std::max(0.f, g_config->menuroundness - 2.f),
                                     LAYER_GUI - 10);
    context.color().draw_filled_rect(item_rect.grown(3.0f),
                                     Color(1.0f, 1.0f, 1.0f, 0.5f),
                                     std::max(0.f, g_config->menuroundness - 4.f),
                                     LAYER_GUI - 10);

    // Set item description.
    set_help(item.description);
  }
}

void
ItemHorizontalMenu::process_action(const MenuAction& action)
{
  switch (action)
  {
    case MenuAction::LEFT:
      go_left();
      break;

    case MenuAction::RIGHT:
      go_right();
      break;

    default:
      break;
  }
}

void
ItemHorizontalMenu::event(const SDL_Event& ev)
{
  switch (ev.type)
  {
    case SDL_MOUSEMOTION:
    {
      const Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(ev.motion.x, ev.motion.y);

      if (m_rect.contains(mouse_pos))
      {
        float pos_x = m_rect.get_left() + 20.0f;
        for (int i = m_item_range_begin; i <= m_item_range_end; i++)
        {
          const float item_width = get_item_width(m_items[i].text);
          const Rectf item_rect(Vector(pos_x - 2.0f, m_rect.p1().y + s_icon_y),
                                Sizef(item_width, m_height - 30.f + Resources::normal_font->get_text_height(m_items[i].text)));

          if (item_rect.contains(mouse_pos))
          {
            m_selected_item = i;
            break;
          }

          pos_x += item_width + s_item_spacing;
        }
      }
      break;
    }
  }
}

// Navigation functions

void
ItemHorizontalMenu::go_left()
{
  // If the selected item is the first, go from the back.
  if (m_selected_item == 0)
  {
    m_selected_item = static_cast<int>(m_items.size()) - 1;
    if (m_item_range_end == m_selected_item) // The last item is already displayed.
      return;

    // Calculate the firstmost menu item, starting with which
    // the end item will be the last one.
    calculate_width(m_selected_item);
    int total_items = 0;
    while (m_item_range_end == m_selected_item)
    {
      total_items++;
      calculate_width(m_selected_item - total_items);
    }

    calculate_width(m_selected_item - total_items + 1);
    return;
  }

  m_selected_item--;
  if (m_selected_item < m_item_range_begin)
    calculate_width(m_selected_item);
}

void
ItemHorizontalMenu::go_right()
{
  // If the selected item is last, go from the beginning.
  if (m_selected_item == static_cast<int>(m_items.size()) - 1)
  {
    m_selected_item = 0;
    calculate_width(0);
    return;
  }

  m_selected_item++;
  while (m_selected_item > m_item_range_end)
    calculate_width(m_item_range_begin + 1);
}

/* EOF */
