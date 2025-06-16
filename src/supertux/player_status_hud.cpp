//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/player_status_hud.hpp"

#include <iostream>

#include "object/display_effect.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/debug.hpp"
#include "supertux/game_object.hpp"
#include "supertux/level.hpp"
#include "supertux/player_status.hpp"
#include "supertux/resources.hpp"
#include "supertux/title_screen.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "editor/editor.hpp"
#include "worldmap/worldmap_sector.hpp"

static const int DISPLAYED_COINS_UNSET = -1;

PlayerStatusHUD::PlayerStatusHUD(PlayerStatus& player_status) :
  m_player_status(player_status),
  displayed_coins(DISPLAYED_COINS_UNSET),
  displayed_coins_frame(0),
  coin_surface(Surface::from_file("images/engine/hud/coins-0.png")),
  m_bonus_sprites(),
  m_item_pocket_border(Surface::from_file("images/engine/hud/item_pocket.png"))
{
  m_bonus_sprites[BONUS_FIRE]  = SpriteManager::current()->create("images/powerups/fireflower/fireflower.sprite");
  m_bonus_sprites[BONUS_ICE]   = SpriteManager::current()->create("images/powerups/iceflower/iceflower.sprite");
  m_bonus_sprites[BONUS_AIR]   = SpriteManager::current()->create("images/powerups/airflower/airflower.sprite");
  m_bonus_sprites[BONUS_EARTH] = SpriteManager::current()->create("images/powerups/earthflower/earthflower.sprite");
}

void
PlayerStatusHUD::reset()
{
  displayed_coins = DISPLAYED_COINS_UNSET;
}

void
PlayerStatusHUD::update(float dt_sec)
{
}

void
PlayerStatusHUD::draw(DrawingContext& context)
{
  if (g_debug.hide_player_hud || Editor::is_active() ||
      (Sector::current() && Sector::current()->get_effect().has_active_borders()))
    return;

  if ((displayed_coins == DISPLAYED_COINS_UNSET) ||
      (std::abs(displayed_coins - m_player_status.coins) > 100)) {
    displayed_coins = m_player_status.coins;
    displayed_coins_frame = 0;
  }
  if (++displayed_coins_frame > 2) {
    displayed_coins_frame = 0;
    if (displayed_coins < m_player_status.coins) displayed_coins++;
    if (displayed_coins > m_player_status.coins) displayed_coins--;
  }
  displayed_coins = std::min(std::max(displayed_coins, 0), m_player_status.get_max_coins());

  float hudpos = BORDER_Y + 1.0f;
  const std::string coins_text = std::to_string(displayed_coins);

  context.push_transform();
  context.set_translation(Vector(0, 0));
  context.transform().scale = 1.f;
  if (coin_surface)
  {
    context.color().draw_surface(coin_surface,
                                Vector(context.get_width() - BORDER_X - static_cast<float>(coin_surface->get_width()) - Resources::fixed_font->get_text_width(coins_text),
                                       hudpos),
                                LAYER_HUD);
  }

  context.color().draw_text(Resources::fixed_font,
                            coins_text,
                            Vector(static_cast<float>(context.get_width()) - BORDER_X - Resources::fixed_font->get_text_width(coins_text),
                                  hudpos + 13.f),
                            ALIGN_LEFT,
                            LAYER_HUD,
                            PlayerStatusHUD::text_color);


  if (m_player_status.is_item_pocket_allowed())
  {
    for (int i = 0; i < InputManager::current()->get_num_users(); i++)
    {
      float ypos = static_cast<float>(m_item_pocket_border->get_height() * i);
      Vector pos(BORDER_X, BORDER_Y + ypos);
      context.color().draw_surface(m_item_pocket_border, pos, LAYER_HUD);

      if (m_bonus_sprites.find(m_player_status.m_item_pockets[i]) != m_bonus_sprites.end())
      {
        pos += 20;

        Sprite* sprite = m_bonus_sprites[m_player_status.m_item_pockets[i]].get();
        if (sprite)
          sprite->draw(context.color(), pos, LAYER_HUD);
      }
    }
  }

  context.pop_transform();
}

/* EOF */
