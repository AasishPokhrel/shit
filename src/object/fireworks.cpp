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

#include "object/fireworks.hpp"

#include "audio/sound_manager.hpp"
#include "math/random.hpp"
#include "object/camera.hpp"
#include "object/particles.hpp"
#include "supertux/sector.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

Fireworks::Fireworks() :
  timer()
{
  timer.start(.2f);
  SoundManager::current()->preload("sounds/fireworks.wav");
}

void
Fireworks::update(float )
{
  if (timer.check()) {
    Vector pos = Sector::get().get_camera().get_translation();
    pos += Vector(graphicsRandom.randf(static_cast<float>(SCREEN_WIDTH)),
                  graphicsRandom.randf(static_cast<float>(SCREEN_HEIGHT) / 2.0f));

    float red = graphicsRandom.randf(1.0f);
    float green = graphicsRandom.randf(1.0f);
    Sector::get().add<Particles>(
      pos, 0, 360, 140.0f, 140.0f,
      Vector(0, 0), 45, Color(red, green, 0.0f), 3, 1.3f,
      LAYER_FOREGROUND1+1);
    SoundManager::current()->play("sounds/fireworks.wav");
    timer.start(graphicsRandom.randf(1.0f, 1.5f));
  }
}

void
Fireworks::draw(DrawingContext& )
{
}

/* EOF */
