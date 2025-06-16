//  SuperTux - MagicBlock
//
//  Magic Blocks are tile-like game objects that are sensitive to
//  lighting conditions. They are rendered in a color and
//  will only be solid as long as light of the same color shines
//  on the block.
//
//  Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
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

#include "object/magicblock.hpp"

#include "editor/editor.hpp"
#include "object/camera.hpp"
#include "sprite/sprite.hpp"
#include "supertux/constants.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

const float MIN_INTENSITY = 0.8f;
const float ALPHA_SOLID = 0.7f;
const float ALPHA_NONSOLID = 0.3f;
const float MIN_SOLIDTIME = 1.0f;
const float SWITCH_DELAY = 0.0f; /**< seconds to wait for stable conditions until switching solidity */

} // namespace

MagicBlock::MagicBlock(const ReaderMapping& mapping) :
  MovingSprite(mapping, "images/objects/magicblock/magicblock.sprite"),
  m_is_solid(false),
  m_trigger_red(),
  m_trigger_green(),
  m_trigger_blue(),
  m_solid_time(0),
  m_switch_delay(0),
  m_solid_box(),
  m_color(),
  m_light(std::make_shared<Color>(1.0f,1.0f,1.0f)),
  m_center(0.0f, 0.0f),
  m_black()
{
  set_group(COLGROUP_STATIC);

  std::vector<float> vColor;
  if (mapping.get("color", vColor )) {
    m_color = Color( vColor );
  } else {
    m_color = Color(0, 0, 0);
  }

  if (!Editor::is_active()) {
    // all alpha to make the sprite still visible
    m_color.alpha = ALPHA_SOLID;

    // set trigger
    set_trigger_color();
  }

  m_center = m_col.m_bbox.get_middle();
  m_solid_box = Rectf(m_col.m_bbox.get_left() + SHIFT_DELTA, m_col.m_bbox.get_top() + SHIFT_DELTA, m_col.m_bbox.get_right() - SHIFT_DELTA, m_col.m_bbox.get_bottom() - SHIFT_DELTA);
}

ObjectSettings
MagicBlock::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_rgb(_("Color"), &m_color, "color", Color::BLACK);

  result.reorder({"color", "x", "y"});

  return result;
}

void
MagicBlock::after_editor_set()
{
  MovingSprite::after_editor_set();
  set_trigger_color();
  m_sprite->set_color(m_color);
}

void
MagicBlock::set_trigger_color()
{
  if (m_color.red == 0 && m_color.green == 0 && m_color.blue == 0)
  { 
    //is it black?
    m_black = true;
    m_trigger_red = MIN_INTENSITY;
    m_trigger_green = MIN_INTENSITY;
    m_trigger_blue = MIN_INTENSITY;
  }
  else
  {
    m_black = false;
    m_trigger_red = m_color.red;
    m_trigger_green = m_color.green;
    m_trigger_blue = m_color.blue;
  }
}

void
MagicBlock::update(float dt_sec)
{
  // Check if center of this block is on screen.
  // Don't update if not, because there is no light off screen.
  float screen_left = Sector::get().get_camera().get_translation().x;
  float screen_top = Sector::get().get_camera().get_translation().y;
  float screen_right = screen_left + static_cast<float>(SCREEN_WIDTH);
  float screen_bottom = screen_top + static_cast<float>(SCREEN_HEIGHT);
  if ((m_center.x > screen_right ) || (m_center.y > screen_bottom) ||
      (m_center.x < screen_left) || (m_center.y < screen_top)) {
    m_switch_delay = SWITCH_DELAY;
    return;
  }

  bool lighting_ok;
  if (m_black) {
    lighting_ok = (m_light->red >= m_trigger_red ||
                   m_light->green >= m_trigger_green ||
                   m_light->blue >= m_trigger_blue);
  } else {
    lighting_ok = (m_light->red >= m_trigger_red &&
                   m_light->green >= m_trigger_green &&
                   m_light->blue >= m_trigger_blue);
  }

  // overrule lighting_ok if switch_delay has not yet passed
  if (lighting_ok == m_is_solid) {
    m_switch_delay = SWITCH_DELAY;
  } else {
    if (m_switch_delay > 0) {
      lighting_ok = m_is_solid;
      m_switch_delay -= dt_sec;
    }
  }

  if (lighting_ok) {
    // lighting suggests going solid

    if (!m_is_solid) {
      if (Sector::get().is_free_of_movingstatics(m_solid_box, this)) {
        m_is_solid = true;
        m_solid_time = 0;
        m_switch_delay = SWITCH_DELAY;
      }
    }
  } else {
    // lighting suggests going nonsolid

    if ( m_solid_time >= MIN_SOLIDTIME ){
      m_is_solid = false;
    }
  }

  // Update Sprite.
  if (m_is_solid) {
    m_solid_time+=dt_sec;
    m_color.alpha = ALPHA_SOLID;
    set_action("solid");
    set_group(COLGROUP_STATIC);
  } else {
    m_color.alpha = ALPHA_NONSOLID;
    set_action("default");
    set_group(COLGROUP_DISABLED);
  }
}

void
MagicBlock::draw(DrawingContext& context)
{
  // Ask for update about lightmap at center of this block
  context.light().get_pixel(m_center, m_light);

  MovingSprite::draw(context);
  context.color().draw_filled_rect(m_col.m_bbox, m_color, m_layer);
}

bool
MagicBlock::collides(MovingObject& /*other*/, const CollisionHit& /*hit*/) const
{
  return m_is_solid;
}

HitResponse
MagicBlock::collision(MovingObject& /*other*/, const CollisionHit& /*hit*/)
{
  return FORCE_MOVE;
}

void
MagicBlock::on_flip(float height)
{
  MovingSprite::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
  m_center = m_col.m_bbox.get_middle();
}

/* EOF */
