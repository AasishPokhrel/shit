//  SuperTux
//  Copyright (C) 2023 MatusGuy
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

#ifndef HEADER_SUPERTUX_BADGUY_GRANITO_BIG_HPP
#define HEADER_SUPERTUX_BADGUY_GRANITO_BIG_HPP

#include "badguy/granito.hpp"

/**
 * @scripting
 * @summary A ""GranitoBig"" that was given a name can be controlled by scripts.
 *          It has some features from its base class, ""Granito"", disabled.
 * @instances A ""GranitoBig"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class GranitoBig final : public Granito
{
public:
  static void register_class(ssq::VM& vm);

public:
  GranitoBig(const ReaderMapping& reader);

  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;

  virtual void active_update(float dt_sec) override;

  static std::string class_name() { return "granito_big"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "GranitoBig"; }
  static std::string display_name() { return _("Big Granito"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Granito::get_class_types().add(typeid(GranitoBig)); }

  virtual ObjectSettings get_settings() override;
  virtual GameObjectTypes get_types() const override;

  void carry(Granito* granito);
  inline Granito* get_carrying() const { return m_carrying; }

  /**
   * @scripting
   * @description This function tells the Granito being carried to
   *              unglue itself from the Big Granito, by jumping and walking off.
   */
  virtual void eject() override;

  /**
   * @scripting
   * @description Gets the name of the Granito being carried by the Big Granito.
   */
  std::string get_carrying_name() const;

  // The following functions are unimplemented for Big Granito.
  virtual void wave() override {}
  virtual void jump() override {}
  virtual void sit() override {}
  virtual GranitoBig* get_carrier() const override { return nullptr; }

public:
  Granito* m_carrying;

protected:
  bool try_wave() override;
  bool try_jump() override;

private:
  GranitoBig(const GranitoBig&) = delete;
  GranitoBig& operator=(const GranitoBig&) = delete;
};

#endif

/* EOF */
