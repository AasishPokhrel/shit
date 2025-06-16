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

#include "gui/item_color_picker_2d.hpp"

#include <vector>

#include "math/util.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/surface.hpp"
#include "video/sdl_surface.hpp"


namespace {

/** Get the color of a pixel from an 8-bit RGB image
 *
 * @param surface SDL2 surface containing the image
 * @param pos Floating-point pixel coordinates in [0,1]^2
 *
 * @return Color of the pixel at the integer position which corresponds to pos
 */
Color
get_pixel(const SDLSurfacePtr& surface, const Vector& pos)
{
  assert(surface->format->BytesPerPixel == 3);
  int x = math::clamp(
    static_cast<int>(pos.x * static_cast<float>(surface->w - 1) + 0.5f),
    0,
    surface->w - 1
  );
  int y = math::clamp(
    static_cast<int>(pos.y * static_cast<float>(surface->h - 1) + 0.5f),
    0,
    surface->h - 1
  );
  uint8_t *pixel = static_cast<uint8_t *>(surface->pixels)
    + y * surface->pitch + x * 3;
  if constexpr (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    return Color::from_rgb888(pixel[2], pixel[1], pixel[0]);
  return Color::from_rgb888(pixel[0], pixel[1], pixel[2]);
}

}  // namespace


ItemColorPicker2D::ItemColorPicker2D(Color& col) :
  MenuItem(""),
  m_image(Surface::from_file("images/engine/editor/color_picker_2d.png")),
  m_image_with_pixels(SDLSurface::from_file(
    "images/engine/editor/color_picker_2d.png")),
  m_original_color(col),
  m_color(col)
{
}

void
ItemColorPicker2D::draw_marker(Canvas& canvas, Color col, float radius) const
{
  ColorOKLCh col_oklab = col;
  Vector pos_rel(
    fmodf(col_oklab.h * 0.5f / math::PI + 1.0f, 1.0f),
    1.0f - col_oklab.get_modified_lightness()
  );
  Vector pos(
    m_image_rect.get_left() + pos_rel.x * (m_image_rect.get_right()
      - m_image_rect.get_left()),
    m_image_rect.get_top() + pos_rel.y * (m_image_rect.get_bottom()
      - m_image_rect.get_top())
  );
  col.alpha = 1.0f;
  canvas.draw_hexagon(pos, radius, Color::BLACK, LAYER_GUI+1);
  canvas.draw_hexagon(pos, 0.86f * radius, Color::WHITE, LAYER_GUI+1);
  canvas.draw_hexagon(pos, 0.7f * radius, col, LAYER_GUI+2);
}

void
ItemColorPicker2D::draw(DrawingContext& context, const Vector& pos,
  int menu_width, bool active)
{
  m_image_rect = Rectf(
    pos + Vector(16, -get_height() / 2 + 8),
    pos + Vector(menu_width - 16, get_height() / 2 - 8)
  );
  context.color().draw_surface_scaled(m_image, m_image_rect, LAYER_GUI);
  draw_marker(context.color(), m_original_color, 4.7f);
  draw_marker(context.color(), m_color, 5.5f);
}

void
ItemColorPicker2D::event(const SDL_Event& ev)
{
  bool is_mouseclick = ev.type == SDL_MOUSEBUTTONDOWN
    && ev.button.button == SDL_BUTTON_LEFT;
  bool is_hold_mousemove = ev.type == SDL_MOUSEMOTION
    && (ev.motion.state & SDL_BUTTON_LMASK);
  if (is_mouseclick) {
    m_mousedown = true;
  } else if (!is_hold_mousemove || !m_mousedown) {
    m_mousedown = false;
    return;
  }

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(
    ev.motion.x, ev.motion.y);
  Vector pos(
    (mouse_pos.x - m_image_rect.get_left())
      / (m_image_rect.get_right() - m_image_rect.get_left()),
    (mouse_pos.y - m_image_rect.get_top())
      / (m_image_rect.get_bottom() - m_image_rect.get_top())
  );
  if (is_mouseclick
      && (pos.x < 0.0f || pos.x > 1.0f || pos.y < 0.0f || pos.y > 1.0f)) {
    m_mousedown = false;
    return;
  }

  // The hue is periodic -> go back to the start after the mouse leaves the
  // corner
  pos.x = fmodf(pos.x + 3.0f, 1.0f);
  // The lightness is not periodic -> clamp
  pos.y = math::clamp(pos.y, 0.0f, 1.0f);
  float alpha = m_color.alpha;
  m_color = get_pixel(m_image_with_pixels, pos);
  m_color.alpha = alpha;
}


/* EOF */
