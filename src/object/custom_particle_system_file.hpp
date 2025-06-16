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

#ifndef HEADER_SUPERTUX_OBJECT_CUSTOM_PARTICLE_SYSTEM_FILE_HPP
#define HEADER_SUPERTUX_OBJECT_CUSTOM_PARTICLE_SYSTEM_FILE_HPP

#include "math/easing.hpp"
#include "math/vector.hpp"
#include "object/custom_particle_system.hpp"
#include "object/particlesystem_interactive.hpp"
#include "object/particle_zone.hpp"
#include "video/surface.hpp"
#include "video/surface_ptr.hpp"

class CustomParticleSystemFile final : public CustomParticleSystem
{
  friend class ParticleEditor;

public:
  CustomParticleSystemFile();
  CustomParticleSystemFile(const ReaderMapping& reader);
  ~CustomParticleSystemFile() override;

  static std::string class_name() { return "particles-custom-file"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Custom Particles from file"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return CustomParticleSystem::get_class_types().add(typeid(CustomParticleSystemFile)); }
  virtual ObjectSettings get_settings() override;

  virtual const std::string get_icon_path() const override {
    return "images/engine/editor/particle_file.png";
  }

private:
  void update_data();

private:
  std::string m_filename;

private:
  CustomParticleSystemFile(const CustomParticleSystemFile&) = delete;
  CustomParticleSystemFile& operator=(const CustomParticleSystemFile&) = delete;
};

#endif

/* EOF */
