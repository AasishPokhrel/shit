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

#ifndef HEADER_SUPERTUX_BADGUY_DISPENSER_HPP
#define HEADER_SUPERTUX_BADGUY_DISPENSER_HPP

#include "badguy/badguy.hpp"

class GameObject;

/**
 * @scripting
 * @summary A ""Dispenser"" that was given a name can be controlled by scripts.
 * @instances A ""Dispenser"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class Dispenser final : public BadGuy
{
public:
  static void register_class(ssq::VM& vm);

private:
  enum DispenserType {
    DROPPER, CANNON, POINT, GRANITO
  };

public:
  Dispenser(const ReaderMapping& reader);

  virtual void draw(DrawingContext& context) override;
  virtual void initialize() override;
  /**
   * @scripting
   * @description Makes the dispenser start dispensing badguys.
   */
  virtual void activate() override;
  /**
   * @scripting
   * @description Stops the dispenser from dispensing badguys.
   */
  virtual void deactivate() override;
  virtual void active_update(float dt_sec) override;

  virtual void kill_fall() override;
  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;
  virtual bool is_freezable() const override;
  virtual bool is_flammable() const override;
  virtual bool always_active() const override { return true; }
  virtual bool is_portable() const override;

  static std::string class_name() { return "dispenser"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Dispenser"; }
  static std::string display_name() { return _("Dispenser"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Dispenser)); }

  virtual ObjectSettings get_settings() override;
  virtual GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

  virtual void on_flip(float height) override;

  virtual void after_editor_set() override;

  void notify_dead() {
    if (m_limit_dispensed_badguys) {
      m_current_badguys--;
    }
  }

protected:
  void add_object(std::unique_ptr<GameObject> object);

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  void launch_object();

  void on_type_change(int old_type) override;

private:
  void set_correct_action();
  void set_correct_colgroup();

private:
  float m_cycle;
  std::vector<std::unique_ptr<GameObject>> m_objects;
  unsigned int m_next_object;
  Timer m_dispense_timer;
  bool m_autotarget;
  bool m_random;
  bool m_gravity;

  /** Do we need to limit the number of dispensed badguys? */
  bool m_limit_dispensed_badguys;

  /** Maximum concurrent number of badguys to be dispensed */
  int m_max_concurrent_badguys;

  /** Current amount of spawned badguys */
  int m_current_badguys;

private:
  Dispenser(const Dispenser&) = delete;
  Dispenser& operator=(const Dispenser&) = delete;
};

#endif

/* EOF */
