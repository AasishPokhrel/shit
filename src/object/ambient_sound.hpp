//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023 mrkubax10 <mrkubax10@onet.pl>
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

#ifndef HEADER_SUPERTUX_OBJECT_AMBIENT_SOUND_HPP
#define HEADER_SUPERTUX_OBJECT_AMBIENT_SOUND_HPP

#include "math/vector.hpp"
#include "supertux/moving_object.hpp"
#include "video/layer.hpp"

class GameObject;
class ReaderMapping;
class SoundSource;

/**
 * @scripting
 * @summary An ""AmbientSound"" that was given a name can be controlled by scripts.
 * @instances An ""AmbientSound"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class AmbientSound final : public MovingObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  AmbientSound(const ReaderMapping& mapping);
  AmbientSound(const Vector& pos, float radius, float vol, const std::string& file);
  ~AmbientSound() override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit_) override;

  static std::string class_name() { return "ambient-sound"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "AmbientSound"; }
  static std::string display_name() { return _("Ambient Sound"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual bool has_variable_size() const override { return true; }
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(AmbientSound)); }

  virtual void draw(DrawingContext& context) override;

  virtual ObjectSettings get_settings() override;

  virtual int get_layer() const override { return LAYER_OBJECTS; }

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @deprecated Use ""get_x()"" instead!
   * @description Returns the ambient sound's X coordinate.
   */
  float get_pos_x() const;
  /**
   * @scripting
   * @deprecated Use ""get_y()"" instead!
   * @description Returns the ambient sound's Y coordinate.
   */
  float get_pos_y() const;
#endif

protected:
  virtual void update(float dt_sec) override;

private:
  void prepare_sound_source();

private:
  std::string m_sample;
  std::unique_ptr<SoundSource> m_sound_source;

  float m_radius;
  float m_radius_in_px;
  float m_volume;
  bool m_has_played_sound;

private:
  AmbientSound(const AmbientSound&) = delete;
  AmbientSound& operator=(const AmbientSound&) = delete;
};

#endif

/* EOF */
