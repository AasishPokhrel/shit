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

#include "object/display_effect.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"

static const float BORDER_SIZE = 75;

DisplayEffect::DisplayEffect(const std::string& name) :
  GameObject(name),
  screen_fade(FadeType::NO_FADE),
  screen_fadetime(0),
  screen_fading(0),
  border_fade(FadeType::NO_FADE),
  border_fadetime(0),
  border_fading(),
  border_size(0),
  black(false),
  borders(false)
{
}

DisplayEffect::~DisplayEffect()
{
}

void
DisplayEffect::update(float dt_sec)
{
  switch (screen_fade) {
    case FadeType::NO_FADE:
      break;
    case FadeType::FADE_IN:
      screen_fading -= dt_sec;
      if (screen_fading < 0) {
        screen_fade = FadeType::NO_FADE;
      }
      break;
    case FadeType::FADE_OUT:
      screen_fading -= dt_sec;
      if (screen_fading < 0) {
        screen_fade = FadeType::NO_FADE;
        black = true;
      }
      break;
    default:
      assert(false);
  }

  switch (border_fade) {
    case FadeType::NO_FADE:
      break;
    case FadeType::FADE_IN:
      border_fading -= dt_sec;
      if (border_fading < 0) {
        border_fade = FadeType::NO_FADE;
      }
      border_size = (border_fadetime - border_fading)
        / border_fadetime * BORDER_SIZE;
      break;
    case FadeType::FADE_OUT:
      border_fading -= dt_sec;
      if (border_fading < 0) {
        borders = false;
        border_fade = FadeType::NO_FADE;
      }
      border_size = border_fading / border_fadetime * BORDER_SIZE;
      break;
    default:
      assert(false);
  }
}

void
DisplayEffect::draw(DrawingContext& context)
{
  context.push_transform();
  context.set_translation(Vector(0, 0));
  context.transform().scale = 1.f;

  if (black || screen_fade != FadeType::NO_FADE) {
    float alpha;
    if (black) {
      alpha = 1.0f;
    } else {
      switch (screen_fade) {
        case FadeType::FADE_IN:
          alpha = screen_fading / screen_fadetime;
          break;
        case FadeType::FADE_OUT:
          alpha = (screen_fadetime - screen_fading) / screen_fadetime;
          break;
        default:
          alpha = 0.0f; // NOLINT
          assert(false);
      }

      // Same as in fadetoblack.cpp.
      alpha = Color::remove_gamma(alpha);
    }
    context.color().draw_filled_rect(context.get_rect(), Color(0, 0, 0, alpha), LAYER_GUI - 10);
  }

  if (borders) {
    context.color().draw_filled_rect(Rectf(0, 0,
                                           context.get_width(),
                                           static_cast<float>(border_size)),
                                       Color(0, 0, 0, 1.0f), LAYER_GUI-10);
    context.color().draw_filled_rect(Rectf(Vector(0,
                                                  context.get_height() - border_size),
                                           Sizef(context.get_width(),
                                                 static_cast<float>(border_size))),
                                       Color(0, 0, 0, 1.0f), LAYER_GUI-10);
  }

  context.pop_transform();
}

void
DisplayEffect::fade_out(float fadetime)
{
  black = false;
  screen_fadetime = fadetime;
  screen_fading = fadetime;
  screen_fade = FadeType::FADE_OUT;
}

void
DisplayEffect::fade_in(float fadetime)
{
  black = false;
  screen_fadetime = fadetime;
  screen_fading = fadetime;
  screen_fade = FadeType::FADE_IN;
}

void
DisplayEffect::sixteen_to_nine(float fadetime)
{
  if (fadetime == 0) {
    borders = true;
    border_size = BORDER_SIZE;
  } else {
    borders = true;
    border_size = 0;
    border_fade = FadeType::FADE_IN;
    border_fadetime = fadetime;
    border_fading = border_fadetime;
  }
}

void
DisplayEffect::four_to_three(float fadetime)
{
  if (fadetime == 0) {
    borders = false;
  } else {
    border_size = BORDER_SIZE;
    border_fade = FadeType::FADE_OUT;
    border_fadetime = fadetime;
    border_fading = border_fadetime;
  }
}


void
DisplayEffect::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<DisplayEffect>("DisplayEffect", vm.findClass("GameObject"));

  cls.addFunc("fade_out", &DisplayEffect::fade_out);
  cls.addFunc("fade_in", &DisplayEffect::fade_in);
  cls.addFunc("set_black", &DisplayEffect::set_black);
  cls.addFunc("is_black", &DisplayEffect::is_black);
  cls.addFunc("sixteen_to_nine", &DisplayEffect::sixteen_to_nine);
  cls.addFunc("four_to_three", &DisplayEffect::four_to_three);

  cls.addVar("black", &DisplayEffect::black);
}

/* EOF */
