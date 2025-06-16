//  SuperTux
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

#include "supertux/game_object_factory.hpp"

#include <sstream>

#include "audio/sound_source.hpp"
#include "badguy/angrystone.hpp"
#include "badguy/bouncing_snowball.hpp"
#include "badguy/captainsnowball.hpp"
#include "badguy/corrupted_granito.hpp"
#include "badguy/corrupted_granito_big.hpp"
#include "badguy/crusher.hpp"
#include "badguy/crystallo.hpp"
#include "badguy/dart.hpp"
#include "badguy/darttrap.hpp"
#include "badguy/dispenser.hpp"
#include "badguy/dive_mine.hpp"
#include "badguy/fish_chasing.hpp"
#include "badguy/fish_harmless.hpp"
#include "badguy/fish_jumping.hpp"
#include "badguy/fish_swimming.hpp"
#include "badguy/flame.hpp"
#include "badguy/flyingsnowball.hpp"
#include "badguy/ghosttree.hpp"
#include "badguy/ghoul.hpp"
#include "badguy/goldbomb.hpp"
#include "badguy/granito.hpp"
#include "badguy/granito_big.hpp"
#include "badguy/granito_giant.hpp"
#include "badguy/haywire.hpp"
#include "badguy/igel.hpp"
#include "badguy/jumpy.hpp"
#include "badguy/kamikazesnowball.hpp"
#include "badguy/kugelblitz.hpp"
#include "badguy/livefire.hpp"
#include "badguy/mole.hpp"
#include "badguy/mole_rock.hpp"
#include "badguy/mrbomb.hpp"
#include "badguy/mrtree.hpp"
#include "badguy/owl.hpp"
#include "badguy/plant.hpp"
#include "badguy/rcrystallo.hpp"
#include "badguy/root.hpp"
#include "badguy/root_sapling.hpp"
#include "badguy/short_fuse.hpp"
#include "badguy/skydive.hpp"
#include "badguy/smartball.hpp"
#include "badguy/smartblock.hpp"
#include "badguy/snail.hpp"
#include "badguy/snowball.hpp"
#include "badguy/snowman.hpp"
#include "badguy/scrystallo.hpp"
#include "badguy/spiky.hpp"
#include "badguy/sspiky.hpp"
#include "badguy/stalactite.hpp"
#include "badguy/stumpy.hpp"
#include "badguy/tarantula.hpp"
#include "badguy/toad.hpp"
#include "badguy/totem.hpp"
#include "badguy/viciousivy.hpp"
#include "badguy/walking_candle.hpp"
#include "badguy/walkingleaf.hpp"
#include "badguy/willowisp.hpp"
#include "badguy/yeti.hpp"
#include "badguy/yeti_stalactite.hpp"
#include "badguy/zeekling.hpp"
#include "editor/editor_comment.hpp"
#include "math/vector.hpp"
#include "object/ambient_light.hpp"
#include "object/ambient_sound.hpp"
#include "object/background.hpp"
#include "object/bicycle_platform.hpp"
#include "object/bigsnowball.hpp"
#include "object/bonus_block.hpp"
#include "object/brick.hpp"
#include "object/bumper.hpp"
#include "object/camera.hpp"
#include "object/candle.hpp"
#include "object/circleplatform.hpp"
#include "object/cloud_particle_system.hpp"
#include "object/custom_particle_system.hpp"
#include "object/custom_particle_system_file.hpp"
#include "object/coin.hpp"
#include "object/conveyor_belt.hpp"
#include "object/decal.hpp"
#include "object/display_effect.hpp"
#include "object/explosion.hpp"
#include "object/fallblock.hpp"
#include "object/firefly.hpp"
#include "object/floating_image.hpp"
#include "object/ghost_particle_system.hpp"
#include "object/gradient.hpp"
#include "object/hurting_platform.hpp"
#include "object/infoblock.hpp"
#include "object/invisible_block.hpp"
#include "object/invisible_wall.hpp"
#include "object/ispy.hpp"
#include "object/key.hpp"
#include "object/lantern.hpp"
#include "object/level_time.hpp"
#include "object/lit_object.hpp"
#include "object/magicblock.hpp"
#include "object/path.hpp"
#include "object/path_gameobject.hpp"
#include "object/particle_zone.hpp"
#include "object/platform.hpp"
#include "object/pneumatic_platform.hpp"
#include "object/powerup.hpp"
#include "object/pushbutton.hpp"
#include "object/rain_particle_system.hpp"
#include "object/rublight.hpp"
#include "object/rusty_trampoline.hpp"
#include "object/scripted_object.hpp"
#include "object/shard.hpp"
#include "object/snow_particle_system.hpp"
#include "object/sound_object.hpp"
#include "object/spawnpoint.hpp"
#include "object/spotlight.hpp"
#include "object/text_array_object.hpp"
#include "object/textscroller.hpp"
#include "object/thunderstorm.hpp"
#include "object/tilemap.hpp"
#include "object/torch.hpp"
#include "object/trampoline.hpp"
#include "object/unstable_tile.hpp"
#include "object/weak_block.hpp"
#include "object/wind.hpp"
#include "supertux/level.hpp"
#include "supertux/tile_manager.hpp"
#include "trigger/climbable.hpp"
#include "trigger/door.hpp"
#include "trigger/scripttrigger.hpp"
#include "trigger/secretarea_trigger.hpp"
#include "trigger/sequence_trigger.hpp"
#include "trigger/switch.hpp"
#include "trigger/text_area.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "worldmap/level_tile.hpp"
#include "worldmap/spawn_point.hpp"
#include "worldmap/special_tile.hpp"
#include "worldmap/sprite_change.hpp"
#include "worldmap/teleporter.hpp"

GameObjectFactory&
GameObjectFactory::instance()
{
  static GameObjectFactory instance_;
  return instance_;
}

GameObjectFactory::GameObjectFactory()
{
  init_factories();
}

void
GameObjectFactory::init_factories()
{
  // Badguys.
  m_adding_badguys = true;
  add_factory<AngryStone>("angrystone");
  add_factory<BouncingSnowball>("bouncingsnowball", OBJ_PARAM_DISPENSABLE);
  add_factory<CaptainSnowball>("captainsnowball", OBJ_PARAM_DISPENSABLE);
  add_type_factory<CorruptedGranito>("skullyhop", CorruptedGranito::SKULLYHOP); // Backward compatibility
  add_factory<CorruptedGranito>("corrupted_granito", OBJ_PARAM_DISPENSABLE);
  add_factory<CorruptedGranitoBig>("corrupted_granito_big", OBJ_PARAM_DISPENSABLE);
  add_factory<Crusher>("icecrusher"); // Backward compatibility
  add_factory<Crusher>("crusher");
  add_factory<Crystallo>("crystallo", OBJ_PARAM_DISPENSABLE);
  add_factory<Dart>("dart", OBJ_PARAM_DISPENSABLE);
  add_factory<DartTrap>("darttrap");
  add_factory<Dispenser>("dispenser", OBJ_PARAM_DISPENSABLE);
  add_factory<DiveMine>("dive-mine", OBJ_PARAM_DISPENSABLE);
  add_factory<FishChasing>("fish-chasing", OBJ_PARAM_DISPENSABLE);
  add_factory<FishHarmless>("fish-harmless", OBJ_PARAM_DISPENSABLE);
  add_factory<FishJumping>("fish"); // Backward compatibility
  add_factory<FishJumping>("fish-jumping", OBJ_PARAM_DISPENSABLE);
  add_factory<FishSwimming>("fish-swimming", OBJ_PARAM_DISPENSABLE);
  add_factory<Flame>("flame", OBJ_PARAM_DISPENSABLE);
  add_factory<FlyingSnowBall>("flyingsnowball", OBJ_PARAM_DISPENSABLE);
  add_type_factory<Flame>("ghostflame", Flame::GHOST); // Backward compatibility.
  add_factory<GhostTree>("ghosttree");
  add_factory<Ghoul>("ghoul", OBJ_PARAM_DISPENSABLE);
  add_factory<GoldBomb>("goldbomb", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<Granito>("granito", OBJ_PARAM_DISPENSABLE);
  add_factory<GranitoBig>("granito_big", OBJ_PARAM_DISPENSABLE);
  add_factory<GranitoGiant>("granito_giant", OBJ_PARAM_DISPENSABLE);
  add_factory<Haywire>("haywire", OBJ_PARAM_DISPENSABLE);
  add_type_factory<Flame>("iceflame", Flame::ICE); // Backward compatibility.
  add_factory<Igel>("igel", OBJ_PARAM_DISPENSABLE);
  add_factory<Ispy>("ispy");
  add_factory<Jumpy>("jumpy", OBJ_PARAM_DISPENSABLE);
  add_factory<KamikazeSnowball>("kamikazesnowball", OBJ_PARAM_DISPENSABLE);
  add_factory<Kugelblitz>("kugelblitz", OBJ_PARAM_DISPENSABLE);
  add_factory<LeafShot>("leafshot", OBJ_PARAM_DISPENSABLE);
  add_factory<LiveFire>("livefire", OBJ_PARAM_DISPENSABLE);
  add_factory<LiveFireAsleep>("livefire_asleep", OBJ_PARAM_DISPENSABLE);
  add_factory<LiveFireDormant>("livefire_dormant", OBJ_PARAM_DISPENSABLE);
  add_factory<Mole>("mole");
  add_factory<MoleRock>("mole_rock", OBJ_PARAM_DISPENSABLE);
  add_factory<MrBomb>("mrbomb", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<MrIceBlock>("mriceblock", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<MrTree>("mrtree", OBJ_PARAM_DISPENSABLE);
  add_factory<Owl>("owl", OBJ_PARAM_DISPENSABLE);
  add_factory<Plant>("plant", OBJ_PARAM_DISPENSABLE);
  add_factory<RCrystallo>("rcrystallo", OBJ_PARAM_DISPENSABLE);
  add_factory<Root>("root");
  add_factory<RootSapling>("root_sapling");
  add_factory<SCrystallo>("scrystallo", OBJ_PARAM_DISPENSABLE);
  add_factory<ShortFuse>("short_fuse", OBJ_PARAM_DISPENSABLE);
  add_factory<SSpiky>("sspiky", OBJ_PARAM_DISPENSABLE);
  add_factory<SkyDive>("skydive", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<SmartBall>("smartball", OBJ_PARAM_DISPENSABLE);
  add_factory<SmartBlock>("smartblock", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<Snail>("snail", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<SnowBall>("snowball", OBJ_PARAM_DISPENSABLE);
  add_factory<Snowman>("snowman", OBJ_PARAM_DISPENSABLE);
  add_factory<Spiky>("spiky", OBJ_PARAM_DISPENSABLE);
  add_factory<Stalactite>("stalactite", OBJ_PARAM_DISPENSABLE);
  add_factory<Stumpy>("stumpy", OBJ_PARAM_DISPENSABLE);
  add_factory<Tarantula>("spidermite"); // Backward compatibilty.
  add_factory<Tarantula>("tarantula", OBJ_PARAM_DISPENSABLE);
  add_factory<Toad>("toad", OBJ_PARAM_DISPENSABLE);
  add_factory<Totem>("totem", OBJ_PARAM_DISPENSABLE);
  add_factory<ViciousIvy>("poisonivy"); // Backward compatibilty.
  add_factory<ViciousIvy>("viciousivy", OBJ_PARAM_DISPENSABLE);
  add_factory<WalkingCandle>("walking_candle", OBJ_PARAM_DISPENSABLE);
  add_factory<WalkingLeaf>("walkingleaf", OBJ_PARAM_DISPENSABLE);
  add_factory<WillOWisp>("willowisp", OBJ_PARAM_DISPENSABLE);
  add_factory<Yeti>("yeti");
  add_factory<YetiStalactite>("yeti_stalactite");
  add_factory<Zeekling>("zeekling", OBJ_PARAM_DISPENSABLE);
  m_adding_badguys = false;

  // Other objects.
  add_factory<AmbientLight>("ambient-light");
  add_factory<AmbientSound>("ambient_sound"); // Backward compatibilty.
  add_factory<AmbientSound>("ambient-sound");
  add_factory<Background>("background", OBJ_PARAM_WORLDMAP);
  add_factory<BigSnowball>("bigsnowball", OBJ_PARAM_DISPENSABLE);
  add_factory<PathGameObject>("path");
  add_factory<BicyclePlatform>("bicycle-platform");
  add_factory<BonusBlock>("bonusblock", OBJ_PARAM_DISPENSABLE);
  add_factory<Brick>("brick", OBJ_PARAM_DISPENSABLE);
  add_factory<Bumper>("bumper");
  add_factory<Camera>("camera");
  add_factory<Candle>("candle");
  add_factory<CirclePlatform>("circleplatform");
  add_factory<CloudParticleSystem>("particles-clouds");
  add_factory<ConveyorBelt>("conveyor-belt");
  add_factory<CustomParticleSystem>("particles-custom");
  add_factory<CustomParticleSystemFile>("particles-custom-file");
  add_factory<Coin>("coin", OBJ_PARAM_DISPENSABLE);
  add_factory<Decal>("decal", OBJ_PARAM_WORLDMAP);
  add_factory<Explosion>("explosion", OBJ_PARAM_DISPENSABLE);
  add_factory<FallBlock>("fallblock", OBJ_PARAM_DISPENSABLE);
  add_factory<Firefly>("firefly");
  add_factory<GhostParticleSystem>("particles-ghosts");
  add_factory<Gradient>("gradient");
  add_factory<HeavyBrick>("heavy-brick", OBJ_PARAM_DISPENSABLE);
  add_factory<HeavyCoin>("heavycoin");
  add_factory<HurtingPlatform>("hurting_platform");
  add_factory<InfoBlock>("infoblock", OBJ_PARAM_DISPENSABLE);
  add_factory<InvisibleBlock>("invisible_block", OBJ_PARAM_DISPENSABLE);
  add_factory<InvisibleWall>("invisible_wall");
  add_factory<Key>("key");
  add_factory<Lantern>("lantern", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<LevelTime>("leveltime");
  add_factory<LitObject>("lit-object");
  add_factory<MagicBlock>("magicblock");
  add_factory<ParticleZone>("particle-zone");
  add_factory<Platform>("platform");
  add_factory<PneumaticPlatform>("pneumatic-platform");
  add_factory<PowerUp>("powerup", OBJ_PARAM_DISPENSABLE);
  add_factory<PushButton>("pushbutton");
  add_factory<RainParticleSystem>("particles-rain");
  add_factory<Rock>("rock", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<RubLight>("rublight", OBJ_PARAM_DISPENSABLE);
  add_factory<ScriptedObject>("scriptedobject");
  add_factory<Shard>("shard", OBJ_PARAM_DISPENSABLE);
  add_type_factory<UnstableTile>("skull_tile", UnstableTile::DELAYED); // Backward compatibility.
  add_factory<SoundObject>("sound-object");
  add_factory<SnowParticleSystem>("particles-snow");
  add_factory<Spotlight>("spotlight");
  add_factory<TextScroller>("textscroller");
  add_factory<Thunderstorm>("thunderstorm");
  add_factory<Torch>("torch");
  add_factory<Trampoline>("trampoline", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<RustyTrampoline>("rustytrampoline", OBJ_PARAM_PORTABLE | OBJ_PARAM_DISPENSABLE);
  add_factory<UnstableTile>("unstable_tile");
  add_factory<WeakBlock>("weak_block", OBJ_PARAM_DISPENSABLE);
  add_factory<Wind>("wind");
  add_factory<TextArea>("text-area");

  // Triggers.
  add_factory<Climbable>("climbable");
  add_factory<Door>("door");
  add_factory<ScriptTrigger>("scripttrigger");
  add_factory<SecretAreaTrigger>("secretarea");
  add_factory<SequenceTrigger>("sequencetrigger");
  add_factory<Switch>("switch");

  // Editor stuff.
  add_factory<SpawnPointMarker>("spawnpoint");
  add_factory<EditorComment>("editor-comment");

  // Worldmap objects.
  add_factory<worldmap::LevelTile>("level", OBJ_PARAM_WORLDMAP);
  add_factory<worldmap::SpecialTile>("special-tile", OBJ_PARAM_WORLDMAP);
  add_factory<worldmap::SpriteChange>("sprite-change", OBJ_PARAM_WORLDMAP);
  add_factory<worldmap::Teleporter>("teleporter", OBJ_PARAM_WORLDMAP);
  add_factory<worldmap::SpawnPointObject>("worldmap-spawnpoint");

  add_factory("tilemap", {
    [](const ReaderMapping& reader) {
      auto tileset = TileManager::current()->get_tileset(Level::current()->get_tileset());
      return std::make_unique<TileMap>(tileset, reader);
    },
    TileMap::display_name
  });
}

/** Register all scriptable objects to a Squirrel VM. */
void
GameObjectFactory::register_objects(ssq::VM& vm)
{
  /* Base classes */
  GameObject::register_class(vm);
  MovingObject::register_class(vm);
  MovingSprite::register_class(vm);
  BadGuy::register_class(vm);
  ParticleSystem::register_class(vm);

  AmbientSound::register_class(vm);
  Background::register_class(vm);
  Camera::register_class(vm);
  Candle::register_class(vm);
  CloudParticleSystem::register_class(vm);
  ConveyorBelt::register_class(vm);
  CustomParticleSystem::register_class(vm);
  Decal::register_class(vm);
  Dispenser::register_class(vm);
  DisplayEffect::register_class(vm);
  FloatingImage::register_class(vm);
  Gradient::register_class(vm);
  LevelTime::register_class(vm);
  LitObject::register_class(vm);
  Platform::register_class(vm);
  Player::register_class(vm);
  RainParticleSystem::register_class(vm);
  ScriptedObject::register_class(vm);
  SoundObject::register_class(vm);
  Spotlight::register_class(vm);
  TextArrayObject::register_class(vm);
  TextObject::register_class(vm);
  Thunderstorm::register_class(vm);
  TileMap::register_class(vm);
  Torch::register_class(vm);
  WillOWisp::register_class(vm);
  Wind::register_class(vm);
  Granito::register_class(vm);
  GranitoBig::register_class(vm);
}

std::unique_ptr<GameObject>
GameObjectFactory::create(const std::string& name, const std::string& data) const
{
  std::stringstream lisptext;
  lisptext << "(" << name << "\n" << data << ")";

  auto doc = ReaderDocument::from_stream(lisptext);
  return create(name, doc.get_root().get_mapping());
}

std::unique_ptr<GameObject>
GameObjectFactory::create(const std::string& name, const Vector& pos, const Direction& dir, const std::string& data) const
{
  std::stringstream lisptext;
  lisptext << "(" << name << "\n"
           << " (x " << pos.x << ")"
           << " (y " << pos.y << ")" << data;
  if (dir != Direction::AUTO) {
    lisptext << " (direction \"" << dir << "\"))";
  } else {
    lisptext << ")";
  }

  auto doc = ReaderDocument::from_stream(lisptext);
  return create(name, doc.get_root().get_mapping());
}

/* EOF */
