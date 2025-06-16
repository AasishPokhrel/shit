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

#include "object/floating_text.hpp"

#include <stdio.h>

#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"

FloatingText::FloatingText(const Vector& pos, const std::string& text_) :
  position(pos),
  text(text_),
  timer()
{
  timer.start(.1f);
  position.x -= static_cast<float>(text.size()) * 8.0f;
}

FloatingText::FloatingText(const Vector& pos, int score) :
  position(pos),
  text(std::to_string(score)),
  timer()
{
  timer.start(.1f);

  position.x -= static_cast<float>(text.size()) * 8.0f;
}

void
FloatingText::update(float dt_sec)
{
  position.y -= 1.4f * dt_sec;

  if (timer.check())
    remove_me();
}

const float FADING_TIME = .350f;

void
FloatingText::draw(DrawingContext& context)
{
  // Make an alpha animation when disappearing.
  float alpha;
  if (timer.get_timeleft() < FADING_TIME)
    alpha = timer.get_timeleft() * 255.0f / FADING_TIME;
  else
    alpha = 255.0f;

  context.push_transform();
  context.set_alpha(alpha);

  context.color().draw_text(Resources::normal_font, text, position, ALIGN_LEFT, LAYER_OBJECTS+1, FloatingText::text_color);

  context.pop_transform();
}

/* EOF */
