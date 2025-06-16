//  SuperTux
//  Copyright (C) 2009 qMax
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

#include "supertux/colorscheme.hpp"

#include "editor/overlay_widget.hpp"
#include "interface/control.hpp"
#include "object/floating_text.hpp"
#include "object/level_time.hpp"
#include "object/text_object.hpp"
#include "supertux/levelintro.hpp"
#include "supertux/player_status_hud.hpp"
#include "supertux/statistics.hpp"
#include "supertux/textscroller_screen.hpp"
#include "trigger/climbable.hpp"
#include "trigger/secretarea_trigger.hpp"
#include "worldmap/worldmap.hpp"

Color LevelIntro::s_header_color(1.f,1.f,0.6f);
Color LevelIntro::s_author_color(1.f,1.f,1.f);
Color LevelIntro::s_stat_hdr_color(0.2f,0.5f,1.f);
Color LevelIntro::s_stat_color(1.f,1.f,1.f);
Color LevelIntro::s_stat_perfect_color(0.4f,1.f,0.4f);

Color Statistics::header_color(1.f,1.f,1.f);
Color Statistics::text_color(1.f,1.f,0.6f);
Color Statistics::perfect_color(0.4f,1.f,0.4f);

Color ColorScheme::Menu::back_color(0.2f, 0.3f, 0.4f, 0.8f);
Color ColorScheme::Menu::front_color(0.6f, 0.7f, 0.8f, 0.5f);
Color ColorScheme::Menu::help_back_color(0.5f, 0.6f, 0.7f, 0.8f);
Color ColorScheme::Menu::help_front_color(0.8f, 0.9f, 1.f, 0.5f);
Color ColorScheme::Menu::hl_color(0.6f, 0.7f, 1.f, 1.f);
Color ColorScheme::Menu::default_color(1.f,1.f,1.f);
Color ColorScheme::Menu::active_color(0.4f,0.66f,1.f);
Color ColorScheme::Menu::inactive_color(0.5f,0.5f,0.5f);
Color ColorScheme::Menu::label_color(0.f,1.f,1.f);
Color ColorScheme::Menu::field_color(1.f,1.f,0.6f);
Color ColorScheme::Menu::warning_color(1.f,1.f,0.6f);

Color PlayerStatusHUD::text_color(1.f,1.f,0.6f);

Color TextObject::default_color(1.f,1.f,1.f);

Color FloatingText::text_color(1.f,1.f,0.6f);

Color LevelTime::text_color(1.f,1.f,0.6f);

Color SecretAreaTrigger::text_color(1.f,1.f,0.6f);

Color Climbable::text_color(1.f,1.f,0.6f);

Color worldmap::WorldMap::s_level_title_color(1.f,1.f,1.f);
Color worldmap::WorldMap::s_message_color(1.f,1.f,0.6f);
Color worldmap::WorldMap::s_teleporter_message_color(1.f,1.f,1.f);

Color ColorScheme::Text::small_color(1.f,1.f,1.f);
Color ColorScheme::Text::heading_color(1.f,1.f,0.6f);
Color ColorScheme::Text::reference_color(0.2f,0.6f,1.f);
Color ColorScheme::Text::normal_color(1.f,1.f,1.f);

Color ColorScheme::Editor::default_color(0.9f, 0.9f, 1.0f, 0.6f);
Color ColorScheme::Editor::hover_color(1.f, 1.f, 1.f, 0.4f);
Color ColorScheme::Editor::grab_color(1.f, 1.f, 1.f, 0.7f);
Color EditorOverlayWidget::text_autotile_available_color(1.f,1.f,0.6f);
Color EditorOverlayWidget::text_autotile_active_color(1.f,1.f,1.f);
Color EditorOverlayWidget::text_autotile_error_color(1.f,0.2f,0.1f);
Color EditorOverlayWidget::warning_color(1.f, 0.5f, 0.f);
Color EditorOverlayWidget::error_color(1.f, 0.2f, 0.f);

/* EOF */
