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

#include "audio/sound_manager.hpp"

#include <SDL.h>
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <memory>

#include "audio/dummy_sound_source.hpp"
#include "audio/sound_file.hpp"
#include "audio/stream_sound_source.hpp"
#include "util/log.hpp"

SoundManager::SoundManager() :
  m_device(alcOpenDevice(nullptr)),
  m_context(alcCreateContext(m_device, nullptr)),
  m_sound_enabled(false),
  m_sound_volume(0),
  m_buffers(),
  m_sources(),
  m_update_list(),
  m_music_source(),
  m_music_enabled(false),
  m_music_volume(0),
  m_current_music()
{
  try {
    if (m_device == nullptr) {
      throw std::runtime_error("Couldn't open audio device.");
    }
    check_alc_error("Couldn't create audio context: ");
    alcMakeContextCurrent(m_context);
    check_alc_error("Couldn't select audio context: ");

    check_al_error("Audio error after init: ");
    m_sound_enabled = true;
    m_music_enabled = true;

    set_listener_orientation(Vector(0.0f, 0.0f), Vector(0.0f, -1.0f));
  } catch(std::exception& e) {
    if (m_context != nullptr) {
      alcDestroyContext(m_context);
      m_context = nullptr;
    }
    if (m_device != nullptr) {
      alcCloseDevice(m_device);
      m_device = nullptr;
    }
    log_warning << "Couldn't initialize audio device: " << e.what() << std::endl;
    print_openal_version();
  }
}

SoundManager::~SoundManager()
{
  m_music_source.reset();
  m_sources.clear();

  for (const auto& buffer : m_buffers) {
    alDeleteBuffers(1, &buffer.second);
  }

  if (m_context != nullptr) {
    alcDestroyContext(m_context);
    m_context = nullptr;
  }
  if (m_device != nullptr) {
    alcCloseDevice(m_device);
    m_device = nullptr;
  }
}

ALuint
SoundManager::load_file_into_buffer(SoundFile& file)
{
  ALenum format = get_sample_format(file);
  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_error("Couldn't create audio buffer: ");
  std::unique_ptr<char[]> samples(new char[file.m_size]);
  file.read(samples.get(), file.m_size);
  log_debug << "buffer: " << buffer << "\n"
            << "format: " << format << "\n"
            << "samples: " << samples.get() << "\n"
            << "file size: " << static_cast<ALsizei>(file.m_size) << "\n"
            << "file rate: " << static_cast<ALsizei>(file.m_rate) << "\n";

  alBufferData(buffer, format, samples.get(),
               static_cast<ALsizei>(file.m_size),
               static_cast<ALsizei>(file.m_rate));
  check_al_error("Couldn't fill audio buffer: ");

  return buffer;
}

std::unique_ptr<OpenALSoundSource>
SoundManager::intern_create_sound_source(const std::string& filename)
{
  assert(m_sound_enabled);

  auto source = std::make_unique<OpenALSoundSource>();
  source->set_volume(static_cast<float>(m_sound_volume) / 100.0f);

  ALuint buffer;

  // reuse an existing static sound buffer
  auto it = m_buffers.find(filename);
  if (it != m_buffers.end()) {
    buffer = it->second;
  } else {
    // Load sound file
    std::unique_ptr<SoundFile> file(load_sound_file(filename));

    if (file->m_size < 100000) {
      log_debug << "Adding \"" << filename <<
        "\" into the buffer, file size: " << file->m_size << std::endl;
      buffer = load_file_into_buffer(*file);
      m_buffers.insert(std::make_pair(filename, buffer));
    } else {
      log_debug << "Playing \"" << filename <<
        "\" as StreamSoundSource, file size: " << file->m_size << std::endl;
      auto stream_source = std::make_unique<StreamSoundSource>();
      stream_source->set_sound_file(std::move(file));
      stream_source->set_volume(static_cast<float>(m_sound_volume) / 100.0f);
      return std::unique_ptr<OpenALSoundSource>(stream_source.release());
    }
  }

  alSourcei(source->m_source, AL_BUFFER, buffer);
  return source;
}

std::unique_ptr<SoundSource>
SoundManager::create_sound_source(const std::string& filename)
{
  if (!m_sound_enabled)
    return create_dummy_sound_source();

  try {
    return intern_create_sound_source(filename);
  } catch(std::exception &e) {
    log_warning << "Couldn't create audio source: " << e.what() << std::endl;
    return create_dummy_sound_source();
  }
}

void
SoundManager::preload(const std::string& filename)
{
  if (!m_sound_enabled)
    return;

  auto it = m_buffers.find(filename);
  // already loaded?
  if (it != m_buffers.end())
    return;
  try {
    std::unique_ptr<SoundFile> file (load_sound_file(filename));
    // only keep small files
    if (file->m_size >= 100000)
      return;

    ALuint buffer = load_file_into_buffer(*file);
    m_buffers.insert(std::make_pair(filename, buffer));
  } catch(std::exception& e) {
    log_warning << "Error while preloading sound file: " << e.what() << std::endl;
  }
}

void
SoundManager::play(const std::string& filename, const Vector& pos,
  const float gain)
{
  if (!m_sound_enabled)
    return;

  // Test gain for invalid values; it must not exceed 1 because in the end
  // the value is set to min(sound_gain * sound_volume, 1)
  assert(gain >= 0.0f && gain <= 1.0f);

  try {
    std::unique_ptr<OpenALSoundSource> source(intern_create_sound_source(filename));
    source->set_gain(gain);

    if (pos.x < 0 || pos.y < 0) {
      source->set_relative(true);
    } else {
      source->set_position(pos);
    }
    source->play();
    m_sources.push_back(std::move(source));
  } catch(std::exception& e) {
    log_warning << "Couldn't play sound " << filename << ": " << e.what() << std::endl;
  }
}

void
SoundManager::manage_source(std::unique_ptr<SoundSource> source)
{
  assert(source);
  if (dynamic_cast<OpenALSoundSource*>(source.get()))
  {
    std::unique_ptr<OpenALSoundSource> openal_source(static_cast<OpenALSoundSource*>(source.release()));
    m_sources.push_back(std::move(openal_source));
  }
}

void
SoundManager::register_for_update(StreamSoundSource* sss)
{
  if (sss)
  {
    m_update_list.push_back(sss);
  }
}

void
SoundManager::remove_from_update(StreamSoundSource* sss)
{
  if (sss)
  {
    auto it = m_update_list.begin();
    while (it != m_update_list.end()) {
      if (*it == sss) {
        it = m_update_list.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void
SoundManager::enable_sound(bool enable)
{
  if (m_device == nullptr)
    return;

  m_sound_enabled = enable;
}

void
SoundManager::enable_music(bool enable)
{
  if (m_device == nullptr)
    return;

  m_music_enabled = enable;
  if (m_music_enabled) {
    play_music(m_current_music);
  } else {
    if (m_music_source) {
      m_music_source.reset();
    }
  }
}

void
SoundManager::stop_music(float fadetime)
{
  if (fadetime > 0) {
    if (m_music_source
       && m_music_source->get_fade_state() != StreamSoundSource::FadingOff)
      m_music_source->set_fading(StreamSoundSource::FadingOff, fadetime);
  } else {
    m_music_source.reset();
  }
  m_current_music = "";
}

void
SoundManager::set_music_volume(int volume)
{
  m_music_volume = volume;
  if (m_music_source != nullptr) m_music_source->set_volume(static_cast<float>(volume) / 100.0f);
}

void 
SoundManager::play_music(const std::string& filename, float fadetime)
{
  if (filename == m_current_music && m_music_source != nullptr)
  {
    if (m_music_source->paused())
    {
      m_music_source->resume();
    }
    else if (!m_music_source->playing())
    {
      m_music_source->play();
    }
    return;
  }
  m_current_music = filename;
  if (!m_music_enabled)
    return;

  if (filename.empty()) {
    m_music_source.reset();
    return;
  }

  try {
    auto newmusic = std::make_unique<StreamSoundSource>();
    newmusic->set_sound_file(load_sound_file(filename));
    newmusic->set_looping(true);
    newmusic->set_relative(true);
    newmusic->set_volume(static_cast<float>(m_music_volume) / 100.0f);
    if (fadetime > 0)
      newmusic->set_fading(StreamSoundSource::FadingOn, fadetime);
    newmusic->play();

    m_music_source = std::move(newmusic);
  } catch(std::exception& e) {
    log_warning << "Couldn't play music file '" << filename << "': " << e.what() << std::endl;
    // When this happens, previous music continued playing, stop it, just in case.
    stop_music(0);
  }
}

void
SoundManager::play_music(const std::string& filename, bool fade)
{
  play_music(filename, fade ? 0.5f : 0);
}

void
SoundManager::pause_music(float fadetime)
{
  if (m_music_source == nullptr)
    return;

  if (fadetime > 0) {
    if (m_music_source->get_fade_state() != StreamSoundSource::FadingPause)
      m_music_source->set_fading(StreamSoundSource::FadingPause, fadetime);
  } else {
    m_music_source->pause();
  }
}

void
SoundManager::pause_sounds()
{
  for (auto& source : m_sources) {
    if (source->playing()) {
      source->pause();
    }
  }
}

void
SoundManager::resume_sounds()
{
  for (auto& source : m_sources) {
    if (source->paused()) {
      source->resume();
    }
  }
}

void
SoundManager::stop_sounds()
{
  for (auto& source : m_sources) {
    source->stop();
  }
}

void
SoundManager::set_sound_volume(int volume)
{
  m_sound_volume = volume;
  for (auto& source : m_sources) {
    source->set_volume(static_cast<float>(volume) / 100.0f);
  }
}

void
SoundManager::resume_music(float fadetime)
{
  if (m_music_source == nullptr)
    return;

  if (fadetime > 0 && m_music_source->get_fade_state() == StreamSoundSource::FadingResume)
    return;

  if (fadetime > 0)
  {
    m_music_source->set_fading(StreamSoundSource::FadingResume, fadetime);
  }
  m_music_source->resume();
}

void
SoundManager::set_listener_position(const Vector& pos)
{
  static Uint32 lastticks = SDL_GetTicks();

  Uint32 current_ticks = SDL_GetTicks();
  if (current_ticks - lastticks < 300)
    return;
  lastticks = current_ticks;

  alListener3f(AL_POSITION, pos.x, pos.y, -300);
}

void
SoundManager::set_listener_velocity(const Vector& vel)
{
  alListener3f(AL_VELOCITY, vel.x, vel.y, 0);
}

void
SoundManager::set_listener_orientation(const Vector& at, const Vector& up)
{
  ALfloat orientation[]={at.x, at.y, 1.0, up.x, up.y, 0.0};
  alListenerfv(AL_ORIENTATION, orientation);
}

void
SoundManager::update()
{
  static Uint32 lasttime = SDL_GetTicks();
  Uint32 now = SDL_GetTicks();

  if (now - lasttime < 300)
    return;
  lasttime = now;

  // update and check for finished sound sources
  for (auto it = m_sources.begin(); it != m_sources.end(); ) {
    auto& source = *it;

    source->update();

    if (!source->playing()) {
      it = m_sources.erase(it);
    } else {
      ++it;
    }
  }
  // check streaming sounds
  if (m_music_source) {
    m_music_source->update();
  }

  if (m_context)
  {
    alcProcessContext(m_context);
    check_alc_error("Error while processing audio context: ");
  }

  //run update() for stream_sound_source
  auto s = m_update_list.begin();
  while (s != m_update_list.end()) {
    (*s)->update();
    ++s;
  }
}

ALenum
SoundManager::get_sample_format(const SoundFile& file)
{
  if (file.m_channels == 2) {
    if (file.m_bits_per_sample == 16) {
      return AL_FORMAT_STEREO16;
    } else if (file.m_bits_per_sample == 8) {
      return AL_FORMAT_STEREO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  } else if (file.m_channels == 1) {
    if (file.m_bits_per_sample == 16) {
      return AL_FORMAT_MONO16;
    } else if (file.m_bits_per_sample == 8) {
      return AL_FORMAT_MONO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  }

  throw std::runtime_error("Only 1 and 2 channel samples supported");
}

void
SoundManager::print_openal_version()
{
  log_info << "OpenAL Vendor: " << alGetString(AL_VENDOR) << std::endl;
  log_info << "OpenAL Version: " << alGetString(AL_VERSION) << std::endl;
  log_info << "OpenAL Renderer: " << alGetString(AL_RENDERER) << std::endl;
  log_info << "OpenAl Extensions: " << alGetString(AL_EXTENSIONS) << std::endl;
}

void
SoundManager::check_alc_error(const char* message) const
{
  int err = alcGetError(m_device);
  if (err != ALC_NO_ERROR) {
    std::stringstream msg;
    msg << message << alcGetString(m_device, err);
    throw std::runtime_error(msg.str());
  }
}

void
SoundManager::check_al_error(const char* message)
{
  int err = alGetError();
  if (err != AL_NO_ERROR) {
    std::stringstream msg;
    msg << message << alGetString(err);
    throw std::runtime_error(msg.str());
  }
}

/* EOF */
