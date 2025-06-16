//  SuperTux - Weak Block
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "object/weak_block.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "math/random.hpp"
#include "object/bullet.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/globals.hpp"
#include "supertux/sector.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

WeakBlock::WeakBlock(const ReaderMapping& mapping) :
  MovingSprite(mapping, "images/objects/weak_block/meltbox.sprite", LAYER_OBJECTS + 10, COLGROUP_STATIC),
  state(STATE_NORMAL),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-small.sprite"))
{
  // Older levels utilize hardcoded behaviour from the "linked" property.
  if (get_version() == 1)
  {
    bool linked = true;
    mapping.get("linked", linked);

    // The object was set to the "meltbox" (ICE) sprite, if it's not linked.
    // The default sprite was previously the HAY one, so we do the opposite check here.
    if (linked)
      m_type = HAY;

    on_type_change(TypeChange::INITIAL);
  }
  else
  {
    parse_type(mapping);
  }

  lightsprite->set_blend(Blend::ADD);
  lightsprite->set_color(Color(0.3f, 0.2f, 0.1f));

  if (m_type == HAY)
    SoundManager::current()->preload("sounds/fire.ogg"); // TODO: Use own sound?
  else
    SoundManager::current()->preload("sounds/sizzle.ogg");

  set_action("default");
}

void
WeakBlock::update_version()
{
  // Use ICE as default, when migrating from version 1.
  if (get_version() == 1)
  {
    m_type = ICE;
    on_type_change(m_type);
  }

  GameObject::update_version();
}

void
WeakBlock::save(Writer& writer)
{
  // If the version is 1 and the type is ICE, save "linked" as false.
  // Used to properly initialize the object as ICE when reading the saved level.
  if (get_version() == 1 && m_type == ICE)
    writer.write("linked", false);

  GameObject::save(writer);
}

GameObjectTypes
WeakBlock::get_types() const
{
  return {
    { "ice", _("Ice") },
    { "hay", _("Hay") }
  };
}

std::string
WeakBlock::get_default_sprite_name() const
{
  switch (m_type)
  {
    case HAY:
      return "images/objects/weak_block/strawbox.sprite";
    default:
      return m_default_sprite_name;
  }
}

HitResponse
WeakBlock::collision_bullet(Bullet& bullet, const CollisionHit& hit)
{
  switch (state) {

    case STATE_NORMAL:
      //Ensure only fire destroys weakblock
      if (bullet.get_type() == BONUS_FIRE) {
        startBurning();
        bullet.remove_me();
      }
      //Other bullets ricochet
      else {
        bullet.ricochet(*this, hit);
      }
    break;

    case STATE_BURNING:
    case STATE_DISINTEGRATING:
      break;

    default:
      log_debug << "unhandled state" << std::endl;
      break;
	}

	return FORCE_MOVE;
}

HitResponse
WeakBlock::collision(MovingObject& other, const CollisionHit& hit)
{
  switch (state) {

      case STATE_NORMAL:
        if (auto bullet = dynamic_cast<Bullet*> (&other)) {
          return collision_bullet(*bullet, hit);
        }
        break;

      case STATE_BURNING:
        if (m_type != HAY)
          break;

        if (auto badguy = dynamic_cast<BadGuy*> (&other)) {
          badguy->ignite();
        }
        break;
      case STATE_DISINTEGRATING:
        break;

      default:
        log_debug << "unhandled state" << std::endl;
        break;
  }

  return FORCE_MOVE;
}

void
WeakBlock::update(float )
{
  switch (state) {

      case STATE_NORMAL:
        break;

      case STATE_BURNING:
        // cause burn light to flicker randomly
        if (m_type == HAY) {
          if (graphicsRandom.rand(10) >= 7) {
            lightsprite->set_color(Color(0.2f + graphicsRandom.randf(20.0f) / 100.0f,
                                         0.1f + graphicsRandom.randf(20.0f)/100.0f,
                                         0.1f));
          } else
            lightsprite->set_color(Color(0.3f, 0.2f, 0.1f));
        }

        if (m_sprite->animation_done()) {
          state = STATE_DISINTEGRATING;
          set_action("disintegrating", 1);
          spreadHit();
          set_group(COLGROUP_DISABLED);
          lightsprite = SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-tiny.sprite");
          lightsprite->set_blend(Blend::ADD);
          lightsprite->set_color(Color(0.3f, 0.2f, 0.1f));
        }
        break;

      case STATE_DISINTEGRATING:
        if (m_sprite->animation_done()) {
          remove_me();
          return;
        }
        break;

  }
}

void
WeakBlock::draw(DrawingContext& context)
{
  MovingSprite::draw(context);
  if (m_type == HAY && (state != STATE_NORMAL))
  {
    lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), 0);
  }
}

void
WeakBlock::startBurning()
{
  if (state != STATE_NORMAL) return;
  state = STATE_BURNING;
  set_action("burning", 1);

  if (m_type == HAY)
    SoundManager::current()->play("sounds/fire.ogg", get_pos()); // TODO: Use own sound?
  else
    SoundManager::current()->play("sounds/sizzle.ogg", get_pos());
}

void
WeakBlock::spreadHit()
{
  //Destroy adjacent weakblocks if applicable
  if (m_type == HAY) {
    for (auto& wb : Sector::get().get_objects_by_type<WeakBlock>()) {
      if (&wb != this && wb.state == STATE_NORMAL)
      {
        const float dx = fabsf(wb.get_pos().x - m_col.m_bbox.get_left());
        const float dy = fabsf(wb.get_pos().y - m_col.m_bbox.get_top());
        if ((dx <= 32.5f) && (dy <= 32.5f)) {
          wb.startBurning();
        }
      }
    }
  }
}

void
WeakBlock::on_flip(float height)
{
  MovingSprite::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

std::vector<std::string>
WeakBlock::get_patches() const
{
  return { _("Sprites no longer define the behaviour of the object.\nObject types are used instead.") };
}

/* EOF */
