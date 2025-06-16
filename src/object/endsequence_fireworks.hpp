//  SuperTux - End Sequence: Tux walks right
//  Copyright (C) 2007 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_FIREWORKS_HPP
#define HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_FIREWORKS_HPP

#include "object/endsequence.hpp"
#include "supertux/timer.hpp"

class EndSequenceFireworks final : public EndSequence
{
public:
  EndSequenceFireworks();
  ~EndSequenceFireworks() override;
  virtual GameObjectClasses get_class_types() const override { return EndSequence::get_class_types().add(typeid(EndSequenceFireworks)); }
  virtual void draw(DrawingContext& context) override;

protected:
  virtual void starting() override; /**< called when EndSequence starts */
  virtual void running(float dt_sec) override; /**< called while the EndSequence is running */
  virtual void stopping() override; /**< called when EndSequence stops */

  Timer endsequence_timer;
};

#endif

/* EOF */
