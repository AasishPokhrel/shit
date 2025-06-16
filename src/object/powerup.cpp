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

#include "object/powerup.hpp"

#include "audio/sound_manager.hpp"
#include "math/random.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/game_session.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

PowerUp::PowerUp(const ReaderMapping& mapping) :
  MovingSprite(mapping, "images/powerups/egg/egg.sprite", LAYER_OBJECTS, COLGROUP_MOVING),
  physic(),
  script(),
  no_physics(),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-small.sprite"))
{
  parse_type(mapping);
  mapping.get("script", script, "");
  mapping.get("disable-physics", no_physics, false);
  initialize();
}

PowerUp::PowerUp(const Vector& pos, int type) :
  MovingSprite(pos, "images/powerups/egg/egg.sprite", LAYER_OBJECTS, COLGROUP_MOVING),
  physic(),
  script(),
  no_physics(false),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-small.sprite"))
{
  m_type = type;
  on_type_change(TypeChange::INITIAL);

  update_version();
  initialize();
}

GameObjectTypes
PowerUp::get_types() const
{
  return {
    { "egg", _("Egg") },
    { "fire", _("Fire Flower") },
    { "ice", _("Ice Flower") },
    { "air", _("Air Flower") },
    { "earth", _("Earth Flower") },
    { "star", _("Star") },
    { "oneup", _("Tux Doll") },
    { "flip", _("Flip Potion") },
    { "mints", _("Mints") },
    { "coffee", _("Coffee") },
    { "herring", _("Herring") }
  };
}

std::string
PowerUp::get_default_sprite_name() const
{
  switch (m_type)
  {
    case FIRE:
      return "images/powerups/fireflower/fireflower.sprite";
    case ICE:
      return "images/powerups/iceflower/iceflower.sprite";
    case AIR:
      return "images/powerups/airflower/airflower.sprite";
    case EARTH:
      return "images/powerups/earthflower/earthflower.sprite";
    case STAR:
      return "images/powerups/star/star.sprite";
    case ONEUP:
      return "images/powerups/1up/1up.sprite";
    case FLIP:
      return "images/powerups/potions/red-potion.sprite";
    case MINTS:
      return "images/powerups/retro/mints.png";
    case COFFEE:
      return "images/powerups/retro/coffee.png";
    case HERRING:
      return "images/powerups/retro/golden_herring.png";
    default:
      return m_default_sprite_name;
  }
}

void
PowerUp::initialize()
{
  physic.enable_gravity(true);
  SoundManager::current()->preload("sounds/grow.ogg");
  SoundManager::current()->preload("sounds/fire-flower.wav");
  SoundManager::current()->preload("sounds/gulp.wav");

  // Older levels utilize hardcoded behaviour from the chosen sprite
  if (get_version() == 1)
  {
    if (matches_sprite("images/powerups/egg/egg.sprite"))
      m_type = EGG;
    else if (matches_sprite("images/powerups/fireflower/fireflower.sprite"))
      m_type = FIRE;
    else if (matches_sprite("images/powerups/iceflower/iceflower.sprite"))
      m_type = ICE;
    else if (matches_sprite("images/powerups/airflower/airflower.sprite"))
      m_type = AIR;
    else if (matches_sprite("images/powerups/earthflower/earthflower.sprite"))
      m_type = EARTH;
    else if (matches_sprite("images/powerups/star/star.sprite"))
      m_type = STAR;
    else if (matches_sprite("images/powerups/1up/1up.sprite"))
      m_type = ONEUP;
    else if (matches_sprite("images/powerups/potions/red-potion.sprite"))
      m_type = FLIP;
  }

  setup_lightsprite();
}

void
PowerUp::setup_lightsprite()
{
  lightsprite->set_blend(Blend::ADD);
  lightsprite->set_color(Color(0.0f, 0.0f, 0.0f));
  // Set default light for glow effect for default sprites.
  if (matches_sprite(get_default_sprite_name()))
  {
    switch (m_type)
    {
      case EGG:
        lightsprite->set_color(Color(0.2f, 0.2f, 0.0f));
        break;
      case FIRE:
        lightsprite->set_color(Color(0.3f, 0.0f, 0.0f));
        break;
      case ICE:
        lightsprite->set_color(Color(0.0f, 0.1f, 0.2f));
        break;
      case AIR:
        lightsprite->set_color(Color(0.15f, 0.0f, 0.15f));
        break;
      case EARTH:
        lightsprite->set_color(Color(0.0f, 0.3f, 0.0f));
        break;
      case STAR:
        lightsprite->set_color(Color(0.4f, 0.4f, 0.4f));
        break;
    }
  }
}

void
PowerUp::after_editor_set()
{
  MovingSprite::after_editor_set();
  setup_lightsprite();
}

void
PowerUp::collision_solid(const CollisionHit& hit)
{
  if (hit.bottom) {
    physic.set_velocity_y(0);
  }
  if (hit.right || hit.left) {
    physic.set_velocity_x(-physic.get_velocity_x());
  }
}

HitResponse
PowerUp::collision(MovingObject& other, const CollisionHit&)
{
  Player* player = dynamic_cast<Player*>(&other);
  if (!player)
    return FORCE_MOVE;

  if (m_type == FLIP)
    SoundManager::current()->play("sounds/gulp.wav", get_pos());

  if (!script.empty())
  {
    Sector::get().run_script(script, "powerup-script");
    remove_me();
    return ABORT_MOVE;
  }

  switch (m_type)
  {
    case EGG:
    case MINTS:
      if (!player->add_bonus(BONUS_GROWUP, true))
        return FORCE_MOVE;
      SoundManager::current()->play("sounds/grow.ogg", get_pos());
      break;
    case FIRE:
    case COFFEE:
      if (!player->add_bonus(BONUS_FIRE, true))
        return FORCE_MOVE;
      SoundManager::current()->play("sounds/fire-flower.wav", get_pos());
      break;
    case ICE:
      if (!player->add_bonus(BONUS_ICE, true))
        return FORCE_MOVE;
      SoundManager::current()->play("sounds/fire-flower.wav", get_pos());
      break;
    case AIR:
      if (!player->add_bonus(BONUS_AIR, true))
        return FORCE_MOVE;
      SoundManager::current()->play("sounds/fire-flower.wav", get_pos());
      break;
    case EARTH:
      if (!player->add_bonus(BONUS_EARTH, true))
        return FORCE_MOVE;
      SoundManager::current()->play("sounds/fire-flower.wav", get_pos());
      break;
    case STAR:
    case HERRING:
      player->make_invincible();
      break;
    case ONEUP:
      player->get_status().add_coins(100);
      break;
    case FLIP:
      FlipLevelTransformer flip_transformer;
      flip_transformer.transform(GameSession::current()->get_current_level());
      break;
  }

  remove_me();
  return ABORT_MOVE;
}

PowerUp::Type
PowerUp::get_type_from_bonustype(int type)
{
  switch (type)
  {
    case BONUS_GROWUP:
      return EGG;

    case BONUS_FIRE:
      return FIRE;

    case BONUS_ICE:
      return ICE;

    case BONUS_AIR:
      return AIR;

    case BONUS_EARTH:
      return EARTH;

    default:
      return FIRE;
  }
}

void
PowerUp::update(float dt_sec)
{
  if (!no_physics)
    m_col.set_movement(physic.get_movement(dt_sec));

  // Stars and herrings should sparkle when close to Tux.
  if (m_type == STAR || m_type == HERRING)
  {
    if (auto* player = Sector::get().get_nearest_player(m_col.m_bbox))
    {
      float disp_x = player->get_bbox().get_left() - m_col.m_bbox.get_left();
      float disp_y = player->get_bbox().get_top() - m_col.m_bbox.get_top();
      if (disp_x*disp_x + disp_y*disp_y <= 256*256)
      {
        if (graphicsRandom.rand(0, 2) == 0) {
          float px = graphicsRandom.randf(m_col.m_bbox.get_left() * 1.0f, m_col.m_bbox.get_right() * 1.0f);
          float py = graphicsRandom.randf(m_col.m_bbox.get_top() * 1.0f, m_col.m_bbox.get_bottom() * 1.0f);
          Vector ppos = Vector(px, py);
          Vector pspeed = Vector(0, 0);
          Vector paccel = Vector(0, 0);
          Sector::get().add<SpriteParticle>(
            "images/particles/sparkle.sprite",
            // draw bright sparkles when very close to Tux, dark sparkles when slightly further
            (disp_x*disp_x + disp_y*disp_y <= 128*128) ?
            // make every other a longer sparkle to make trail a bit fuzzy
            (size_t(g_game_time*20)%2) ? "small" : "medium" : "dark",
            ppos, ANCHOR_MIDDLE, pspeed, paccel, LAYER_OBJECTS+1+5);
        }
      }
    }
  }
}

void
PowerUp::draw(DrawingContext& context)
{
  m_sprite->draw(context.color(), get_pos(), m_layer, m_flip);

  // Stars and herrings are brighter.
  if (m_type == STAR || m_type == HERRING)
    m_sprite->draw(context.color(), get_pos(), m_layer, m_flip);

  lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), 0);
}

ObjectSettings
PowerUp::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_script(_("Script"), &script, "script");
  result.add_bool(_("Disable gravity"), &no_physics, "disable-physics", false);

  result.reorder({"script", "disable-physics", "sprite", "x", "y"});

  return result;
}

std::vector<std::string>
PowerUp::get_patches() const
{
  return { _("Sprites no longer define the behaviour of the object.\nObject types are used instead.") };
}

void
PowerUp::on_flip(float height)
{
  MovingSprite::on_flip(height);
  if (no_physics)
    FlipLevelTransformer::transform_flip(m_flip);
}

/* EOF */
