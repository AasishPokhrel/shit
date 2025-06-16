//  SuperTux
//  Copyright (C) 2020 Grzegorz Przybylski <zwatotem@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_VERTICAL_STRIPES_HPP
#define HEADER_SUPERTUX_OBJECT_VERTICAL_STRIPES_HPP

#include "math/rect.hpp"
#include "math/rectf.hpp"

#include "supertux/game_object.hpp"

class VerticalStripes final : public GameObject
{
public:
	VerticalStripes();
	~VerticalStripes() override;

  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(VerticalStripes)); }

  virtual bool is_singleton() const override { return true; }
  virtual bool is_saveable() const override { return false; }
  virtual void update(float dt_sec) override;
 	virtual void draw(DrawingContext& context) override;

private:
  bool m_visible;
  int m_layer;
  Rectf m_left_stripe;
  Rectf m_right_stripe;

private:
  VerticalStripes(const VerticalStripes&) = delete;
  VerticalStripes& operator=(const VerticalStripes&) = delete;
};

#endif

/* EOF */
