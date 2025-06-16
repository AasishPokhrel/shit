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

#ifndef HEADER_SUPERTUX_OBJECT_PARTICLESYSTEM_INTERACTIVE_HPP
#define HEADER_SUPERTUX_OBJECT_PARTICLESYSTEM_INTERACTIVE_HPP

#include "object/particlesystem.hpp"

#include "math/fwd.hpp"

/**
   This is an alternative class for particle systems. It is
   responsible for storing a set of particles with each having an x-
   and y-coordinate the number of the layer where it should be drawn
   and a texture.

   This version of the particle system class doesn't use virtual
   screen coordinates, but Interactive ones. Particle systems which
   need Interactive levels coordinates, such as rain, should be
   implemented here.

   Classes that implement a particle system should subclass from this
   class, initialize particles in the constructor and move them in the
   simulate function.
*/
class ParticleSystem_Interactive : public ParticleSystem
{
public:
  ParticleSystem_Interactive();
  ParticleSystem_Interactive(const ReaderMapping& mapping);
  ~ParticleSystem_Interactive() override;

  virtual void draw(DrawingContext& context) override;
  virtual std::string get_display_name() const override {
    return _("Interactive particle system");
  }
  virtual GameObjectClasses get_class_types() const override { return ParticleSystem::get_class_types().add(typeid(ParticleSystem_Interactive)); }

protected:
  virtual int collision(Particle* particle, const Vector& movement);

private:
  ParticleSystem_Interactive(const ParticleSystem_Interactive&) = delete;
  ParticleSystem_Interactive& operator=(const ParticleSystem_Interactive&) = delete;
};

#endif

/* EOF */
