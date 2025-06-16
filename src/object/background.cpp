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

#include "object/background.hpp"

#include <utility>

#include <physfs.h>
#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "editor/editor.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/reader.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "video/drawing_context.hpp"

static const std::unordered_map<std::string, std::string> fallback_paths = {
  {"arctis2.png", "antarctic/arctis2.png"},
  {"misty_snowhills_small.png", "antarctic/misty_snowhills_small.png"},
  {"semi_arctic.jpg", "antarctic/semi_arctic.jpg"},
  {"bridgecloud-dark.png", "arctic_bridge/bridgecloud-dark.png"},
  {"bridgecloud-light.png", "arctic_bridge/bridgecloud-light.png"},
  {"bridgeocean-fade.png", "arctic_bridge/bridgeocean-fade.png"},
  {"bridgeocean-nofade.png", "arctic_bridge/bridgeocean-nofade.png"},
  {"bridgeocean-original.png", "arctic_bridge/bridgeocean-original.png"},
  {"arcticskies1.png", "arcticskies/arcticskies1.png"},
  {"arcticskies2.png", "arcticskies/arcticskies2.png"},
  {"arcticskies3.png", "arcticskies/arcticskies3.png"},
  {"arcticskies35.png", "arcticskies/arcticskies35.png"},
  {"arcticskies4.png", "arcticskies/arcticskies4.png"},
  {"block-snow-background.png", "block_snow/block-snow-background.png"},
  {"block-snow-midground.png", "block_snow/block-snow-midground.png"},
  {"block-snow-top.png", "block_snow/block-snow-top.png"},
  {"bluemountain-bottom.png", "bluemountain/bluemountain-bottom.png"},
  {"bluemountain-middle.png", "bluemountain/bluemountain-middle.png"},
  {"bluemountain-top.png", "bluemountain/bluemountain-top.png"},
  {"bluemountain2.png", "bluemountain/bluemountain2.png"},
  {"castle_foreground.png", "castle/castle_foreground.png"},
  {"snowcastle.png", "castle/snowcastle.png"},
  {"cloud-mountains-background.png", "cloud_mountains/cloud-mountains-background.png"},
  {"cloud-mountains-bottom.png", "cloud_mountains/cloud-mountains-bottom.png"},
  {"cloud-mountains-forground.png", "cloud_mountains/cloud-mountains-forground.png"},
  {"cloud-mountains-midground.png", "cloud_mountains/cloud-mountains-midground.png"},
  {"dawn_hill_para_blur.png", "forest/dawn_hill_para_blur.png"},
  {"forest2_para.png", "forest/forest2_para.png"},
  {"forest_para2.png", "forest/forest_para2.png"},
  {"forest_para3.png", "forest/forest_para3.png"},
  {"forest_para3_bottom.png", "forest/forest_para3_bottom.png"},
  {"nighthills.png", "forest/nighthills.png"},
  {"ghostforest.jpg", "ghostforest/ghostforest.jpg"},
  {"ghostforest_grave.png", "ghostforest/ghostforest_grave.png"},
  {"ghostforest_para.png", "ghostforest/ghostforest_para.png"},
  {"cave2.jpg", "ice_cave/cave2.jpg"},
  {"darkcave-background.png", "ice_cave/darkcave-background.png"},
  {"darkcave-middle.png", "ice_cave/darkcave-middle.png"},
  {"darkcave-preview.png", "ice_cave/darkcave-preview.png"},
  {"darkcave-top_and_bottom.png", "ice_cave/darkcave-top_and_bottom.png"},
  {"darkcavemidground-middle.png", "ice_cave/darkcavemidground-middle.png"},
  {"darkcavemidground-top_and_bottom.png", "ice_cave/darkcavemidground-top_and_bottom.png"},
  {"black_800px.png", "misc/black_800px.png"},
  {"fog.png", "misc/fog.png"},
  {"grid.png", "misc/grid.png"},
  {"grid.surface", "misc/grid.surface"},
  {"heatshimmer-displacement.png", "misc/heatshimmer-displacement.png"},
  {"heatshimmer.png", "misc/heatshimmer.png"},
  {"heatshimmer.surface", "misc/heatshimmer.surface"},
  {"leaves.png", "misc/leaves.png"},
  {"oiltux.jpg", "misc/oiltux.jpg"},
  {"transparent_up.png", "misc/transparent_up.png"},
  {"nightsky.png", "nightsky/nightsky.png"},
  {"nightsky_bottom.png", "nightsky/nightsky_bottom.png"},
  {"nightsky_middle.png", "nightsky/nightsky_middle.png"},
  {"nightsky_para.png", "nightsky/nightsky_para.png"},
  {"nightsky_top.png", "nightsky/nightsky_top.png"},
};

static SpritePtr load_background_sprite(const std::string& image_path)
{
  if (image_path.empty())
    return nullptr;

  if (PHYSFS_exists(image_path.c_str()))
    // No need to search fallback paths.
    return SpriteManager::current()->create(image_path);

  // Search for a fallback image in fallback_paths.
  static const std::string default_dir = "images/background/";
  static const std::string default_dir2 = "/images/background/";
  std::string new_path = image_path;
  if (image_path.substr(0, default_dir.length()) == default_dir)
    new_path.erase(0, default_dir.length());
  else if (image_path.substr(0, default_dir2.length()) == default_dir2)
    new_path.erase(0, default_dir2.length());

  auto it = fallback_paths.find(new_path);
  if (it == fallback_paths.end())
    // Unknown image, try checking for a ".deprecated" version, or use the dummy texture.
    return SpriteManager::current()->create(image_path);

  new_path = default_dir + it->second;
  return SpriteManager::current()->create(new_path);
}

static SpritePtr load_background(const std::string& image_path)
{
  SpritePtr sprite = load_background_sprite(image_path);
  if (!sprite) return nullptr;

  if (Editor::is_active())
    sprite->pause_animation();

  return sprite;
}


Background::Background() :
  m_alignment(NO_ALIGNMENT),
  m_fill(false),
  m_layer(LAYER_BACKGROUND0),
  m_imagefile_top(),
  m_imagefile(),
  m_imagefile_bottom(),
  m_pos(0.0f, 0.0f),
  m_parallax_speed(0.0f, 0.0f),
  m_scroll_speed(0.0f, 0.0f),
  m_scroll_offset(0.0f, 0.0f),
  m_image_top(),
  m_image(),
  m_image_bottom(),
  m_blend(),
  m_color(1.f, 1.f, 1.f),
  m_target(DrawingTarget::COLORMAP),
  m_timer_color(),
  m_src_color(),
  m_dst_color(),
  m_flip(NO_FLIP)
{
}

Background::Background(const ReaderMapping& reader) :
  LayerObject(reader),
  m_alignment(NO_ALIGNMENT),
  m_fill(false),
  m_layer(LAYER_BACKGROUND0),
  m_imagefile_top(),
  m_imagefile(),
  m_imagefile_bottom(),
  m_pos(0.0f, 0.0f),
  m_parallax_speed(1.0f, 1.0f),
  m_scroll_speed(0.0f, 0.0f),
  m_scroll_offset(0.0f, 0.0f),
  m_image_top(),
  m_image(),
  m_image_bottom(),
  m_blend(),
  m_color(),
  m_target(DrawingTarget::COLORMAP),
  m_timer_color(),
  m_src_color(),
  m_dst_color(),
  m_flip(NO_FLIP)
{
  reader.get("fill", m_fill);

  std::string alignment_str;
  if (reader.get("alignment", alignment_str))
  {
    if (alignment_str == "left")
    {
      m_alignment = LEFT_ALIGNMENT;
    }
    else if (alignment_str == "right")
    {
      m_alignment = RIGHT_ALIGNMENT;
    }
    else if (alignment_str == "top")
    {
      m_alignment = TOP_ALIGNMENT;
    }
    else if (alignment_str == "bottom")
    {
      m_alignment = BOTTOM_ALIGNMENT;
    }
    else if (alignment_str == "none")
    {
      m_alignment = NO_ALIGNMENT;
    }
    else
    {
      log_warning << "Background: invalid alignment: '" << alignment_str << "'" << std::endl;
      m_alignment = NO_ALIGNMENT;
    }
  }

  reader.get("scroll-offset-x", m_scroll_offset.x, 0.0f);
  reader.get("scroll-offset-y", m_scroll_offset.y, 0.0f);

  // For backward compatibility, add position to scroll offset.
  float px;
  float py;
  if (reader.get("x", px))
    m_scroll_offset.x += px;
  if (reader.get("y", py))
    m_scroll_offset.y += py;

  reader.get("scroll-speed-x", m_scroll_speed.x, 0.0f);
  reader.get("scroll-speed-y", m_scroll_speed.y, 0.0f);

  m_layer = reader_get_layer(reader, LAYER_BACKGROUND0);

  reader.get("image", m_imagefile, "images/background/misc/transparent_up.png");
  m_image = load_background(m_imagefile);

  if(!reader.get("speed-x", m_parallax_speed.x))
  {
    // For backward compatibility.
    reader.get("speed", m_parallax_speed.x, 0.5f);
  }

  reader.get("speed-y", m_parallax_speed.y, m_parallax_speed.x);

  if (reader.get("image-top", m_imagefile_top)) {
    m_image_top = load_background(m_imagefile_top);
  } else {
    if (!Editor::is_active()) {
      m_imagefile_top = m_imagefile;
    }
  }

  if (reader.get("image-bottom", m_imagefile_bottom)) {
    m_image_bottom = load_background(m_imagefile_bottom);
    } else {
    if (!Editor::is_active()) {
      m_imagefile_bottom = m_imagefile;
    }
  }

  std::vector<float> color;
  if (reader.get("color", color))
  {
    m_color = Color(color);
  }
  else
  {
    m_color = Color(1, 1, 1);
  }

  reader.get_custom("blend", m_blend, Blend_from_string);
  reader.get_custom("target", m_target, DrawingTarget_from_string);
}

Background::~Background()
{
}

ObjectSettings
Background::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_float(_("X"), &m_pos.x, "x", 0.0f, OPTION_HIDDEN);
  result.add_float(_("Y"), &m_pos.y, "y", 0.0f, OPTION_HIDDEN);

  result.add_bool(_("Fill"), &m_fill, "fill", false);
  result.add_int(_("Z-pos"), &m_layer, "z-pos", LAYER_BACKGROUND0);
  result.add_enum(_("Alignment"), reinterpret_cast<int*>(&m_alignment),
                  {_("none"), _("left"), _("right"), _("top"), _("bottom")},
                  {"none", "left", "right", "top", "bottom"},
                  static_cast<int>(NO_ALIGNMENT), "alignment");
  result.add_float(_("Scroll offset x"), &m_scroll_offset.x, "scroll-offset-x", 0.0f);
  result.add_float(_("Scroll offset y"), &m_scroll_offset.y, "scroll-offset-y", 0.0f);
  result.add_float(_("Scroll speed x"), &m_scroll_speed.x, "scroll-speed-x", 0.0f);
  result.add_float(_("Scroll speed y"), &m_scroll_speed.y, "scroll-speed-y", 0.0f);
  result.add_float(_("Parallax Speed x"), &m_parallax_speed.x, "speed", std::nullopt);
  result.add_float(_("Parallax Speed y"), &m_parallax_speed.y, "speed-y", m_parallax_speed.x);
  result.add_sprite(_("Top image"), &m_imagefile_top, "image-top", "");
  result.add_sprite(_("Image"), &m_imagefile, "image");
  result.add_sprite(_("Bottom image"), &m_imagefile_bottom, "image-bottom", "");
  result.add_rgba(_("Colour"), &m_color, "color");
  result.add_enum(_("Draw target"), reinterpret_cast<int*>(&m_target),
                  {_("Normal"), _("Lightmap")},
                  {"normal", "lightmap"},
                  static_cast<int>(DrawingTarget::COLORMAP),
                  "target");

  result.reorder({"x", "y", "alignment", "scroll-speed-x", "scroll-speed-y", "speed", "speed-y", "fill", "target", "image-top", "image", "image-bottom", "z-pos"});

  result.add_remove();

  return result;
}

void
Background::after_editor_set()
{
  m_image_top = load_background(m_imagefile_top);
  m_image = load_background(m_imagefile);
  m_image_bottom = load_background(m_imagefile_bottom);
}

void
Background::update(float dt_sec)
{
  m_scroll_offset += m_scroll_speed * dt_sec;

  if (m_timer_color.check())
  {
    m_color = m_dst_color;
    m_timer_color.stop(); // To reset the "check()" value.
  }
  else if (m_timer_color.started())
  {
    float progress = m_timer_color.get_progress();

    m_color = (m_src_color + (m_dst_color - m_src_color) * progress).validate();
  }
}

void
Background::fade_color(Color color, float time)
{
  m_src_color = m_color;
  m_dst_color = color;

  m_timer_color.start(time, false);

  m_color = m_src_color;
}

void
Background::set_image(const std::string& name)
{
  m_imagefile = name;
  m_image = load_background(name);
}

void
Background::set_images(const std::string& name_top,
                       const std::string& name_middle,
                       const std::string& name_bottom)
{
  m_image_top = load_background(name_top);
  m_imagefile_top = name_top;

  m_image = load_background(name_middle);
  m_imagefile = name_middle;

  m_image_bottom = load_background(name_bottom);
  m_imagefile_bottom = name_bottom;
}

void
Background::set_speed(float speed)
{
  m_parallax_speed.x = speed;
  m_parallax_speed.y = speed;
}

void
Background::draw_image(DrawingContext& context, const Vector& pos_)
{
  const Sizef level(d_gameobject_manager->get_width(), d_gameobject_manager->get_height());
  const Sizef screen = context.get_viewport().get_size();
  const Sizef parallax_image_size((1.0f - m_parallax_speed.x) * screen.width + level.width * m_parallax_speed.x,
                                  (1.0f - m_parallax_speed.y) * screen.height + level.height * m_parallax_speed.y);

  const Rectf cliprect = context.get_cliprect();
  const float img_w = static_cast<float>(m_image->get_width());
  const float img_h = static_cast<float>(m_image->get_height());

  const float img_w_2 = img_w / 2.0f;
  const float img_h_2 = img_h / 2.0f;

  const int start_x = static_cast<int>(floorf((cliprect.get_left() - (pos_.x - img_w /2.0f)) / img_w));
  const int end_x   = static_cast<int>(ceilf((cliprect.get_right() - (pos_.x + img_w /2.0f)) / img_w)) + 1;
  const int start_y = static_cast<int>(floorf((cliprect.get_top() - (pos_.y - img_h/2.0f)) / img_h));
  const int end_y   = static_cast<int>(ceilf((cliprect.get_bottom() - (pos_.y + img_h/2.0f)) / img_h)) + 1;

  Canvas& canvas = context.get_canvas(m_target);

  m_image->set_color(m_color);
  m_image->set_blend(m_blend);

  if (m_fill)
  {
    Rectf dstrect(Vector(pos_.x - context.get_width() / 2.0f,
                         pos_.y - context.get_height() / 2.0f),
                  Sizef(context.get_width(),
                        context.get_height()));
    m_image->draw_scaled(canvas, dstrect, m_layer);
  }
  else
  {
    switch (m_alignment)
    {
      case LEFT_ALIGNMENT:
        for (int y = start_y; y < end_y; ++y)
        {
          Vector p(pos_.x - parallax_image_size.width / 2.0f,
                   pos_.y + static_cast<float>(y) * img_h - img_h_2);
          m_image->draw(canvas, p, m_layer);
        }
        break;

      case RIGHT_ALIGNMENT:
        for (int y = start_y; y < end_y; ++y)
        {
          Vector p(pos_.x + parallax_image_size.width / 2.0f - img_w,
                   pos_.y + static_cast<float>(y) * img_h - img_h_2);
          m_image->draw(canvas, p, m_layer);
        }
        break;

      case TOP_ALIGNMENT:
        for (int x = start_x; x < end_x; ++x)
        {
          Vector p(pos_.x + static_cast<float>(x) * img_w - img_w_2,
                   pos_.y - parallax_image_size.height / 2.0f);
          m_image->draw(canvas, p, m_layer);
        }
        break;

      case BOTTOM_ALIGNMENT:
        for (int x = start_x; x < end_x; ++x)
        {
          Vector p(pos_.x + static_cast<float>(x) * img_w - img_w_2,
                   pos_.y - img_h + parallax_image_size.height / 2.0f);
          m_image->draw(canvas, p, m_layer);
        }
        break;

      case NO_ALIGNMENT:
        for (int y = start_y; y < end_y; ++y)
          for (int x = start_x; x < end_x; ++x)
          {
            Vector p(pos_.x + static_cast<float>(x) * img_w - img_w_2,
                     pos_.y + static_cast<float>(y) * img_h - img_h_2);

            if (m_image_top && (y < 0))
            {
              m_image_top->set_color(m_color);
              m_image_top->set_blend(m_blend);

              m_image_top->draw(canvas, p, m_layer);
            }
            else if (m_image_bottom && (y > 0))
            {
              m_image_bottom->set_color(m_color);
              m_image_bottom->set_blend(m_blend);

              m_image_bottom->draw(canvas, p, m_layer);
            }
            else
            {
              m_image->draw(canvas, p, m_layer);
            }
          }
        break;
    }
  }
}

void
Background::draw(DrawingContext& context)
{
  if (Editor::is_active() && !g_config->editor_render_background)
    return;

  if (!m_image)
    return;
    
  context.push_transform();
  if (!context.perspective_scale(m_parallax_speed.x, m_parallax_speed.y)) {
    //The background is placed behind the camera.
    context.pop_transform();
    return;
  }
  context.set_flip(context.get_flip() ^ m_flip);

  const Sizef level_size(d_gameobject_manager->get_width(),
                   d_gameobject_manager->get_height());
  const Sizef screen = context.get_viewport().get_size();
  const Sizef translation_range = level_size - screen;
  const Vector center_offset(context.get_translation().x - translation_range.width  / 2.0f,
                             context.get_translation().y - translation_range.height / 2.0f);

  const Vector pos(level_size.width / 2,
                   level_size.height / 2);
  draw_image(context, pos + m_scroll_offset + Vector(center_offset.x * (1.0f - m_parallax_speed.x),
                                                     center_offset.y * (1.0f - m_parallax_speed.y)));
  context.pop_transform();
}

void
Background::set_top_image_action(const std::string& action)
{
  if (m_image_top)
    m_image_top->set_action(action);
}

void
Background::set_image_action(const std::string& action)
{
  m_image->set_action(action);
}

void
Background::set_bottom_image_action(const std::string& action)
{
  if (m_image_bottom)
    m_image_bottom->set_action(action);
}

void
Background::set_all_image_actions(const std::string& action)
{
  set_top_image_action(action);
  set_image_action(action);
  set_bottom_image_action(action);
}

void
Background::on_flip(float height)
{
  GameObject::on_flip(height);
  std::swap(m_image_bottom, m_image_top);
  m_pos.y = height - m_pos.y - static_cast<float>(m_image->get_height());
  m_scroll_offset.y = -m_scroll_offset.y;
  if (m_alignment == BOTTOM_ALIGNMENT)
    m_alignment = TOP_ALIGNMENT;
  else if (m_alignment == TOP_ALIGNMENT)
    m_alignment = BOTTOM_ALIGNMENT;
  FlipLevelTransformer::transform_flip(m_flip);
}


void
Background::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Background>("Background", vm.findClass("GameObject"));

  cls.addFunc("set_image", &Background::set_image);
  cls.addFunc("set_images", &Background::set_images);
  cls.addFunc("set_speed", &Background::set_speed);
  cls.addFunc("get_color_red", &Background::get_color_red);
  cls.addFunc("get_color_green", &Background::get_color_green);
  cls.addFunc("get_color_blue", &Background::get_color_blue);
  cls.addFunc("get_color_alpha", &Background::get_color_alpha);
  cls.addFunc("set_color", &Background::set_color);
  cls.addFunc<void, Background, float, float, float, float, float>("fade_color", &Background::fade_color);
}

/* EOF */
