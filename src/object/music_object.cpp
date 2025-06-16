//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "object/music_object.hpp"

#include "audio/sound_manager.hpp"
#include "object/player.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

MusicObject::MusicObject() :
  m_currentmusic(LEVEL_MUSIC),
  m_music()
{
}

MusicObject::MusicObject(const ReaderMapping& mapping) :
  LayerObject(mapping),
  m_currentmusic(LEVEL_MUSIC),
  m_music()
{
  mapping.get("file", m_music);
}

void
MusicObject::update(float dt_sec)
{
}

void
MusicObject::draw(DrawingContext& context)
{
}

void
MusicObject::play_music(MusicType type)
{
  m_currentmusic = type;
  switch (m_currentmusic)
  {
    case LEVEL_MUSIC:
      SoundManager::current()->play_music(m_music);
      break;

    case HERRING_MUSIC:
      SoundManager::current()->play_music("music/misc/invincible.ogg");
      break;

    case HERRING_WARNING_MUSIC:
      SoundManager::current()->stop_music(TUX_INVINCIBLE_TIME_WARNING);
      break;

    default:
      SoundManager::current()->play_music("");
      break;
  }
}

void
MusicObject::resume_music(bool instantly)
{
  if (SoundManager::current()->get_current_music() == m_music)
  {
    SoundManager::current()->resume_music(instantly ? 0.f : 3.2f);
  }
  else
  {
    SoundManager::current()->stop_music();
    SoundManager::current()->play_music(m_music, true);
  }
}

ObjectSettings
MusicObject::get_settings()
{
  auto settings = GameObject::get_settings();

  settings.add_music(_("File"), &m_music, "file");

  return settings;
}

/* EOF */
