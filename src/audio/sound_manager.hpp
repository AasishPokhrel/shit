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

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_MANAGER_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_MANAGER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <al.h>
#include <alc.h>

#include "math/vector.hpp"
#include "util/currenton.hpp"

class SoundFile;
class SoundSource;
class StreamSoundSource;
class OpenALSoundSource;

class SoundManager final : public Currenton<SoundManager>
{
  friend class OpenALSoundSource;
  friend class StreamSoundSource;

private:
  static ALuint load_file_into_buffer(SoundFile& file);
  static ALenum get_sample_format(const SoundFile& file);

  static void print_openal_version();
  static void check_al_error(const char* message);

public:
  SoundManager();
  ~SoundManager() override;

  void enable_sound(bool sound_enabled);

  /** Creates a new sound source object which plays the specified
      soundfile. You are responsible for deleting the sound source
      later (this will stop the sound).
      This function never throws exceptions, but might return a DummySoundSource */
  std::unique_ptr<SoundSource> create_sound_source(const std::string& filename);

  /** Convenience functions to simply play a sound at a given position. */
  void play(const std::string& name, const Vector& pos = Vector(-1, -1),
    const float gain = 0.5f);
  void play(const std::string& name, const float gain)
  {
    play(name, Vector(-1, -1), gain);
  }


  /** Adds the source to the list of managed sources (= the source gets deleted
      when it finished playing) */
  void manage_source(std::unique_ptr<SoundSource> source);

  /** preloads a sound, so that you don't get a lag later when playing it */
  void preload(const std::string& name);

  void set_listener_position(const Vector& position);
  void set_listener_velocity(const Vector& velocity);
  void set_listener_orientation(const Vector& at, const Vector& up);

  void enable_music(bool music_enabled);
  void play_music(const std::string& filename, float fadetime);
  void play_music(const std::string& filename, bool fade = false);
  void pause_music(float fadetime = 0);
  void resume_music(float fadetime = 0);
  void stop_music(float fadetime = 0);
  void set_music_volume(int volume);

  void pause_sounds();
  void resume_sounds();
  void stop_sounds();
  void set_sound_volume(int volume);

  inline bool is_music_enabled() const { return m_music_enabled; }
  inline bool is_sound_enabled() const { return m_sound_enabled; }

  inline bool is_audio_enabled() const { return m_device != nullptr && m_context != nullptr; }
  inline const std::string& get_current_music() const { return m_current_music; }
  void update();

  /** Tell soundmanager to call update() for stream_sound_source. */
  void register_for_update(StreamSoundSource* sss);

  /** Unsubscribe from updates for stream_sound_source. */
  void remove_from_update(StreamSoundSource* sss);

private:
  /** creates a new sound source, might throw exceptions, never returns nullptr */
  std::unique_ptr<OpenALSoundSource> intern_create_sound_source(const std::string& filename);

  void check_alc_error(const char* message) const;

private:
  ALCdevice* m_device;
  ALCcontext* m_context;
  bool m_sound_enabled;
  int m_sound_volume;

  std::map<std::string, ALuint> m_buffers;
  std::vector<std::unique_ptr<OpenALSoundSource> > m_sources;

  std::vector<StreamSoundSource*> m_update_list;

  std::unique_ptr<StreamSoundSource> m_music_source;

  bool m_music_enabled;
  int m_music_volume;
  std::string m_current_music;

private:
  SoundManager(const SoundManager&) = delete;
  SoundManager& operator=(const SoundManager&) = delete;
};

#endif

/* EOF */
