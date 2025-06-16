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

#ifndef HEADER_SUPERTUX_GUI_ITEM_IMAGES_HPP
#define HEADER_SUPERTUX_GUI_ITEM_IMAGES_HPP

#include "gui/menu_item.hpp"
#include "video/surface_ptr.hpp"

class ItemImages final : public MenuItem
{
public:
  ItemImages(const std::string& image_path, int max_image_width = 0, int max_image_height = 0, int id = -1);
  ItemImages(const std::vector<std::string>& image_paths, int max_image_width = 0, int max_image_height = 0, int id = -1);

  virtual void draw(DrawingContext& drawing_context, const Vector& pos, int menu_width, bool active) override;
  virtual int get_width() const override { return m_item_width; }
  virtual int get_height() const override { return m_item_height; }
  virtual void process_action(const MenuAction& action) override;

private:
  std::vector<SurfacePtr> m_images;
  bool m_gallery_mode;
  int m_selected_image;
  int m_max_image_width;
  int m_max_image_height;
  int m_item_width;
  int m_item_height;

private:
  ItemImages(const ItemImages&) = delete;
  ItemImages& operator=(const ItemImages&) = delete;
};

#endif
/* EOF */
