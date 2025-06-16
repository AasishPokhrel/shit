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

#ifndef HEADER_SUPERTUX_OBJECT_DISPLAY_EFFECT_HPP
#define HEADER_SUPERTUX_OBJECT_DISPLAY_EFFECT_HPP

#include "supertux/game_object.hpp"

/**
 * @scripting
 * @summary ""DisplayEffect"" is an interface for toying with the display.
 * @instances SuperTux creates an instance named ""Effect"" when starting the scripting engine.
              Its usage is preferred – creating another instance might have unexpected side effects and is strongly discouraged.
              (Use ""sector.Effect"" in the console.)
 */
class DisplayEffect final : public GameObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  DisplayEffect(const std::string& name = "");
  ~DisplayEffect() override;
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(DisplayEffect)); }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual bool is_singleton() const override { return true; }
  virtual bool is_saveable() const override { return false; }
  virtual std::string get_exposed_class_name() const override { return "DisplayEffect"; }

  inline bool has_active_borders() const { return borders; }

  /** @name Scriptable Methods
      @{ */

  /**
   * @scripting
   * @description Gradually fades out the screen to black for the next ""time"" seconds.
   * @param float $time
   */
  void fade_out(float time);
  /**
   * @scripting
   * @description Gradually fades in the screen from black for the next ""time"" seconds.
   * @param float $time
   */
  void fade_in(float time);
  /**
   * @scripting
   * @description Blackens or un-blackens the screen (depending on the value of ""black"").
   * @param bool $black
   */
  inline void set_black(bool black_) { black = black_; }
  /**
   * @scripting
   * @description Returns ""true"" if the screen has been blackened by ""set_black"".
     Note: Calling ""fade_in"" or ""fade_out"" resets the return value to ""false"".
   */
  inline bool is_black() const { return black; }
  /**
   * @scripting
   * @description Sets the display ratio to 16:9, effectively adding black bars at the top and bottom of the screen.
                  Should be used before cutscenes. Gradually fades to this state for the next ""time"" seconds.
   * @param float $time
   */
  void sixteen_to_nine(float time);
  /**
   * @scripting
   * @description Sets the display ratio to 4:3, removing the black bars added by ""sixteen_to_nine()"".
                  Should be used after cutscenes. Gradually fades to this state for the next ""time"" seconds.
   * @param float $time
   */
  void four_to_three(float time);

  /** @} */

private:
  enum class FadeType {
    NO_FADE, FADE_IN, FADE_OUT
  };

private:
  FadeType screen_fade;
  float screen_fadetime;
  float screen_fading;
  FadeType border_fade;
  float border_fadetime;
  float border_fading;
  float border_size;

  /**
   * @scripting
   * @description Determines whether the screen has been blackened.
                  Equivalent to ""set_black()"" and ""is_black()"".
   */
  bool black;
  bool borders;

private:
  DisplayEffect(const DisplayEffect&) = delete;
  DisplayEffect& operator=(const DisplayEffect&) = delete;
};

#endif

/* EOF */
