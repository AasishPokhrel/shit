//  SuperTux
//  Copyright (C) 2023 mrkubax10 <mrkubax10@onet.pl>
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

#include "object/sound_object.hpp"

#include <limits>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "editor/editor.hpp"
#include "util/reader_mapping.hpp"

SoundObject::SoundObject(const ReaderMapping& mapping) :
  LayerObject(mapping),
  m_sample(),
  m_sound_source(),
  m_volume(),
  m_started(false)
{
  mapping.get("sample", m_sample, "");
  mapping.get("volume", m_volume, 1.0f);

  prepare_sound_source();
}

SoundObject::SoundObject(float vol, const std::string& file) :
  m_sample(file),
  m_sound_source(),
  m_volume(vol),
  m_started(false)
{
  prepare_sound_source();
}

SoundObject::~SoundObject()
{
  stop_looping_sounds();
}

void
SoundObject::update(float dt_sec)
{
  if (m_started)
    return;
  m_sound_source->play();
  m_started = true;
}

ObjectSettings
SoundObject::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_sound(_("Sound"), &m_sample, "sample");
  result.add_float(_("Volume"), &m_volume, "volume");

  result.reorder({"sample", "volume", "name"});

  result.add_remove();

  return result;
}

void
SoundObject::stop_looping_sounds()
{
  if (m_sound_source)
    m_sound_source->pause();
}

void
SoundObject::play_looping_sounds()
{
  if (!Editor::is_active() && m_sound_source)
    m_sound_source->play();
}

void
SoundObject::prepare_sound_source()
{
  if (Editor::is_active())
    return;

  if (m_sample.empty())
  {
    remove_me();
    return;
  }

  try
  {
    m_sound_source = SoundManager::current()->create_sound_source(m_sample);
    if (!m_sound_source)
      throw std::runtime_error("file not found");

    m_sound_source->set_gain(0);
    m_sound_source->set_looping(true);
    m_sound_source->set_relative(true);
    m_sound_source->set_gain(m_volume);
  }
  catch(const std::exception& e)
  {
    log_warning << "Couldn't load '" << m_sample << "': " << e.what() << std::endl;
    m_sound_source.reset();
    remove_me();
  }
}

void
SoundObject::set_volume(float volume)
{
  m_volume = volume;
  m_sound_source->set_gain(volume);
}


void
SoundObject::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<SoundObject>("SoundObject", vm.findClass("GameObject"));

  cls.addFunc("start_playing", &SoundObject::play_looping_sounds);
  cls.addFunc("stop_playing", &SoundObject::stop_looping_sounds);
  cls.addFunc("set_volume", &SoundObject::set_volume);
  cls.addFunc("get_volume", &SoundObject::get_volume);

  cls.addVar("volume", &SoundObject::get_volume, &SoundObject::set_volume);
}

/* EOF */
