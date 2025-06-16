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

#include "object/gradient.hpp"

#include <utility>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "editor/editor.hpp"
#include "object/camera.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/level.hpp"
#include "supertux/sector.hpp"
#include "util/reader.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

const Color Gradient::DEFAULT_GRADIENT_TOP(0.3f, 0.4f, 0.75f);
const Color Gradient::DEFAULT_GRADIENT_BOTTOM(Color::WHITE);

Gradient::Gradient() :
  m_layer(LAYER_BACKGROUND0),
  m_gradient_top(DEFAULT_GRADIENT_TOP),
  m_gradient_bottom(DEFAULT_GRADIENT_BOTTOM),
  m_gradient_direction(),
  m_blend(),
  m_target(DrawingTarget::COLORMAP),
  m_start_gradient_top(),
  m_start_gradient_bottom(),
  m_fade_gradient_top(),
  m_fade_gradient_bottom(),
  m_fade_total_time(),
  m_fade_time()
{
}

Gradient::Gradient(const ReaderMapping& reader) :
  LayerObject(reader),
  m_layer(LAYER_BACKGROUND0),
  m_gradient_top(DEFAULT_GRADIENT_TOP),
  m_gradient_bottom(DEFAULT_GRADIENT_BOTTOM),
  m_gradient_direction(),
  m_blend(),
  m_target(DrawingTarget::COLORMAP),
  m_start_gradient_top(),
  m_start_gradient_bottom(),
  m_fade_gradient_top(),
  m_fade_gradient_bottom(),
  m_fade_total_time(),
  m_fade_time()
{
  m_layer = reader_get_layer (reader, LAYER_BACKGROUND0);
  std::vector<float> bkgd_top_color, bkgd_bottom_color;
  std::string direction;
  if (reader.get("direction", direction))
  {
    set_direction(direction);
  }
  else
  {
    set_direction(VERTICAL);
  }

  if (reader.get("top_color", bkgd_top_color))
    m_gradient_top = Color(bkgd_top_color);

  if (reader.get("bottom_color", bkgd_bottom_color))
    m_gradient_bottom = Color(bkgd_bottom_color);

  reader.get_custom("blend", m_blend, Blend_from_string);
  reader.get_custom("target", m_target, DrawingTarget_from_string);
}

ObjectSettings
Gradient::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_rgba(_("Primary Colour"), &m_gradient_top, "top_color");
  result.add_rgba(_("Secondary Colour"), &m_gradient_bottom, "bottom_color");

  result.add_int(_("Z-pos"), &m_layer, "z-pos", LAYER_BACKGROUND0);

  result.add_enum(_("Direction"), reinterpret_cast<int*>(&m_gradient_direction),
                  {_("Vertical"), _("Horizontal"), _("Vertical (whole sector)"), _("Horizontal (whole sector)")},
                  {"vertical", "horizontal", "vertical_sector", "horizontal_sector"},
                  static_cast<int>(VERTICAL), "direction");

  result.add_enum(_("Draw target"), reinterpret_cast<int*>(&m_target),
                  {_("Normal"), _("Lightmap")},
                  {"normal", "lightmap"},
                  static_cast<int>(DrawingTarget::COLORMAP),
                  "target");

  result.add_enum(_("Blend mode"), reinterpret_cast<int*>(&m_blend),
                  {_("Blend"), _("Additive"), _("Modulate"), _("None")},
                  {"blend", "add", "mod", "none"},
                  static_cast<int>(Blend::BLEND),
                  "blend");

  result.reorder({"blend", "top_color", "bottom_color", "target", "z-pos"});

  result.add_remove();

  return result;
}

Gradient::~Gradient()
{
}

void
Gradient::update(float delta)
{
  if (m_fade_time <= 0) return;
  
  m_fade_time -= delta;
  if (m_fade_time <= 0)
  {
    m_fade_time = 0;
    
    m_gradient_top = m_fade_gradient_top;
    m_gradient_bottom = m_fade_gradient_bottom;
    
    return;
  }
  
  float progress = m_fade_time / m_fade_total_time;
  m_gradient_top =
    (m_fade_gradient_top + (m_start_gradient_top - m_fade_gradient_top) * progress).validate();
  m_gradient_bottom =
    (m_fade_gradient_bottom + (m_start_gradient_bottom - m_fade_gradient_bottom) * progress).validate();
}

void
Gradient::set_gradient(const Color& top, const Color& bottom)
{
  m_gradient_top.red = std::clamp(m_gradient_top.red, 0.f, 1.f);
  m_gradient_top.green = std::clamp(m_gradient_top.green, 0.f, 1.f);
  m_gradient_top.blue = std::clamp(m_gradient_top.blue, 0.f, 1.f);
  m_gradient_top.alpha = std::clamp(m_gradient_top.alpha, 0.f, 1.f);

  m_gradient_bottom.red = std::clamp(m_gradient_bottom.red, 0.f, 1.f);
  m_gradient_bottom.green = std::clamp(m_gradient_bottom.green, 0.f, 1.f);
  m_gradient_bottom.blue = std::clamp(m_gradient_bottom.blue, 0.f, 1.f);
  m_gradient_bottom.alpha = std::clamp(m_gradient_bottom.alpha, 0.f, 1.f);

  m_gradient_top = top;
  m_gradient_bottom = bottom;
}

void
Gradient::fade_gradient(const Color& top, const Color& bottom, float time)
{
  m_start_gradient_top = m_gradient_top;
  m_start_gradient_bottom = m_gradient_bottom;
  m_fade_gradient_top = top;
  m_fade_gradient_bottom = bottom;
  m_fade_total_time = time;
  m_fade_time = time;
}

std::string
Gradient::get_direction_string() const
{
  if (m_gradient_direction == HORIZONTAL)
    return "horizontal";
  if (m_gradient_direction == VERTICAL)
    return "vertical";
  if (m_gradient_direction == HORIZONTAL_SECTOR)
    return "horizontal_sector";
  if (m_gradient_direction == VERTICAL_SECTOR)
    return "vertical_sector";

  return "";
}

void
Gradient::set_direction(const std::string& direction)
{
  if (direction == "horizontal")
  {
    m_gradient_direction = HORIZONTAL;
  }
  else if (direction == "horizontal_sector")
  {
    m_gradient_direction = HORIZONTAL_SECTOR;
  }
  else if (direction == "vertical_sector")
  {
    m_gradient_direction = VERTICAL_SECTOR;
  }
  else if (direction == "vertical")
  {
    m_gradient_direction = VERTICAL;
  }
  else
  {
    log_info << "Invalid direction for gradient \"" << direction << "\"";
    m_gradient_direction = VERTICAL;
  }
}

void
Gradient::set_color1(float red, float green, float blue, float alpha)
{
  set_gradient(Color(red, green, blue, alpha), m_gradient_bottom);
}

void
Gradient::set_color2(float red, float green, float blue, float alpha)
{
  set_gradient(m_gradient_top, Color(red, green, blue, alpha));
}

void
Gradient::set_colors(float red1, float green1, float blue1, float red2, float green2, float blue2, float alpha1, float alpha2)
{
  set_gradient(Color(red1, green1, blue1, alpha1), Color(red2, green2, blue2, alpha2));
}

void
Gradient::fade_color1(float red, float green, float blue, float time, float alpha)
{
  fade_gradient(Color(red, green, blue, alpha), m_gradient_bottom, time);
}

void
Gradient::fade_color2(float red, float green, float blue, float time, float alpha)
{
  fade_gradient(m_gradient_top, Color(red, green, blue, alpha), time);
}

void
Gradient::fade_colors(float red1, float green1, float blue1, float red2, float green2, float blue2, float time, float alpha1, float alpha2)
{
  fade_gradient(Color(red1, green1, blue1, alpha1), Color(red2, green2, blue2, alpha2), time);
}

void
Gradient::swap_colors()
{
  set_gradient(m_gradient_bottom, m_gradient_top);
}

void
Gradient::draw(DrawingContext& context)
{
  if (Editor::is_active() && !g_config->editor_render_background)
    return;

  Rectf gradient_region;
  if (m_gradient_direction != HORIZONTAL && m_gradient_direction != VERTICAL)
  {
      auto camera_translation = Sector::get().get_camera().get_translation();
      auto sector_width = Sector::get().get_width();
      auto sector_height = Sector::get().get_height();
      gradient_region = Rectf(-camera_translation.x, -camera_translation.y, sector_width, sector_height);
  }
  else
  {
    gradient_region = context.get_rect();
  }

  context.push_transform();
  context.set_translation(Vector(0, 0));
  context.transform().scale = 1.f;
  context.get_canvas(m_target).draw_gradient(m_gradient_top, m_gradient_bottom, m_layer, m_gradient_direction,
                                             gradient_region, m_blend);
  context.pop_transform();
}

bool
Gradient::is_saveable() const
{
  return !(Level::current() &&
           Level::current()->is_worldmap());
}

void
Gradient::on_flip(float height)
{
  GameObject::on_flip(height);
  if (m_gradient_direction == VERTICAL || m_gradient_direction == VERTICAL_SECTOR)
    std::swap(m_gradient_top, m_gradient_bottom);
}


void
Gradient::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Gradient>("Gradient", vm.findClass("GameObject"));

  cls.addFunc<void, Gradient, const std::string&>("set_direction", &Gradient::set_direction);
  cls.addFunc("get_direction", &Gradient::get_direction_string);
  cls.addFunc("set_color1", &Gradient::set_color1, ssq::DefaultArguments<float>(1.f));
  cls.addFunc("set_color2", &Gradient::set_color2, ssq::DefaultArguments<float>(1.f));
  cls.addFunc("set_colors", &Gradient::set_colors, ssq::DefaultArguments<float, float>(1.f, 1.f));
  cls.addFunc("fade_color1", &Gradient::fade_color1, ssq::DefaultArguments<float>(1.f));
  cls.addFunc("fade_color2", &Gradient::fade_color2, ssq::DefaultArguments<float>(1.f));
  cls.addFunc("fade_colors", &Gradient::fade_colors, ssq::DefaultArguments<float, float>(1.f, 1.f));
  cls.addFunc("swap_colors", &Gradient::swap_colors);
}

/* EOF */
