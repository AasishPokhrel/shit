//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/gl/gl_painter.hpp"

#include <algorithm>
#include <math.h>

#include "math/util.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_request.hpp"
#include "video/gl/gl_context.hpp"
#include "video/gl/gl_pixel_request.hpp"
#include "video/gl/gl_program.hpp"
#include "video/gl/gl_renderer.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/gl/gl_vertex_arrays.hpp"
#include "video/gl/gl_video_system.hpp"
#include "video/glutil.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

inline std::tuple<GLenum, GLenum> blend_factor(Blend blend)
{
  using B = std::tuple<GLenum, GLenum>;
  switch(blend) {
    case Blend::NONE: return B(GL_ONE, GL_ZERO);
    case Blend::BLEND: return B(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    case Blend::ADD: return B(GL_SRC_ALPHA, GL_ONE);
    case Blend::MOD: return B(GL_DST_COLOR, GL_ZERO);
    default: return B(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

inline GLenum sfactor(Blend blend)
{
  return std::get<0>(blend_factor(blend));
}

inline GLenum dfactor(Blend blend)
{
  return std::get<1>(blend_factor(blend));
}

} // namespace

GLPainter::GLPainter(GLVideoSystem& video_system, GLRenderer& renderer) :
  m_video_system(video_system),
  m_renderer(renderer),
  m_vertices(),
  m_uvs()
{
}

void
GLPainter::draw_texture(const TextureRequest& request)
{
  assert_gl();

  const auto& texture = static_cast<const GLTexture&>(*request.texture);

  assert(request.srcrects.size() == request.dstrects.size());
  assert(request.srcrects.size() == request.angles.size());

  m_vertices.clear();
  m_uvs.clear();

  m_vertices.reserve(request.srcrects.size() * 12);
  m_uvs.reserve(request.srcrects.size() * 12);

  for (size_t i = 0; i < request.srcrects.size(); ++i)
  {
    const float& left = request.dstrects[i].get_left();
    const float& top = request.dstrects[i].get_top();
    const float& right  = request.dstrects[i].get_right();
    const float& bottom = request.dstrects[i].get_bottom();

    float uv_left = request.srcrects[i].get_left() / static_cast<float>(texture.get_texture_width());
    float uv_top = request.srcrects[i].get_top() / static_cast<float>(texture.get_texture_height());
    float uv_right = request.srcrects[i].get_right() / static_cast<float>(texture.get_texture_width());
    float uv_bottom = request.srcrects[i].get_bottom() / static_cast<float>(texture.get_texture_height());

    if (request.flip & HORIZONTAL_FLIP)
      std::swap(uv_left, uv_right);

    if (request.flip & VERTICAL_FLIP)
      std::swap(uv_top, uv_bottom);

    if (request.angles[i] == 0.0f)
    {
      auto vertices_lst = {
        left, top,
        right, top,
        right, bottom,

        left, bottom,
        left, top,
        right, bottom,
      };
      m_vertices.insert(m_vertices.end(), std::begin(vertices_lst), std::end(vertices_lst));

      auto uvs_lst = {
        uv_left, uv_top,
        uv_right, uv_top,
        uv_right, uv_bottom,

        uv_left, uv_bottom,
        uv_left, uv_top,
        uv_right, uv_bottom,
      };
      m_uvs.insert(m_uvs.end(), std::begin(uvs_lst), std::end(uvs_lst));
    }
    else
    {
      // Rotated blit.
      const float center_x = (left + right) / 2;
      const float center_y = (top + bottom) / 2;

      const float sa = sinf(math::radians(request.angles[i]));
      const float ca = cosf(math::radians(request.angles[i]));

      const float new_left = left - center_x;
      const float new_right = right - center_x;

      const float new_top = top - center_y;
      const float new_bottom = bottom - center_y;

      const float vertices_lst[] = {
        new_left*ca - new_top*sa + center_x, new_left*sa + new_top*ca + center_y,
        new_right*ca - new_top*sa + center_x, new_right*sa + new_top*ca + center_y,
        new_right*ca - new_bottom*sa + center_x, new_right*sa + new_bottom*ca + center_y,

        new_left*ca - new_bottom*sa + center_x, new_left*sa + new_bottom*ca + center_y,
        new_left*ca - new_top*sa + center_x, new_left*sa + new_top*ca + center_y,
        new_right*ca - new_bottom*sa + center_x, new_right*sa + new_bottom*ca + center_y,
      };
      m_vertices.insert(m_vertices.end(), std::begin(vertices_lst), std::end(vertices_lst));

      const float uvs_lst[] = {
        uv_left, uv_top,
        uv_right, uv_top,
        uv_right, uv_bottom,

        uv_left, uv_bottom,
        uv_left, uv_top,
        uv_right, uv_bottom,
      };
      m_uvs.insert(m_uvs.end(), std::begin(uvs_lst), std::end(uvs_lst));
    }
  }

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_texture(texture, request.displacement_texture);
  context.set_texcoords(m_uvs.data(), sizeof(float) * m_uvs.size());
  context.set_positions(m_vertices.data(), sizeof(float) * m_vertices.size());
  context.set_color(Color(request.color.red,
                          request.color.green,
                          request.color.blue,
                          request.color.alpha * request.alpha));

  context.draw_arrays(GL_TRIANGLES, 0, static_cast<GLsizei>(request.srcrects.size() * 2 * 3));

  assert_gl();
}

void
GLPainter::draw_gradient(const GradientRequest& request)
{
  assert_gl();

  const Color& top = request.top;
  const Color& bottom = request.bottom;
  const GradientDirection& direction = request.direction;
  const Rectf& region = request.region;

  GLContext& context = m_video_system.get_context();

  const float vertices[] = {
    region.get_left(), region.get_top(),
    region.get_right(), region.get_top(),
    region.get_right(), region.get_bottom(),
    region.get_left(), region.get_bottom()
  };

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_positions(vertices, sizeof(vertices));
  context.set_texcoord(0.0f, 0.0f);

  if (direction == VERTICAL || direction == VERTICAL_SECTOR)
  {
    const float colors[] = {
      top.red, top.green, top.blue, top.alpha,
      top.red, top.green, top.blue, top.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
    };
    context.set_colors(colors, sizeof(colors));
  }
  else
  {
    const float colors[] = {
      top.red, top.green, top.blue, top.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
      top.red, top.green, top.blue, top.alpha,
    };
    context.set_colors(colors, sizeof(colors));
  }

  context.draw_arrays(GL_TRIANGLE_FAN, 0, 4);

  assert_gl();
}

void
GLPainter::draw_filled_rect(const FillRectRequest& request)
{
  assert_gl();

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_texcoord(0.0f, 0.0f);
  context.set_color(request.color);

  if (request.radius != 0.0f)
  {
    // Draw a rounded rectangle.
    // Ensure that the radius stays within limits to avoid rendering irregular shapes.
    const float radius = std::min(request.radius,
                                  std::min(request.rect.get_width() / 2.0f,
                                           request.rect.get_height() / 2.0f));

    // Using Rectf here might result in an invalid Rectf due
    // to float imprecision, leading to assert().
    const float inner_rect_left = request.rect.get_left() + radius;
    const float inner_rect_top = request.rect.get_top() + radius;
    const float inner_rect_right = request.rect.get_right() - radius;
    const float inner_rect_bottom = request.rect.get_bottom() - radius;

    const int n = 8;
    size_t p = 0;
    std::vector<float> vertices((n+1) * 4 * 2);

    for (int i = 0; i <= n; ++i)
    {
      const float x = sinf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;
      const float y = cosf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;

      vertices[p++] = inner_rect_left - x;
      vertices[p++] = inner_rect_top  - y;

      vertices[p++] = inner_rect_right + x;
      vertices[p++] = inner_rect_top   - y;
    }

    for (int i = 0; i <= n; ++i)
    {
      const float x = cosf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;
      const float y = sinf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;

      vertices[p++] = inner_rect_left   - x;
      vertices[p++] = inner_rect_bottom + y;

      vertices[p++] = inner_rect_right  + x;
      vertices[p++] = inner_rect_bottom + y;
    }

    context.set_positions(vertices.data(), sizeof(float) * vertices.size());

    context.draw_arrays(GL_TRIANGLE_STRIP, 0,  static_cast<GLsizei>(vertices.size() / 2));
  }
  else
  {
    const float& x = request.rect.get_left();
    const float& y = request.rect.get_top();
    const float& w = request.rect.get_width();
    const float& h = request.rect.get_height();

    const float vertices[] = {
      x,   y,
      x+w, y,
      x+w, y+h,
      x,   y+h
    };

    context.set_positions(vertices, sizeof(vertices));

    context.draw_arrays(GL_TRIANGLE_FAN, 0, 4);
  }

  assert_gl();
}

void
GLPainter::draw_inverse_ellipse(const InverseEllipseRequest& request)
{
  assert_gl();

  const float& x = request.pos.x;
  const float& y = request.pos.y;
  const float w = request.size.x/2.0f;
  const float h = request.size.y/2.0f;

  constexpr int slices = 16;
  constexpr int points = (slices+1) * 12;

  float vertices[points * 2];
  int p = 0;

  const Viewport& viewport = m_video_system.get_viewport();
  const float screen_width = static_cast<float>(viewport.get_screen_width());
  const float screen_height = static_cast<float>(viewport.get_screen_height());

  // Bottom.
  vertices[p++] = screen_width; vertices[p++] = screen_height;
  vertices[p++] = 0;            vertices[p++] = screen_height;
  vertices[p++] = x;            vertices[p++] = y+h;

  // Top.
  vertices[p++] = screen_width; vertices[p++] = 0;
  vertices[p++] = 0;            vertices[p++] = 0;
  vertices[p++] = x;            vertices[p++] = y-h;

  // Left.
  vertices[p++] = screen_width; vertices[p++] = 0;
  vertices[p++] = screen_width; vertices[p++] = screen_height;
  vertices[p++] = x+w;          vertices[p++] = y;

  // Right.
  vertices[p++] = 0;            vertices[p++] = 0;
  vertices[p++] = 0;            vertices[p++] = screen_height;
  vertices[p++] = x-w;          vertices[p++] = y;

  for (int i = 0; i < slices; ++i)
  {
    const float ex1 = sinf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i)) * w;
    const float ey1 = cosf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i)) * h;

    const float ex2 = sinf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i+1)) * w;
    const float ey2 = cosf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i+1)) * h;

    // Bottom/Right.
    vertices[p++] = screen_width; vertices[p++] = screen_height;
    vertices[p++] = x + ex1;      vertices[p++] = y + ey1;
    vertices[p++] = x + ex2;      vertices[p++] = y + ey2;

    // Top/Left.
    vertices[p++] = 0;            vertices[p++] = 0;
    vertices[p++] = x - ex1;      vertices[p++] = y - ey1;
    vertices[p++] = x - ex2;      vertices[p++] = y - ey2;

    // Top/Right.
    vertices[p++] = screen_width; vertices[p++] = 0;
    vertices[p++] = x + ex1;      vertices[p++] = y - ey1;
    vertices[p++] = x + ex2;      vertices[p++] = y - ey2;

    // Bottom/Left
    vertices[p++] = 0;            vertices[p++] = screen_height;
    vertices[p++] = x - ex1;      vertices[p++] = y + ey1;
    vertices[p++] = x - ex2;      vertices[p++] = y + ey2;
  }

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_positions(vertices, sizeof(vertices));
  context.set_texcoord(0.0f, 0.0f);
  context.set_color(request.color);

  context.draw_arrays(GL_TRIANGLES, 0, points);

  assert_gl();
}

void
GLPainter::draw_line(const LineRequest& request)
{
  assert_gl();

  Vector viewport_scale = m_video_system.get_viewport().get_scale();
  const float& x1 = request.pos.x;
  const float& y1 = request.pos.y;
  const float& x2 = request.dest_pos.x;
  const float& y2 = request.dest_pos.y;

  // OpenGL3.3 doesn't have GL_LINES anymore, so instead we transform
  // the line into a quad and draw it as triangle strip.
  float x_step = (y2 - y1);
  float y_step = -(x2 - x1);

  const float step_norm = sqrtf(x_step * x_step + y_step * y_step);
  x_step /= step_norm * viewport_scale.x;
  y_step /= step_norm * viewport_scale.y;

  x_step *= 0.5f;
  y_step *= 0.5f;

  const float vertices[] = {
    (x1 - x_step), (y1 - y_step),
    (x2 - x_step), (y2 - y_step),
    (x1 + x_step), (y1 + y_step),
    (x2 + x_step), (y2 + y_step),
  };

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_positions(vertices, sizeof(vertices));
  context.set_texcoord(0.0f, 0.0f);
  context.set_color(request.color);

  context.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

  assert_gl();
}

void
GLPainter::draw_triangle(const TriangleRequest& request)
{
  assert_gl();

  const float vertices[] = {
    request.pos1.x, request.pos1.y,
    request.pos2.x, request.pos2.y,
    request.pos3.x, request.pos3.y,
  };

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_texcoord(0.0f, 0.0f);
  context.set_positions(vertices, sizeof(vertices));
  context.set_color(request.color);

  context.draw_arrays(GL_TRIANGLES, 0, 3);

  assert_gl();
}

void
GLPainter::clear(const Color& color)
{
  assert_gl();

  glClearColor(color.red, color.green, color.blue, color.alpha);
  glClear(GL_COLOR_BUFFER_BIT);

  assert_gl();
}

void
GLPainter::get_pixel(const GetPixelRequest& request) const
{
  assert_gl();

  const Rect& rect = m_renderer.get_rect();
  const Size& logical_size = m_renderer.get_logical_size();

  float x = request.pos.x * static_cast<float>(rect.get_width()) / static_cast<float>(logical_size.width);
  float y = request.pos.y * static_cast<float>(rect.get_height()) / static_cast<float>(logical_size.height);

  x += static_cast<float>(rect.left);
  y += static_cast<float>(rect.top);

#if 0
  // #ifndef USE_OPENGLES2
  //
  // FIXME: glFenceSync() causes crashes on Intel I965, so disable
  // GLPixelRequest for now, it's not yet properly used anyway.
  GLPixelRequest pixel_request(1, 1);
  pixel_request.request(static_cast<int>(x), static_cast<int>(y));

  *(request.color_ptr) = pixel_request.get_color();

#else
  float pixels[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

  // OpenGLES2 does not have PBOs, only GLES3 has.
  glReadPixels(static_cast<GLint>(x), static_cast<GLint>(y),
               1, 1, GL_RGB, GL_FLOAT, pixels);

  *(request.color_ptr) = Color(pixels[0], pixels[1], pixels[2]);
#endif

  assert_gl();
}

void
GLPainter::set_clip_rect(const Rect& clip_rect)
{
  assert_gl();

  const Rect& rect = m_renderer.get_rect();
  const Size& logical_size = m_renderer.get_logical_size();

  glScissor(rect.left + rect.get_width() * clip_rect.left / logical_size.width,
            rect.bottom - rect.get_height() * clip_rect.bottom / logical_size.height,
            rect.get_width() * clip_rect.get_width() / logical_size.width,
            rect.get_height() * clip_rect.get_height() / logical_size.height);
  glEnable(GL_SCISSOR_TEST);

  assert_gl();
}

void
GLPainter::clear_clip_rect()
{
  assert_gl();

  glDisable(GL_SCISSOR_TEST);

  assert_gl();
}

/* EOF */
