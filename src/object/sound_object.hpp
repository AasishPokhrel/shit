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

#ifndef HEADER_SUPERTUX_SOUND_OBJECT_HPP
#define HEADER_SUPERTUX_SOUND_OBJECT_HPP

#include "editor/layer_object.hpp"

class SoundSource;

/** Plays sound at given interval with specified volume hearable in entire Sector */
class SoundObject final : public LayerObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  SoundObject(const ReaderMapping& mapping);
  SoundObject(float vol, const std::string& file);
  ~SoundObject() override;

  virtual void draw(DrawingContext& context) override {}
  virtual void update(float dt_sec) override;

  static std::string class_name() { return "sound-object"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "SoundObject"; }
  static std::string display_name() { return _("Sound"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(SoundObject)); }
  virtual const std::string get_icon_path() const override { return "images/engine/editor/sound.png"; }

  virtual ObjectSettings get_settings() override;

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

  /** @name Scriptable methods
      @{ */

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description Starts playing sound, if currently stopped.
   */
  void start_playing();
  /**
   * @scripting
   * @description Stops playing sound.
   */
  void stop_playing();
#endif

  /**
   * @scripting
   * @description Sets the volume of the played sound.
   * @param float $volume
   */
  void set_volume(float volume);
  /**
   * @scripting
   * @description Returns the volume of the played sound.
   */
  inline float get_volume() const { return m_volume; }

  /** @} */

private:
  std::string m_sample;
  std::unique_ptr<SoundSource> m_sound_source;
  /**
   * @scripting
   * @description The volume of the played sound.
   */
  float m_volume;
  bool m_started;

private:
  void prepare_sound_source();

private:
  SoundObject(const SoundObject&) = delete;
  SoundObject& operator=(const SoundObject&) = delete;
};

#endif

/* EOF */
