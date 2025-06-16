//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/compositor.hpp"

#include "math/rect.hpp"
#include "video/drawing_context.hpp"
#include "video/drawing_request.hpp"
#include "video/painter.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

bool Compositor::s_render_lighting = true;

Compositor::Compositor(VideoSystem& video_system, float time_offset) :
  m_video_system(video_system),
  m_obst(),
  m_drawing_contexts(),
  m_time_offset(time_offset)
{
  obstack_init(&m_obst);
}

Compositor::~Compositor()
{
  m_drawing_contexts.clear();
  obstack_free(&m_obst, nullptr);
}

DrawingContext&
Compositor::make_context(bool overlay)
{
  m_drawing_contexts.emplace_back(new DrawingContext(m_video_system, m_obst, overlay, m_time_offset));
  return *m_drawing_contexts.back();
}

void
Compositor::render()
{
  auto& lightmap = m_video_system.get_lightmap();

  bool use_lightmap = std::any_of(m_drawing_contexts.begin(), m_drawing_contexts.end(),
                                  [](std::unique_ptr<DrawingContext>& ctx){
                                    return ctx->use_lightmap();
                                  });

  use_lightmap = use_lightmap && s_render_lighting;

  // Prepare lightmap.
  if (use_lightmap)
  {
    lightmap.start_draw();
    Painter& painter = lightmap.get_painter();

    for (auto& ctx : m_drawing_contexts)
    {
      if (!ctx->is_overlay())
      {
        painter.clear(ctx->get_ambient_color());

        ctx->light().render(lightmap, Canvas::ALL);
      }
    }
    lightmap.end_draw();
  }

  auto back_renderer = m_video_system.get_back_renderer();
  if (back_renderer)
  {
    back_renderer->start_draw();

    for (auto& ctx : m_drawing_contexts)
    {
      ctx->color().render(*back_renderer, Canvas::BELOW_LIGHTMAP);
    }

    back_renderer->end_draw();
  }

  // Compose the screen.
  {
    auto& renderer = m_video_system.get_renderer();

    renderer.start_draw();

    for (auto& ctx : m_drawing_contexts)
    {
      ctx->color().render(renderer, Canvas::BELOW_LIGHTMAP);
    }

    if (use_lightmap)
    {
      const TexturePtr& texture = lightmap.get_texture();
      if (texture)
      {
        DrawingTransform transform(m_video_system.get_viewport());
        TextureRequest request(transform);

        request.blend = Blend::MOD;

        request.srcrects.emplace_back(0.0f, 0.0f,
                                      static_cast<float>(texture->get_image_width()),
                                      static_cast<float>(texture->get_image_height()));
        request.dstrects.emplace_back(Vector(0.0f, 0.0f), lightmap.get_logical_size());
        request.angles.emplace_back(0.0f);

        request.texture = texture.get();
        request.color = Color::WHITE;

        renderer.get_painter().draw_texture(request);
      }
    }

    // Render overlay elements.
    for (auto& ctx : m_drawing_contexts)
    {
      ctx->color().render(renderer, Canvas::ABOVE_LIGHTMAP);
    }

    renderer.end_draw();
  }

  // Clean up.
  for (auto& ctx : m_drawing_contexts)
  {
    ctx->clear();
  }
  m_video_system.flip();

  obstack_free(&m_obst, nullptr);
  obstack_init(&m_obst);
}

/* EOF */
