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

#ifndef HEADER_SUPERTUX_TRIGGER_TRIGGER_BASE_HPP
#define HEADER_SUPERTUX_TRIGGER_TRIGGER_BASE_HPP

#include "object/sticky_object.hpp"
#include "supertux/moving_object.hpp"
#include "supertux/object_remove_listener.hpp"

#include <vector>

class Player;

/** This class is the base class for all objects you can interact with
    in some way. There are several interaction types defined like
    touch and activate */
class TriggerBase : public ObjectRemoveListener
{
public:
  enum EventType {
    EVENT_TOUCH,     /**< Object came into contact */
    EVENT_LOSETOUCH, /**< Lost contact with object */
    EVENT_ACTIVATE   /**< Action button pressed    */
  };

public:
  TriggerBase();
  ~TriggerBase() override;

  /** Receive trigger events */
  virtual void event(Player& player, EventType type) = 0;

  /** Called by GameObject destructor of an object in losetouch_listeners */
  virtual void object_removed(GameObject* object) override;

protected:
  void update();
  HitResponse collision(MovingObject& other, const CollisionHit& hit);

private:
  std::vector<Player*> m_hit;

  /** Players that will be informed when we lose touch with them */
  std::vector<Player*> m_losetouch_listeners;

private:
  TriggerBase(const TriggerBase&) = delete;
  TriggerBase& operator=(const TriggerBase&) = delete;
};


class Trigger : public MovingObject,
                public TriggerBase
{
public:
  Trigger(const ReaderMapping& reader);
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(TriggerBase)).add(typeid(Trigger)); }

  virtual void update(float) override
  {
    TriggerBase::update();
  }
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override
  {
    return TriggerBase::collision(other, hit);
  }

  int get_layer() const override { return LAYER_TILES + 1; }

private:
  Trigger(const Trigger&) = delete;
  Trigger& operator=(const Trigger&) = delete;
};


class SpritedTrigger : public MovingSprite,
                       public TriggerBase
{
public:
  SpritedTrigger(const ReaderMapping& reader,
                 const std::string& sprite_name,
                 int layer = LAYER_TILES + 1);

  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(TriggerBase)).add(typeid(SpritedTrigger)); }

  virtual void update(float) override
  {
    TriggerBase::update();
  }
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override
  {
    return TriggerBase::collision(other, hit);
  }

private:
  SpritedTrigger(const SpritedTrigger&) = delete;
  SpritedTrigger& operator=(const SpritedTrigger&) = delete;
};


class StickyTrigger : public StickyObject,
                      public TriggerBase
{
public:
  StickyTrigger(const ReaderMapping& reader,
                const std::string& sprite_name,
                int layer = LAYER_TILES + 1);

  virtual GameObjectClasses get_class_types() const override { return StickyObject::get_class_types().add(typeid(TriggerBase)).add(typeid(StickyTrigger)); }

  virtual void update(float dt_sec) override
  {
    StickyObject::update(dt_sec);
    TriggerBase::update();
  }
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override
  {
    return TriggerBase::collision(other, hit);
  }

private:
  StickyTrigger(const StickyTrigger&) = delete;
  StickyTrigger& operator=(const StickyTrigger&) = delete;
};

#endif

/* EOF */
