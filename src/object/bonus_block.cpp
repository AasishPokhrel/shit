//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "object/bonus_block.hpp"

#include <memory>

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "badguy/crusher.hpp"
#include "editor/editor.hpp"
#include "math/vector.hpp"
#include "object/bouncy_coin.hpp"
#include "object/coin_explode.hpp"
#include "object/coin_rain.hpp"
#include "object/flower.hpp"
#include "object/growup.hpp"
#include "object/oneup.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "object/powerup.hpp"
#include "object/rock.hpp"
#include "object/specialriser.hpp"
#include "object/star.hpp"
#include "object/trampoline.hpp"
#include "supertux/constants.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/level.hpp"
#include "supertux/sector.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

namespace {

std::unique_ptr<MovingObject> to_moving_object(std::unique_ptr<GameObject> object)
{
  if (dynamic_cast<MovingObject*>(object.get()) != nullptr) {
    return std::unique_ptr<MovingObject>(static_cast<MovingObject*>(object.release()));
  } else {
    return std::unique_ptr<MovingObject>();
  }
}

const float UPGRADE_SOUND_GAIN = 0.3f;

} // namespace

BonusBlock::BonusBlock(const Vector& pos, int tile_data) :
  Block(pos, "images/objects/bonus_block/bonusblock.sprite"),
  m_contents(),
  m_objects(),
  m_object(),
  m_hit_counter(1),
  m_script(),
  m_lightsprite(),
  m_coin_sprite(get_default_coin_sprite())
{
  set_action("default");
  m_contents = get_content_by_data(tile_data);
  preload_contents(tile_data);
}

BonusBlock::BonusBlock(const ReaderMapping& mapping) :
  Block(mapping, "images/objects/bonus_block/bonusblock.sprite"),
  m_contents(Content::COIN),
  m_objects(),
  m_object(),
  m_hit_counter(1),
  m_script(),
  m_lightsprite(),
  m_coin_sprite(get_default_coin_sprite())
{
  parse_type(mapping);
  mapping.get("count", m_hit_counter);
  mapping.get("script", m_script);
  mapping.get("coin-sprite", m_coin_sprite);

  int data = 0;
  if (mapping.get("data", data))
  {
    m_contents = get_content_by_data(data);
    preload_contents(data);
  }

  std::string content;
  if (mapping.get("contents", content))
  {
    m_contents = get_content_from_string(content);
    if (m_contents == Content::CUSTOM)
    {
      std::optional<ReaderCollection> content_collection;
      if (mapping.get("custom-contents", content_collection))
      {
        const auto& object_specs = content_collection->get_objects();
        if (!object_specs.empty())
        {
          if (object_specs.size() > 1)
            log_warning << "Only one custom object allowed inside bonus blocks, ignoring the rest." << std::endl;

          const auto& spec = object_specs.at(0);
          auto game_object = GameObjectFactory::instance().create(spec.get_name(), spec.get_mapping());

          if (dynamic_cast<MovingObject*>(game_object.get()))
            set_object(std::move(game_object));
          else
            log_warning << "Only `MovingObject`s are allowed inside bonus blocks." << std::endl;
        }
        else
        {
          log_warning << "Custom object not set." << std::endl;
        }
      }
      else
      {
        log_warning << "Custom object not set." << std::endl;
      }
    }
  }

  if (!Editor::is_active() && m_contents == Content::CUSTOM && !m_object)
    throw std::runtime_error("Need to specify content object for custom block");

  if (m_contents == Content::LIGHT || m_contents == Content::LIGHT_ON)
  {
    SoundManager::current()->preload("sounds/switch.ogg");
    m_lightsprite = Surface::from_file("/images/objects/lightmap_light/bonusblock_light.png");
    if (m_contents == Content::LIGHT_ON)
      set_action("on");
    else
      set_action("off");
  }
}

void
BonusBlock::add_object(std::unique_ptr<GameObject> object)
{
  if (!m_objects.empty())
    throw std::runtime_error(_("Only one custom object is allowed inside bonus blocks."));

  set_object(std::move(object));
}

void
BonusBlock::set_object(std::unique_ptr<GameObject> object)
{
  m_object = object.get();

  m_objects.clear();
  m_objects.push_back(std::move(object));
}

GameObjectTypes
BonusBlock::get_types() const
{
  return {
    { "blue", _("Blue") },
    { "orange", _("Orange") },
    { "purple", _("Purple") },
    { "retro", _("Retro") }
  };
}

std::string
BonusBlock::get_default_sprite_name() const
{
  switch (m_type)
  {
    case ORANGE:
      return "images/objects/bonus_block/orangeblock.sprite";
    case PURPLE:
      return "images/objects/bonus_block/purpleblock.sprite";
    case RETRO:
      return "images/objects/bonus_block/retroblock.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
BonusBlock::on_type_change(int old_type)
{
  Block::on_type_change(old_type);

  m_hit_counter = get_default_hit_counter();
  m_coin_sprite = get_default_coin_sprite();
}

int
BonusBlock::get_default_hit_counter() const
{
  switch (m_type)
  {
    case ORANGE:
      return 3;
    case PURPLE:
      return 5;
    default:
      return 1;
  }
}

std::string
BonusBlock::get_default_coin_sprite() const
{
  switch (m_type)
  {
    case RETRO:
      return "images/objects/coin/retro_coin.sprite";
    default:
      return "images/objects/coin/coin.sprite";
  }
}

BonusBlock::Content
BonusBlock::get_content_by_data(int tile_data) const
{
  // Warning: 'tile_data' can't be cast to 'Content', this manual
  // conversion is necessary.
  switch (tile_data) {
    case 1: return Content::COIN;
    case 2: return Content::FIREGROW;
    case 3: return Content::STAR;
    case 4: return Content::ONEUP;
    case 5: return Content::ICEGROW;
    case 6: return Content::LIGHT;
    case 7: return Content::TRAMPOLINE;
    case 8: return Content::PORTABLE_TRAMPOLINE; // Trampoline.
    case 9: return Content::ROCK; // Rock.
    case 10: return Content::RAIN;
    case 11: return Content::EXPLODE;
    case 12: return Content::POTION; // Red potion.
    case 13: return Content::AIRGROW;
    case 14: return Content::EARTHGROW;
    case 15: return Content::LIGHT_ON;
    case 16: return Content::RETROGROW;
    case 17: return Content::RETROSTAR;
    default:
      log_warning << "Invalid box contents" << std::endl;
      return Content::COIN;
  }
}

ObjectSettings
BonusBlock::get_settings()
{
  ObjectSettings result = Block::get_settings();

  result.add_script(_("Script"), &m_script, "script");
  result.add_int(_("Count"), &m_hit_counter, "count", get_default_hit_counter());
  result.add_enum(_("Content"), reinterpret_cast<int*>(&m_contents),
                  { _("Coin"), _("Growth (fire flower)"), _("Growth (ice flower)"), _("Growth (air flower)"),
                   _("Growth (earth flower)"), _("Growth (retro)"), _("Star"), _("Star (retro)"), _("Tux doll"), _("Custom"), _("Script"), _("Light"), _("Light (On)"),
                   _("Trampoline"), _("Portable trampoline"), _("Coin rain"), _("Coin explosion"), _("Rock"), _("Potion") },
                  { "coin", "firegrow", "icegrow", "airgrow", "earthgrow", "retrogrow", "star", "retrostar", "1up", "custom", "script", "light", "light-on",
                   "trampoline", "portabletrampoline", "rain", "explode", "rock", "potion" },
                  static_cast<int>(Content::COIN), "contents");

  if (m_contents == Content::CUSTOM)
    result.add_objects(_("Custom Content"), &m_objects, ObjectFactory::RegisteredObjectParam::OBJ_PARAM_DISPENSABLE,
                       [this](auto obj) { add_object(std::move(obj)); }, "custom-contents");
  else if (m_contents == Content::COIN || m_contents == Content::RAIN || m_contents == Content::EXPLODE)
    result.add_sprite(_("Coin sprite"), &m_coin_sprite, "coin-sprite", get_default_coin_sprite());

  result.reorder({"script", "count", "contents", "custom-content", "coin-sprite", "sprite", "x", "y"});

  return result;
}

int
BonusBlock::get_coins_worth() const
{
  return m_contents == BonusBlock::Content::COIN ? m_hit_counter :
         (m_contents == BonusBlock::Content::RAIN ||
          m_contents == BonusBlock::Content::EXPLODE) ? m_hit_counter * 10 : 0;
}


void
BonusBlock::hit(Player& player)
{
  try_open(&player);
}

HitResponse
BonusBlock::collision(MovingObject& other, const CollisionHit& hit_)
{
  auto player = dynamic_cast<Player*> (&other);
  if (player) {
    if (player->m_does_buttjump ||
      (player->is_swimboosting() && player->get_bbox().get_bottom() < m_col.m_bbox.get_top() + SHIFT_DELTA))
    {
      try_drop(player);
    }
  }

  auto badguy = dynamic_cast<BadGuy*> (&other);
  if (badguy) {
    // Hit contains no information for collisions with blocks.
    // Badguy's bottom has to be below the top of the block
    // SHIFT_DELTA is required to slide over one tile gaps.
    if ( badguy->can_break() && ( badguy->get_bbox().get_bottom() > m_col.m_bbox.get_top() + SHIFT_DELTA ) ) {
      try_open(player);
    }
  }

  auto crusher = dynamic_cast<Crusher*> (&other);
  if (crusher)
  {
    try_open(player);
  }

  auto portable = dynamic_cast<Portable*> (&other);
  if (portable && !badguy) {
    if (other.get_bbox().get_top() > m_col.m_bbox.get_bottom() - SHIFT_DELTA) {
      try_open(player);
    }
  }
  return Block::collision(other, hit_);
}

void
BonusBlock::try_open(Player* player)
{
  SoundManager::current()->play("sounds/brick.wav", get_pos());
  if (m_sprite->get_action() == "empty")
    return;

  if (player == nullptr)
    player = Sector::get().get_nearest_player(m_col.m_bbox);

  if (player == nullptr)
    return;

  Direction direction = (player->get_bbox().get_middle().x > m_col.m_bbox.get_middle().x) ? Direction::LEFT : Direction::RIGHT;

  bool play_upgrade_sound = false;
  switch (m_contents) {
    case Content::COIN:
    {
      Sector::get().add<BouncyCoin>(get_pos(), true, m_coin_sprite);
      SoundManager::current()->play("sounds/coin.wav", get_pos());
      player->get_status().add_coins(1, false);
      if (m_hit_counter != 0 && !m_parent_dispenser)
        Sector::get().get_level().m_stats.increment_coins();
      break;
    }

    case Content::FIREGROW:
    {
      raise_growup_bonus(player, BONUS_FIRE, direction);
      break;
    }

    case Content::ICEGROW:
    {
      raise_growup_bonus(player, BONUS_ICE, direction);
      break;
    }

    case Content::AIRGROW:
    {
      raise_growup_bonus(player, BONUS_AIR, direction);
      break;
    }

    case Content::EARTHGROW:
    {
      raise_growup_bonus(player, BONUS_EARTH, direction);
      break;
    }

    case Content::RETROGROW:
    {
      raise_growup_bonus(player, BONUS_FIRE, direction,
                         "images/powerups/retro/mints.png", "images/powerups/retro/coffee.png");
      break;
    }

    case Content::STAR:
    {
      Sector::get().add<Star>(get_pos() + Vector(0, -32), direction);
      play_upgrade_sound = true;
      break;
    }

    case Content::RETROSTAR:
    {
      Sector::get().add<Star>(get_pos() + Vector(0, -32), direction,
                              "images/powerups/retro/golden_herring.png");
      play_upgrade_sound = true;
      break;
    }

    case Content::ONEUP:
    {
      Sector::get().add<OneUp>(get_pos(), direction);
      play_upgrade_sound = true;
      break;
    }

    case Content::CUSTOM:
    {
      auto moving_obj_copy = to_moving_object(GameObjectFactory::instance().create(m_object->get_class_name(), get_pos() + Vector(0, -32),
                                                                                   direction, m_object->save()));
      Sector::get().add<SpecialRiser>(get_pos(), std::move(moving_obj_copy), true);
      play_upgrade_sound = true;
      break;
    }

    case Content::SCRIPT:
    { break; } // This prevents default contents from being assumed because scripts always run.

    case Content::LIGHT:
    case Content::LIGHT_ON:
    {
      if (m_sprite->get_action() == "on")
        set_action("off");
      else
        set_action("on");
      SoundManager::current()->play("sounds/switch.ogg", get_pos());
      break;
    }
    case Content::TRAMPOLINE:
    {
      Sector::get().add<SpecialRiser>(get_pos(), std::make_unique<Trampoline>(get_pos(), Trampoline::STATIONARY), true);
      play_upgrade_sound = true;
      break;
    }
    case Content::PORTABLE_TRAMPOLINE:
    {
      Sector::get().add<SpecialRiser>(get_pos(), std::make_unique<Trampoline>(get_pos(), Trampoline::PORTABLE), true);
      play_upgrade_sound = true;
      break;
    }
    case Content::ROCK:
    {
      Sector::get().add<SpecialRiser>(get_pos(), std::make_unique<Rock>(get_pos()));
      break;
    }
    case Content::POTION:
    {
      Sector::get().add<SpecialRiser>(get_pos(), std::make_unique<PowerUp>(get_pos(), PowerUp::FLIP));
      break;
    }
    case Content::RAIN:
    {
      Sector::get().add<CoinRain>(get_pos(), true, !m_parent_dispenser, m_coin_sprite);
      play_upgrade_sound = true;
      break;
    }
    case Content::EXPLODE:
    {
      Sector::get().add<CoinExplode>(get_pos() + Vector (0, -40), !m_parent_dispenser, m_coin_sprite);
      play_upgrade_sound = true;
      break;
    }
  }

  if (play_upgrade_sound)
    SoundManager::current()->play("sounds/upgrade.wav", get_pos(), UPGRADE_SOUND_GAIN);

  if (!m_script.empty()) { // Scripts always run if defined.
    Sector::get().run_script(m_script, "BonusBlockScript");
  }

  start_bounce(player);
  if (m_hit_counter <= 0 || m_contents == Content::LIGHT || m_contents == Content::LIGHT_ON) { // Use 0 to allow infinite hits.
  } else if (m_hit_counter == 1) {
    set_action("empty");
  } else {
    m_hit_counter--;
  }
}

void
BonusBlock::try_drop(Player *player)
{
  SoundManager::current()->play("sounds/brick.wav", get_pos());
  if (m_sprite->get_action() == "empty")
    return;

  // Determine the area below the bonus block. If it's solid, send it up regardless (except for dolls).
  Rectf dest_;
  dest_.set_left(m_col.m_bbox.get_left() + 1);
  dest_.set_top(m_col.m_bbox.get_bottom() + 1);
  dest_.set_right(m_col.m_bbox.get_right() - 1);
  dest_.set_bottom(dest_.get_top() + 30);

  if (!Sector::get().is_free_of_statics(dest_, this, true) && !(m_contents == Content::ONEUP))
  {
    try_open(player);
    return;
  }

  if (player == nullptr)
    player = Sector::get().get_nearest_player(m_col.m_bbox);

  if (player == nullptr)
    return;

  Direction direction = (player->get_bbox().get_middle().x > m_col.m_bbox.get_middle().x) ? Direction::LEFT : Direction::RIGHT;

  bool countdown = false;
  bool play_upgrade_sound = false;

  switch (m_contents) {
    case Content::COIN:
    {
      try_open(player);
      break;
    }

    case Content::FIREGROW:
    {
      drop_growup_bonus(player, PowerUp::FIRE, direction, countdown);
      break;
    }

    case Content::ICEGROW:
    {
      drop_growup_bonus(player, PowerUp::ICE, direction, countdown);
      break;
    }

    case Content::AIRGROW:
    {
      drop_growup_bonus(player, PowerUp::AIR, direction, countdown);
      break;
    }

    case Content::EARTHGROW:
    {
      drop_growup_bonus(player, PowerUp::EARTH, direction, countdown);
      break;
    }

    case Content::RETROGROW:
    {
      drop_growup_bonus(player, PowerUp::COFFEE, direction, countdown,
                        "images/powerups/retro/mints.png");
      break;
    }

    case Content::STAR:
    {
      Sector::get().add<Star>(get_pos() + Vector(0, 32), direction);
      play_upgrade_sound = true;
      countdown = true;
      break;
    }

    case Content::RETROSTAR:
    {
      Sector::get().add<Star>(get_pos() + Vector(0, 32), direction,
                              "images/powerups/retro/golden_herring.png");
      play_upgrade_sound = true;
      countdown = true;
      break;
    }

    case Content::ONEUP:
    {
      Sector::get().add<OneUp>(get_pos(), Direction::DOWN);
      play_upgrade_sound = true;
      countdown = true;
      break;
    }

    case Content::CUSTOM:
    {
      // NOTE: Non-portable trampolines could be moved to Content::CUSTOM, but they should not drop.

      auto obj_copy = GameObjectFactory::instance().create(m_object->get_class_name(), get_pos() + Vector(0, 32),
                                                           direction, m_object->save());
      Sector::get().add_object(std::move(obj_copy));
      play_upgrade_sound = true;
      countdown = true;
      break;
    }

    case Content::SCRIPT:
    {
      countdown = true;
      break;
    } // This prevents default contents from being assumed because scripts always run.

    case Content::LIGHT:
    case Content::LIGHT_ON:
    case Content::TRAMPOLINE:
    case Content::RAIN:
    {
      try_open(player);
      break;
    }
    case Content::ROCK:
    {
      Sector::get().add<Rock>(get_pos() + Vector(0, 32),  "images/objects/rock/rock.sprite");
      countdown = true;
      break;
    }
    case Content::PORTABLE_TRAMPOLINE:
    {
      Sector::get().add<Trampoline>(get_pos() + Vector(0, 32), Trampoline::PORTABLE);
      countdown = true;
      break;
    }
    case Content::POTION:
    {
      Sector::get().add<PowerUp>(get_pos() + Vector(0, 32), PowerUp::FLIP);
      countdown = true;
      break;
    }
    case Content::EXPLODE:
    {
      Sector::get().add<CoinExplode>(get_pos() + Vector (0, 40), !m_parent_dispenser, m_coin_sprite);
      play_upgrade_sound = true;
      countdown = true;
      break;
    }
  }

  if (play_upgrade_sound)
    SoundManager::current()->play("sounds/upgrade.wav", get_pos(), UPGRADE_SOUND_GAIN);

  if (!m_script.empty() && countdown) { // Scripts always run if defined.
    Sector::get().run_script(m_script, "powerup-script");
  }

  if (countdown) { // Only decrease the hit counter if try_open was not called.
    if (m_hit_counter == 1) {
      set_action("empty");
    } else {
      m_hit_counter--;
    }
  }
}

void
BonusBlock::raise_growup_bonus(Player* player, const BonusType& bonus, const Direction& dir,
                               const std::string& growup_sprite, const std::string& flower_sprite)
{
  std::unique_ptr<MovingObject> obj;
  if (player->get_status().bonus[player->get_id()] == BONUS_NONE)
  {
    obj = std::make_unique<GrowUp>(get_pos(), dir, growup_sprite);
  }
  else
  {
    obj = std::make_unique<Flower>(bonus, flower_sprite);
  }

  Sector::get().add<SpecialRiser>(get_pos(), std::move(obj));
  SoundManager::current()->play("sounds/upgrade.wav", get_pos(), UPGRADE_SOUND_GAIN);
}

void
BonusBlock::drop_growup_bonus(Player* player, int type, const Direction& dir, bool& countdown,
                              const std::string& growup_sprite)
{
  if (player->get_status().bonus[player->get_id()] == BONUS_NONE)
  {
    Sector::get().add<GrowUp>(get_pos() + Vector(0, 32), dir, growup_sprite);
  }
  else
  {
    Sector::get().add<PowerUp>(get_pos() + Vector(0, 32), type);
  }
  SoundManager::current()->play("sounds/upgrade.wav", get_pos(), UPGRADE_SOUND_GAIN);
  countdown = true;
}

void
BonusBlock::draw(DrawingContext& context)
{
  // Perform the regular drawing first.
  Block::draw(context);
  // Draw the light if the bonus block is in the "on" state.
  if (m_sprite->get_action() == "on")
  {
    Vector pos = get_pos() + (m_col.m_bbox.get_size().as_vector() - Vector(static_cast<float>(m_lightsprite->get_width()),
                                                                   static_cast<float>(m_lightsprite->get_height()))) / 2.0f;
    context.light().draw_surface(m_lightsprite, pos, 10);
  }
}

BonusBlock::Content
BonusBlock::get_content_from_string(const std::string& contentstring) const
{
  if (contentstring == "coin")
    return Content::COIN;
  else if (contentstring == "firegrow")
    return Content::FIREGROW;
  else if (contentstring == "icegrow")
    return Content::ICEGROW;
  else if (contentstring == "airgrow")
    return Content::AIRGROW;
  else if (contentstring == "earthgrow")
    return Content::EARTHGROW;
  else if (contentstring == "retrogrow")
    return Content::RETROGROW;
  else if (contentstring == "star")
    return Content::STAR;
  else if (contentstring == "retrostar")
    return Content::RETROSTAR;
  else if (contentstring == "1up")
    return Content::ONEUP;
  else if (contentstring == "custom")
    return Content::CUSTOM;
  else if (contentstring == "script") // Use this when the bonus block is intended to contain ONLY a script.
    return Content::SCRIPT;
  else if (contentstring == "light")
    return Content::LIGHT;
  else if (contentstring == "light-on")
    return Content::LIGHT_ON;
  else if (contentstring == "trampoline")
    return Content::TRAMPOLINE;
  else if (contentstring == "portabletrampoline")
    return Content::PORTABLE_TRAMPOLINE;
  else if (contentstring == "potion")
    return Content::POTION;
  else if (contentstring == "rock")
    return Content::ROCK;
  else if (contentstring == "rain")
    return Content::RAIN;
  else if (contentstring == "explode")
    return Content::EXPLODE;
  else
  {
    log_warning << "Invalid bonus block contents '" << contentstring << "'" << std::endl;
    return Content::COIN;
  }
}

void
BonusBlock::preload_contents(int d)
{
  switch (d)
  {
    case 6: // Light.
    case 15: // Light (On).
      SoundManager::current()->preload("sounds/switch.ogg");
      m_lightsprite=Surface::from_file("/images/objects/lightmap_light/bonusblock_light.png");
      break;

    case 7:
      // Uncomment the following line if this is to be moved to the "custom" case.
      // object = new Trampoline(get_pos(), false);
      break;

    case 8: // Trampoline.
      set_object(std::make_unique<Trampoline>(get_pos(), true));
      break;

    case 9: // Rock.
      set_object(std::make_unique<Rock>(get_pos(), "images/objects/rock/rock.sprite"));
      break;

    case 12: // Red potion.
      set_object(std::make_unique<PowerUp>(get_pos(), PowerUp::FLIP));
      break;

    default:
      break;
  }
}

/* EOF */
