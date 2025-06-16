//  SuperTux
//  Copyright (C) 2022 mrkubax10 <mrkubax10@onet.pl>
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

#include "gui/item_images.hpp"

#include "supertux/resources.hpp"
#include "video/surface.hpp"
#include "video/drawing_context.hpp"

ItemImages::ItemImages(const std::string& image_path, int max_image_width, int max_image_height, int id):
  MenuItem("", id),
  m_images(),
  m_gallery_mode(false),
  m_selected_image(0),
  m_max_image_width(max_image_width),
  m_max_image_height(max_image_height),
  m_item_width(0),
  m_item_height(0)
{
  m_images.push_back(Surface::from_file(image_path));
  m_item_width = (m_images[0]->get_width() > max_image_width && max_image_width > 0 ? max_image_width : m_images[0]->get_width()) + 4;
  m_item_height = (m_images[0]->get_height() > max_image_height && max_image_height > 0 ? max_image_height : m_images[0]->get_height()) + 4;
}

ItemImages::ItemImages(const std::vector<std::string>& image_paths, int max_image_width, int max_image_height, int id):
  MenuItem("", id),
  m_images(),
  m_gallery_mode(image_paths.size() > 1),
  m_selected_image(0),
  m_max_image_width(max_image_width),
  m_max_image_height(max_image_height),
  m_item_width(0),
  m_item_height(0)
{
  int max_width = 0;
  int max_height = 0;
  for (unsigned i = 0; i < image_paths.size(); i++)
  {
    m_images.push_back(Surface::from_file(image_paths[i]));
    if(m_images[i]->get_width() > max_width)
      max_width = (m_images[i]->get_width() > max_image_width && max_image_width > 0 ? max_image_width : m_images[i]->get_width());
    if(m_images[i]->get_height() > max_height)
      max_height = (m_images[i]->get_height() > max_image_height && max_image_height > 0 ? max_image_height : m_images[i]->get_height());
  }
  m_item_width = max_width + 4;
  m_item_height = max_height + 4;
}

void
ItemImages::draw(DrawingContext& drawing_context, const Vector& pos, int menu_width, bool active)
{
  if (m_images.empty())
    return;
  SurfacePtr surface = m_images[m_selected_image];
  if (m_max_image_width > 0 && m_max_image_height > 0 && (surface->get_width() > m_max_image_width || surface->get_height() > m_max_image_height))
    drawing_context.color().draw_surface_scaled(surface, Rectf(pos + Vector((menu_width - m_max_image_width)/2 - 2, -m_max_image_height/2),
                                                               Sizef(static_cast<float>(m_max_image_width), static_cast<float>(m_max_image_height))), LAYER_GUI);
  else
    drawing_context.color().draw_surface(surface, pos + Vector((menu_width - surface->get_width())/2 - 2, -surface->get_height()/2), LAYER_GUI);
  if (m_gallery_mode)
  {
    float left_arrow_width = Resources::big_font->get_text_width("<");
    float right_arrow_width  = Resources::big_font->get_text_width(">");
    float arrow_height = Resources::big_font->get_text_height("<");
    drawing_context.color().draw_text(Resources::big_font, "<", pos + Vector(left_arrow_width/2 + 2, -arrow_height/2), FontAlignment::ALIGN_LEFT, LAYER_GUI);
    drawing_context.color().draw_text(Resources::big_font, ">", pos + Vector(static_cast<float>(menu_width) - right_arrow_width*1.5f, -arrow_height/2), FontAlignment::ALIGN_LEFT, LAYER_GUI);
  }
}

void
ItemImages::process_action(const MenuAction& action)
{
  if (!m_gallery_mode)
    return;
  switch (action)
  {
  case MenuAction::LEFT:
    m_selected_image--;
    m_selected_image = (m_selected_image < 0 ? static_cast<int>(m_images.size()) - 1 : m_selected_image);
    break;
  case MenuAction::RIGHT:
    m_selected_image++;
    m_selected_image = (m_selected_image >= static_cast<int>(m_images.size()) ? 0 : m_selected_image);
    break;
  default:
    break;
  }
}
/* EOF */
