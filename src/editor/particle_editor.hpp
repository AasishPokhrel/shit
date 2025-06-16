//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_EDITOR_PARTICLE_EDITOR_HPP
#define HEADER_SUPERTUX_EDITOR_PARTICLE_EDITOR_HPP

#include <functional>
#include <vector>
#include <memory>

#include <SDL.h>

#include "interface/control.hpp"
#include "interface/control_button.hpp"
#include "interface/control_enum.hpp"
#include "interface/control_checkbox.hpp"
#include "interface/control_textbox_float.hpp"
#include "interface/control_textbox_int.hpp"
#include "interface/label.hpp"
#include "object/custom_particle_system.hpp"
#include "supertux/screen.hpp"
#include "util/currenton.hpp"
#include "util/writer.hpp"

class ParticleEditor final : public Screen,
                             public Currenton<ParticleEditor>
{
  friend class ParticleEditorMenu;

public:
  static bool is_active();

private:
  static bool (*m_clamp_0_1)(ControlTextboxFloat*, float);

public:
  ParticleEditor();
  ~ParticleEditor() override;

public:
  virtual void draw(Compositor&) override;
  virtual void update(float dt_sec, const Controller& controller) override;

  virtual void setup() override;
  virtual void leave() override;

  virtual IntegrationStatus get_status() const override;

  void event(const SDL_Event& ev) override;
  void update_keyboard(const Controller& controller);
  void check_unsaved_changes(const std::function<void ()>& action);
  void quit_editor();

  void reactivate();

  void open_particle_directory();

  // saves the particle to file
  void save(const std::string& filename, bool retry = false);
  void save(Writer& writer);
  void request_save(bool is_save_as = false,
    const std::function<void(bool)>& callback = [](bool was_saved){});
  inline void open(const std::string& filename) { m_filename = filename; reload(); }
  void new_file() { m_filename = ""; reload(); }

  /** Reloads the particle object from the filename in m_filename.
   *  If m_filename is empty, loads the default file (/particles/default.stcp)
   */
  void reload();

private:
  void reload_particles();
  void reset_main_ui();
  void reset_texture_ui();

  void addTextboxFloat(const std::string& name, float* bind,
                       bool (*float_validator)(ControlTextboxFloat*,
                                               float) = nullptr);
  void addTextboxFloatWithImprecision(const std::string& name, float* bind,
                                      float* imprecision_bind,
                                      bool (*float_validator)(ControlTextboxFloat*,
                                                              float) = nullptr,
                                      bool (*imp_validator)(ControlTextboxFloat*,
                                                            float) = nullptr);
  void addTextboxInt(const std::string& name, int* bind,
                     bool (*int_validator)(ControlTextboxInt*,
                                           int) = nullptr);
  void addCheckbox(std::string name, bool* bind);
  void addControl(std::string name,
                  std::unique_ptr<InterfaceControl> new_control);
  void addEasingEnum(std::string name, EasingMode* bind);

  void push_version();
  void undo();
  void redo();

public:
  bool m_enabled;
  bool m_quit_request;
  std::vector<std::unique_ptr<InterfaceControl>> m_controls;
  std::vector<std::unique_ptr<InterfaceControl>> m_controls_textures;
  std::vector<std::function<void()>> m_texture_rebinds;
  std::vector<std::shared_ptr<CustomParticleSystem::ParticleProps>> m_undo_stack;
  std::vector<std::shared_ptr<CustomParticleSystem::ParticleProps>> m_redo_stack;
  bool m_in_texture_tab;
  int m_texture_current;

  std::shared_ptr<CustomParticleSystem::ParticleProps> m_saved_version;

  std::unique_ptr<CustomParticleSystem> m_particles;
  std::string m_filename;

private:
  ParticleEditor(const ParticleEditor&) = delete;
  ParticleEditor& operator=(const ParticleEditor&) = delete;
};

#endif

/* EOF */
