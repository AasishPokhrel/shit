//  SuperTux
//  Copyright (C) 2024 Daniel Ward <weluvgoatz@gmail.com>
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

namespace
{
  const int DEFAULT_LIVES = 5; // default lives
  const int DEFAULT_PINCH_LIVES = 2; // default pinch lives
}

#include "badguy/boss.hpp"

#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/surface.hpp"

Boss::Boss(const ReaderMapping& reader, const std::string& sprite_name, int layer) :
  BadGuy(reader, sprite_name, layer),
  m_lives(),
  m_max_lives(),
  m_pinch_lives(),
  m_hud_head(),
  m_hud_icon(),
  m_pinch_mode(),
  m_pinch_activation_script()
{
  reader.get("lives", m_lives, DEFAULT_LIVES);
  m_max_lives = m_lives;

  m_countMe = true;

  reader.get("pinch-lives", m_pinch_lives, DEFAULT_PINCH_LIVES);
  reader.get("pinch-activation-script", m_pinch_activation_script, "");
}

void
Boss::boss_update(float dt_sec)
{
  if (!m_pinch_mode && (m_lives <= m_pinch_lives))
  {
    m_pinch_mode = true;
    Sector::get().run_script(m_pinch_activation_script, "pinch-activation-script");
  }
}

void
Boss::draw(DrawingContext& context)
{
  draw_hit_points(context);
  BadGuy::draw(context);
}

void
Boss::draw_hit_points(DrawingContext& context)
{
  if (m_hud_head)
  {
    context.push_transform();
    context.set_translation(Vector(0, 0));
    context.transform().scale = 1.f;

    float startpos = (context.get_width() - static_cast<float>((m_hud_head->get_width() * m_max_lives))) / 2;
    for (int i = 0; i < m_lives; ++i)
    {
      context.color().draw_surface(m_hud_head,
                                   Vector(BORDER_X + (startpos + static_cast<float>(i * m_hud_head->get_width())),
                                          BORDER_Y + 1),
                                   LAYER_HUD);
    }

    context.pop_transform();
  }
}

ObjectSettings
Boss::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_text("hud-icon", &m_hud_icon, "hud-icon", "images/creatures/yeti/hudlife.png", OPTION_HIDDEN);
  result.add_int(_("Lives"), &m_lives, "lives", DEFAULT_LIVES);

  /* l10n: Pinch Mode refers to a particular boss mode that gets
     activated once the boss has lost the specified amounts of lives.
     This setting specifies how many lives need to be spent until pinch
     mode is activated. */
  result.add_int(_("Lives to Pinch Mode"), &m_pinch_lives, "pinch-lives", DEFAULT_PINCH_LIVES);

  /* l10n: Pinch Mode refers to a particular boss mode that gets
    activated once the boss has lost the specified amounts of lives.
    This setting specifies the squirrel script that gets run to activate boss mode.  */
  result.add_script(_("Pinch Mode Activation Script"), &m_pinch_activation_script, "pinch-activation-script");

  return result;
}
