//  SuperTux
//  Copyright (C) 2024 Vankata453
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

#ifndef HEADER_SUPERTUX_EDITOR_LAYER_OBJECT_HPP
#define HEADER_SUPERTUX_EDITOR_LAYER_OBJECT_HPP

#include "supertux/game_object.hpp"

/** Base class for all GameObjects listed as layers in EditorLayersWidget. */
class LayerObject : public GameObject
{
public:
  LayerObject(const std::string& name = "") :
    GameObject(name)
  {}
  LayerObject(const ReaderMapping& reader) :
    GameObject(reader)
  {}
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(LayerObject)); }

  virtual const std::string get_icon_path() const = 0;
  virtual int get_layer() const { return std::numeric_limits<int>::min(); }

private:
  LayerObject(const LayerObject&) = delete;
  LayerObject& operator=(const LayerObject&) = delete;
};

#endif

/* EOF */
