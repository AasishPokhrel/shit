//  SuperTux
//  Copyright (C) 2024 HybridDog
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

#ifndef HEADER_SUPERTUX_GUI_ITEM_COLOR_PICKER_2D_HPP
#define HEADER_SUPERTUX_GUI_ITEM_COLOR_PICKER_2D_HPP

#include "gui/menu_item.hpp"

#include "util/colorspace_oklab.hpp"
#include "video/color.hpp"
#include "video/sdl_surface_ptr.hpp"

/** A two-dimensional color picker
 *
 * The color picker displays an image and enables picking a color from it by
 * clicking.
 * The image contains all fully-saturated sRGB colors,
 * with the OKLab hue on the horizontal axis and modified OKLab lightness, Lr,
 * on the vertical axis.
 * Since the user can select the hue and lightness, and the chroma is fixed to
 * the highest value, the color picker is only two-dimensional.
 */
class ItemColorPicker2D final : public MenuItem
{
public:
  ItemColorPicker2D(Color& col);

  /** Show an image with all fully-saturated sRGB colors, a marker for the
   * currently selected color and a smaller marker for the initial color */
  virtual void draw(DrawingContext&, const Vector& pos, int menu_width,
    bool active) override;
  /// Determine the minimum width of the menu item
  virtual int get_width() const override { return 280; }
  virtual int get_height() const override { return get_width(); }
  /** Handle mouse input */
  virtual void event(const SDL_Event& ev) override;
  virtual bool changes_width() const override { return true; }

private:
  /** Draw a marker for a given color
   *
   * The position of the marker is calculated from col's OKLab Lr and hue
   * values, and is unaffected by chroma.
   * The inside of the marker shows col.
   * The drawing uses the layers LAYER_GUI+1 and LAYER_GUI+2.
   *
   * @param canvas Target canvas where the marker is drawn onto
   * @param col Color for which the marker is drawn
   * @param radius Marker size
   */
  void draw_marker(Canvas& canvas, Color col, float radius) const;

private:
  /// Image for drawing
  SurfacePtr m_image;
  /** The same image to sample pixels from it
   *
   * Since we cannot get pixel data from a Surface, we need this in addition to
   * m_image.
   */
  SDLSurfacePtr m_image_with_pixels;
  /// Color during the initialisation of the menu item
  Color m_original_color;
  /// Color which the user dynamically modifies
  Color& m_color;
  /// Determines if the user is still holding down the left mouse button
  bool m_mousedown = false;
  /// Coordinates where m_image is currently drawn
  Rectf m_image_rect{0, 0, 1, 1};

private:
  ItemColorPicker2D(const ItemColorPicker2D&) = delete;
  ItemColorPicker2D& operator=(const ItemColorPicker2D&) = delete;
};

#endif

/* EOF */
