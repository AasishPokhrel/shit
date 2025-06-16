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

#include "editor/particle_editor.hpp"

#include <physfs.h>

#include "control/input_manager.hpp"
#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_manager.hpp"
#include "gui/menu_filesystem.hpp"
#include "gui/mousecursor.hpp"
#include "math/easing.hpp"
#include "object/custom_particle_system.hpp"
#include "physfs/util.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/menu/particle_editor_save_as.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "util/file_system.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/string_util.hpp"
#include "video/compositor.hpp"


bool
ParticleEditor::is_active()
{
  return ParticleEditor::current() != nullptr;
}

bool (*ParticleEditor::m_clamp_0_1)(ControlTextboxFloat*, float) = [](ControlTextboxFloat* c, float f){
  if (f < 0.f) {
    c->set_value(0.f);
    return false;
  } else if (f > 1.f) {
    c->set_value(1.f);
    return false;
  } else {
    return true;
  }
};

ParticleEditor::ParticleEditor() :
  m_enabled(true),
  m_quit_request(false),
  m_controls(),
  m_controls_textures(),
  m_texture_rebinds(),
  m_undo_stack(),
  m_redo_stack(),
  m_in_texture_tab(),
  m_texture_current(0),
  m_saved_version(),
  m_particles(),
  m_filename("")
{
  reload();
}

void
ParticleEditor::reload()
{
  reload_particles();
  reset_main_ui();
  reset_texture_ui();

  m_redo_stack.clear();
  m_undo_stack.clear();
  m_saved_version = m_particles->get_props();
  m_undo_stack.push_back(m_saved_version);
}

ParticleEditor::~ParticleEditor()
{
}

void
ParticleEditor::reload_particles()
{
  auto doc = ReaderDocument::from_file((m_filename == "") ? "/particles/default.stcp" : m_filename);
  auto root = doc.get_root();
  auto mapping = root.get_mapping();

  if (root.get_name() != "supertux-custom-particle")
    throw std::runtime_error("file is not a supertux-custom-particle file.");

  m_particles.reset(new CustomParticleSystem(mapping));
}

void
ParticleEditor::reset_main_ui()
{
  m_controls.clear();

  // TODO: Use the addButton() command
  // Texture button start
  auto texture_btn = std::make_unique<ControlButton>(_("Change texture...  ->"));
  texture_btn.get()->m_on_change = std::function<void()>([this](){
    m_in_texture_tab = true;
  });
  float tmp_height = 0.f;
  for (const auto& control : m_controls) {
    tmp_height = std::max(tmp_height, control->get_rect().get_bottom() + 5.f);
  }
  texture_btn.get()->set_rect(Rectf(25.f, tmp_height, 325.f, tmp_height + 20.f));
  m_controls.push_back(std::move(texture_btn));
  // Texture button end

  addTextboxInt(_("Max amount"), &(m_particles->m_max_amount), [](ControlTextboxInt* ctrl, int i)
    {
      if (i < 0) {
        ctrl->set_value(0);
        return false;
      }

      if (i > 500) {
        ctrl->set_value(500);
        return false;
      }

      return true;
    }
  );
  addTextboxFloat(_("Delay"), &(m_particles->m_delay));
  addCheckbox(_("Spawn anywhere"), &(m_particles->m_cover_screen));
  addTextboxFloatWithImprecision(_("Life duration"),
                                 &(m_particles->m_particle_lifetime),
                                 &(m_particles->m_particle_lifetime_variation),
                                 [](ControlTextboxFloat* ctrl, float f){ return f > 0.f; },
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; });
  addTextboxFloatWithImprecision(_("Birth duration"),
                                 &(m_particles->m_particle_birth_time),
                                 &(m_particles->m_particle_birth_time_variation),
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; },
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; });
  addTextboxFloatWithImprecision(_("Death duration"),
                                 &(m_particles->m_particle_death_time),
                                 &(m_particles->m_particle_death_time_variation),
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; },
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; });

  auto birth_mode = std::make_unique<ControlEnum<CustomParticleSystem::FadeMode> >();
  birth_mode.get()->add_option(CustomParticleSystem::FadeMode::Shrink, _("Grow"));
  birth_mode.get()->add_option(CustomParticleSystem::FadeMode::Fade, _("Fade in"));
  birth_mode.get()->add_option(CustomParticleSystem::FadeMode::None, _("None"));
  birth_mode.get()->bind_value(&(m_particles->m_particle_birth_mode));
  addControl(_("Birth mode"), std::move(birth_mode));
  auto death_mode = std::make_unique<ControlEnum<CustomParticleSystem::FadeMode> >();
  death_mode.get()->add_option(CustomParticleSystem::FadeMode::Shrink, _("Shrink"));
  death_mode.get()->add_option(CustomParticleSystem::FadeMode::Fade, _("Fade out"));
  death_mode.get()->add_option(CustomParticleSystem::FadeMode::None, _("None"));
  death_mode.get()->bind_value(&(m_particles->m_particle_death_mode));
  addControl(_("Death mode"), std::move(death_mode));

  addEasingEnum(_("Birth easing"), &(m_particles->m_particle_birth_easing));
  addEasingEnum(_("Death easing"), &(m_particles->m_particle_death_easing));

  addTextboxFloatWithImprecision(_("Horizontal speed"),
                                 &(m_particles->m_particle_speed_x),
                                 &(m_particles->m_particle_speed_variation_x),
                                 nullptr,
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; });
  addTextboxFloatWithImprecision(_("Vertical speed"),
                                 &(m_particles->m_particle_speed_y),
                                 &(m_particles->m_particle_speed_variation_y),
                                 nullptr,
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; });
  addTextboxFloat(_("Horizontal acceleration"), &(m_particles->m_particle_acceleration_x));
  addTextboxFloat(_("Vertical acceleration"), &(m_particles->m_particle_acceleration_y));
  addTextboxFloat(_("Horizontal friction"), &(m_particles->m_particle_friction_x));
  addTextboxFloat(_("Vertical friction"), &(m_particles->m_particle_friction_y));
  addTextboxFloat(_("Feather factor"), &(m_particles->m_particle_feather_factor));
  addTextboxFloatWithImprecision(_("Initial rotation"),
                                 &(m_particles->m_particle_rotation),
                                 &(m_particles->m_particle_rotation_variation),
                                 nullptr,
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; });
  addTextboxFloatWithImprecision(_("Rotation speed"),
                                 &(m_particles->m_particle_rotation_speed),
                                 &(m_particles->m_particle_rotation_speed_variation),
                                 nullptr,
                                 [](ControlTextboxFloat* ctrl, float f){ return f >= 0.f; });
  addTextboxFloat(_("Rotation acceleration"), &(m_particles->m_particle_rotation_acceleration));
  addTextboxFloat(_("Rotation friction/decceleration"), &(m_particles->m_particle_rotation_decceleration));

  auto rotation_mode = std::make_unique<ControlEnum<CustomParticleSystem::RotationMode> >();
  rotation_mode.get()->add_option(CustomParticleSystem::RotationMode::Wiggling, _("Wiggling"));
  rotation_mode.get()->add_option(CustomParticleSystem::RotationMode::Facing, _("Facing"));
  rotation_mode.get()->add_option(CustomParticleSystem::RotationMode::Fixed, _("Fixed"));
  rotation_mode.get()->bind_value(&(m_particles->m_particle_rotation_mode));
  addControl(_("Rotation mode"), std::move(rotation_mode));

  auto collision_mode = std::make_unique<ControlEnum<CustomParticleSystem::CollisionMode> >();
  collision_mode.get()->add_option(CustomParticleSystem::CollisionMode::Destroy, _("Destroy"));
  collision_mode.get()->add_option(CustomParticleSystem::CollisionMode::FadeOut, _("Fade out"));
  collision_mode.get()->add_option(CustomParticleSystem::CollisionMode::BounceLight, _("Bounce (light)"));
  collision_mode.get()->add_option(CustomParticleSystem::CollisionMode::BounceHeavy, _("Bounce (heavy)"));
  collision_mode.get()->add_option(CustomParticleSystem::CollisionMode::Stick, _("Stick to surface"));
  collision_mode.get()->add_option(CustomParticleSystem::CollisionMode::StickForever, _("Stick and stay"));
  collision_mode.get()->add_option(CustomParticleSystem::CollisionMode::Ignore, _("No collision"));
  collision_mode.get()->bind_value(&(m_particles->m_particle_collision_mode));
  addControl(_("Collision mode"), std::move(collision_mode));

  auto offscreen_mode = std::make_unique<ControlEnum<CustomParticleSystem::OffscreenMode> >();
  offscreen_mode.get()->add_option(CustomParticleSystem::OffscreenMode::Always, _("Always destroy"));
  offscreen_mode.get()->add_option(CustomParticleSystem::OffscreenMode::OnlyOnExit, _("Only on exit"));
  offscreen_mode.get()->add_option(CustomParticleSystem::OffscreenMode::Never, _("Never"));
  offscreen_mode.get()->bind_value(&(m_particles->m_particle_offscreen_mode));
  addControl(_("Offscreen mode"), std::move(offscreen_mode));

  // TODO: add some ParticleEditor::addButton() function so that I don't have to put all that in here
  auto clear_btn = std::make_unique<ControlButton>(_("Clear"));
  clear_btn.get()->m_on_change = std::function<void()>([this](){ m_particles->clear(); });
  float height = 0.f;
  for (const auto& control : m_controls) {
    height = std::max(height, control->get_rect().get_bottom() + 5.f);
  }
  clear_btn.get()->set_rect(Rectf(25.f, height, 325.f, height + 20.f));
  m_controls.push_back(std::move(clear_btn));
}

void
ParticleEditor::reset_texture_ui()
{
  m_controls_textures.clear();
  m_texture_rebinds.clear();

  auto return_btn = std::make_unique<ControlButton>(_("<- General settings"));
  return_btn.get()->m_on_change = std::function<void()>([this](){
    m_in_texture_tab = false;
  });
  return_btn.get()->set_rect(Rectf(25.f, 0.f, 325.f, 20.f));
  m_controls_textures.push_back(std::move(return_btn));

  auto likeliness_control = std::make_unique<ControlTextboxFloat>();
  likeliness_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->likeliness));
  likeliness_control.get()->set_rect(Rectf(150.f, 50.f, 350.f, 70.f));
  likeliness_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(5.f, 50.f, 135.f, 70.f), _("Likeliness"));
  likeliness_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  auto likeliness_control_ptr = likeliness_control.get();
  m_texture_rebinds.push_back( [this, likeliness_control_ptr]{
    likeliness_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->likeliness));
  });
  m_controls_textures.push_back(std::move(likeliness_control));

  auto color_r_control = std::make_unique<ControlTextboxFloat>();
  color_r_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.red));
  color_r_control.get()->set_rect(Rectf(150.f, 80.f, 192.f, 100.f));
  color_r_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(5.f, 80.f, 140.f, 100.f), _("Color (RGBA)"));
  color_r_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  color_r_control.get()->m_validate_float = m_clamp_0_1;
  auto color_r_control_ptr = color_r_control.get();
  m_texture_rebinds.push_back( [this, color_r_control_ptr]{
    color_r_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.red));
  });
  m_controls_textures.push_back(std::move(color_r_control));

  auto color_g_control = std::make_unique<ControlTextboxFloat>();
  color_g_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.green));
  color_g_control.get()->set_rect(Rectf(202.f, 80.f, 245.f, 100.f));
  color_g_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  color_g_control.get()->m_validate_float = m_clamp_0_1;
  auto color_g_control_ptr = color_g_control.get();
  m_texture_rebinds.push_back( [this, color_g_control_ptr]{
    color_g_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.green));
  });
  m_controls_textures.push_back(std::move(color_g_control));

  auto color_b_control = std::make_unique<ControlTextboxFloat>();
  color_b_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.blue));
  color_b_control.get()->set_rect(Rectf(255.f, 80.f, 297.f, 100.f));
  color_b_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  color_b_control.get()->m_validate_float = m_clamp_0_1;
  auto color_b_control_ptr = color_b_control.get();
  m_texture_rebinds.push_back( [this, color_b_control_ptr]{
    color_b_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.blue));
  });
  m_controls_textures.push_back(std::move(color_b_control));

  auto color_a_control = std::make_unique<ControlTextboxFloat>();
  color_a_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.alpha));
  color_a_control.get()->set_rect(Rectf(307.f, 80.f, 350.f, 100.f));
  color_a_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  color_a_control.get()->m_validate_float = m_clamp_0_1;
  auto color_a_control_ptr = color_a_control.get();
  m_texture_rebinds.push_back( [this, color_a_control_ptr]{
    color_a_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->color.alpha));
  });
  m_controls_textures.push_back(std::move(color_a_control));

  auto scale_x_control = std::make_unique<ControlTextboxFloat>();
  scale_x_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->scale.x));
  scale_x_control.get()->set_rect(Rectf(150.f, 110.f, 240.f, 130.f));
  scale_x_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(5.f, 110.f, 150.f, 130.f), _("Scale (x, y)"));
  scale_x_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  auto scale_x_control_ptr = scale_x_control.get();
  m_texture_rebinds.push_back( [this, scale_x_control_ptr]{
    scale_x_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->scale.x));
  });
  m_controls_textures.push_back(std::move(scale_x_control));

  auto scale_y_control = std::make_unique<ControlTextboxFloat>();
  scale_y_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->scale.y));
  scale_y_control.get()->set_rect(Rectf(260.f, 110.f, 350.f, 130.f));
  scale_y_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  auto scale_y_control_ptr = scale_y_control.get();
  m_texture_rebinds.push_back( [this, scale_y_control_ptr]{
    scale_y_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->scale.y));
  });
  m_controls_textures.push_back(std::move(scale_y_control));

  auto hb_scale_x_control = std::make_unique<ControlTextboxFloat>();
  hb_scale_x_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_scale.x));
  hb_scale_x_control.get()->set_rect(Rectf(150.f, 140.f, 240.f, 160.f));
  hb_scale_x_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(5.f, 140.f, 150.f, 160.f), _("Hitbox scale (x, y)"));
  hb_scale_x_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  auto hb_scale_x_control_ptr = hb_scale_x_control.get();
  m_texture_rebinds.push_back( [this, hb_scale_x_control_ptr]{
    hb_scale_x_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_scale.x));
  });
  m_controls_textures.push_back(std::move(hb_scale_x_control));

  auto hb_scale_y_control = std::make_unique<ControlTextboxFloat>();
  hb_scale_y_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_scale.y));
  hb_scale_y_control.get()->set_rect(Rectf(260.f, 140.f, 350.f, 160.f));
  hb_scale_y_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  auto hb_scale_y_control_ptr = hb_scale_y_control.get();
  m_texture_rebinds.push_back( [this, hb_scale_y_control_ptr]{
    hb_scale_y_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_scale.y));
  });
  m_controls_textures.push_back(std::move(hb_scale_y_control));

  auto hb_offset_x_control = std::make_unique<ControlTextboxFloat>();
  hb_offset_x_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_offset.x));
  hb_offset_x_control.get()->set_rect(Rectf(150.f, 170.f, 240.f, 190.f));
  hb_offset_x_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(5.f, 170.f, 150.f, 190.f), _("Hitbox offset relative to scale"));
  hb_offset_x_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  auto hb_offset_x_control_ptr = hb_offset_x_control.get();
  m_texture_rebinds.push_back( [this, hb_offset_x_control_ptr]{
    hb_offset_x_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_offset.x));
  });
  m_controls_textures.push_back(std::move(hb_offset_x_control));

  auto hb_offset_y_control = std::make_unique<ControlTextboxFloat>();
  hb_offset_y_control.get()->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_offset.y));
  hb_offset_y_control.get()->set_rect(Rectf(260.f, 170.f, 350.f, 190.f));
  hb_offset_y_control.get()->m_on_change = std::function<void()>([this](){ m_particles->reinit_textures(); this->push_version(); });
  auto hb_offset_y_control_ptr = hb_offset_y_control.get();
  m_texture_rebinds.push_back( [this, hb_offset_y_control_ptr]{
    hb_offset_y_control_ptr->bind_value(&((m_particles->m_textures.begin() + m_texture_current)->hb_offset.y));
  });
  m_controls_textures.push_back(std::move(hb_offset_y_control));

  // Texture button start
  auto chg_texture_btn = std::make_unique<ControlButton>(_("Change texture..."));
  chg_texture_btn.get()->m_on_change = std::function<void()>([this](){
    const std::vector<std::string>& filter = {".jpg", ".png", ".surface"};
    MenuManager::instance().push_menu(std::make_unique<FileSystemMenu>(
      nullptr,
      filter,
      "/",
      false,
      [this](const std::string& new_filename) {
        (m_particles->m_textures.begin() + m_texture_current)->texture = Surface::from_file(new_filename);
        m_particles->reinit_textures();
        this->push_version();
      }
    ));
  });
  chg_texture_btn.get()->set_rect(Rectf(25.f, 420, 325.f, 440));
  m_controls_textures.push_back(std::move(chg_texture_btn));
  // Texture button end

  auto prev_btn = std::make_unique<ControlButton>("<");
  prev_btn.get()->m_on_change = std::function<void()>([this](){
    m_texture_current--;
    if (m_texture_current < 0) m_texture_current = 0;
    for (const auto& refresh : m_texture_rebinds)
      refresh();
  });
  prev_btn.get()->set_rect(Rectf(120.f, 450, 140.f, 470.f));
  m_controls_textures.push_back(std::move(prev_btn));

  auto del_btn = std::make_unique<ControlButton>("-");
  del_btn.get()->m_on_change = std::function<void()>([this](){
    if (m_particles->m_textures.size() < 1)
      return;
    m_particles->m_textures.erase(m_particles->m_textures.begin() + m_texture_current);
    m_texture_current--;
    if (m_texture_current < 0) m_texture_current = 0;
    for (const auto& refresh : m_texture_rebinds)
      refresh();
    m_particles->reinit_textures();
    this->push_version();
  });
  del_btn.get()->set_rect(Rectf(150.f, 450, 170.f, 470.f));
  m_controls_textures.push_back(std::move(del_btn));

  auto add_btn = std::make_unique<ControlButton>("+");
  add_btn.get()->m_on_change = std::function<void()>([this](){
    m_particles->m_textures.push_back(CustomParticleSystem::SpriteProperties());
    m_texture_current = static_cast<int>(m_particles->m_textures.size()) - 1;
    for (const auto& refresh : m_texture_rebinds)
      refresh();
    m_particles->reinit_textures();
    this->push_version();
  });
  add_btn.get()->set_rect(Rectf(190.f, 450, 210.f, 470.f));
  m_controls_textures.push_back(std::move(add_btn));

  auto next_btn = std::make_unique<ControlButton>(">");
  next_btn.get()->m_on_change = std::function<void()>([this](){
    m_texture_current++;
    if (m_texture_current > static_cast<int>(m_particles->m_textures.size()) - 1)
      m_texture_current = static_cast<int>(m_particles->m_textures.size()) - 1;
    for (const auto& refresh : m_texture_rebinds)
      refresh();
  });
  next_btn.get()->set_rect(Rectf(220.f, 450, 240.f, 470.f));
  m_controls_textures.push_back(std::move(next_btn));
}

void
ParticleEditor::addTextboxFloat(const std::string& name, float* bind,
  bool (*float_validator)(ControlTextboxFloat*, float))
{
  auto float_control = std::make_unique<ControlTextboxFloat>();
  float_control.get()->bind_value(bind);
  float_control.get()->m_validate_float = float_validator;
  addControl(name, std::move(float_control));
}

void
ParticleEditor::addTextboxFloatWithImprecision(const std::string& name, float* bind,
                                      float* imprecision_bind,
                                      bool (*float_validator)(ControlTextboxFloat*, float),
                                      bool (*imp_validator)(ControlTextboxFloat*, float))
{
  auto float_control = std::make_unique<ControlTextboxFloat>();
  float_control.get()->bind_value(bind);
  float_control.get()->m_validate_float = float_validator;

  auto imp_control = std::make_unique<ControlTextboxFloat>();
  imp_control.get()->bind_value(imprecision_bind);
  imp_control.get()->m_validate_float = imp_validator;

  // Can't use addControl() because this is a special case
  float height = 0.f;
  for (const auto& control : m_controls) {
    height = std::max(height, control->get_rect().get_bottom() + 5.f);
  }

  float_control.get()->set_rect(Rectf(150.f, height, 235.f, height + 20.f));
  imp_control.get()->set_rect(Rectf(265.f, height, 350.f, height + 20.f));

  float_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(5.f, height, 145.f, height + 20.f), name);
  imp_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(240.f, height, 260.f, height + 20.f), "±");

  float_control.get()->m_on_change = std::function<void()>([this](){ this->push_version(); });
  imp_control.get()->m_on_change = std::function<void()>([this](){ this->push_version(); });

  m_controls.push_back(std::move(float_control));
  m_controls.push_back(std::move(imp_control));
}

void
ParticleEditor::addTextboxInt(const std::string& name, int* bind, bool (*int_validator)(ControlTextboxInt*, int))
{
  auto int_control = std::make_unique<ControlTextboxInt>();
  int_control.get()->bind_value(bind);
  int_control.get()->m_validate_int = int_validator;
  addControl(name, std::move(int_control));
}

void
ParticleEditor::addCheckbox(std::string name, bool* bind)
{
  auto bool_control = std::make_unique<ControlCheckbox>();
  bool_control.get()->bind_value(bind);
  bool_control.get()->set_rect(Rectf(240.f, 0.f, 260.f, 20.f));
  addControl(std::move(name), std::move(bool_control));
}

void
ParticleEditor::addEasingEnum(std::string name, EasingMode* bind)
{
  // FIXME: They don't render in the same order as in here
  auto ease_ctrl = std::make_unique<ControlEnum<EasingMode> >();
  ease_ctrl.get()->add_option(EaseNone, _(getEasingName(EaseNone)));
  ease_ctrl.get()->add_option(EaseQuadIn, _(getEasingName(EaseQuadIn)));
  ease_ctrl.get()->add_option(EaseQuadOut, _(getEasingName(EaseQuadOut)));
  ease_ctrl.get()->add_option(EaseQuadInOut, _(getEasingName(EaseQuadInOut)));
  ease_ctrl.get()->add_option(EaseCubicIn, _(getEasingName(EaseCubicIn)));
  ease_ctrl.get()->add_option(EaseCubicOut, _(getEasingName(EaseCubicOut)));
  ease_ctrl.get()->add_option(EaseCubicInOut, _(getEasingName(EaseCubicInOut)));
  ease_ctrl.get()->add_option(EaseQuartIn, _(getEasingName(EaseQuartIn)));
  ease_ctrl.get()->add_option(EaseQuartOut, _(getEasingName(EaseQuartOut)));
  ease_ctrl.get()->add_option(EaseQuartInOut, _(getEasingName(EaseQuartInOut)));
  ease_ctrl.get()->add_option(EaseQuintIn, _(getEasingName(EaseQuintIn)));
  ease_ctrl.get()->add_option(EaseQuintOut, _(getEasingName(EaseQuintOut)));
  ease_ctrl.get()->add_option(EaseQuintInOut, _(getEasingName(EaseQuintInOut)));
  ease_ctrl.get()->add_option(EaseSineIn, _(getEasingName(EaseSineIn)));
  ease_ctrl.get()->add_option(EaseSineOut, _(getEasingName(EaseSineOut)));
  ease_ctrl.get()->add_option(EaseSineInOut, _(getEasingName(EaseSineInOut)));
  ease_ctrl.get()->add_option(EaseCircularIn, _(getEasingName(EaseCircularIn)));
  ease_ctrl.get()->add_option(EaseCircularOut, _(getEasingName(EaseCircularOut)));
  ease_ctrl.get()->add_option(EaseCircularInOut, _(getEasingName(EaseCircularInOut)));
  ease_ctrl.get()->add_option(EaseExponentialIn, _(getEasingName(EaseExponentialIn)));
  ease_ctrl.get()->add_option(EaseExponentialOut, _(getEasingName(EaseExponentialOut)));
  ease_ctrl.get()->add_option(EaseExponentialInOut, _(getEasingName(EaseExponentialInOut)));
  ease_ctrl.get()->add_option(EaseElasticIn, _(getEasingName(EaseElasticIn)));
  ease_ctrl.get()->add_option(EaseElasticOut, _(getEasingName(EaseElasticOut)));
  ease_ctrl.get()->add_option(EaseElasticInOut, _(getEasingName(EaseElasticInOut)));
  ease_ctrl.get()->add_option(EaseBackIn, _(getEasingName(EaseBackIn)));
  ease_ctrl.get()->add_option(EaseBackOut, _(getEasingName(EaseBackOut)));
  ease_ctrl.get()->add_option(EaseBackInOut, _(getEasingName(EaseBackInOut)));
  ease_ctrl.get()->add_option(EaseBounceIn, _(getEasingName(EaseBounceIn)));
  ease_ctrl.get()->add_option(EaseBounceOut, _(getEasingName(EaseBounceOut)));
  ease_ctrl.get()->add_option(EaseBounceInOut, _(getEasingName(EaseBounceInOut)));
  ease_ctrl.get()->bind_value(bind);
  addControl(std::move(name), std::move(ease_ctrl));
}

void
ParticleEditor::addControl(std::string name, std::unique_ptr<InterfaceControl> new_control)
{
  float height = 0.f;
  for (const auto& control : m_controls) {
    height = std::max(height, control->get_rect().get_bottom() + 5.f);
  }

  if (new_control.get()->get_rect().get_width() == 0.f || new_control.get()->get_rect().get_height() == 0.f) {
    new_control.get()->set_rect(Rectf(150.f, height, 350.f, height + 20.f));
  } else {
    new_control.get()->set_rect(Rectf(new_control.get()->get_rect().get_left(),
                                      height,
                                      new_control.get()->get_rect().get_right(),
                                      height + new_control.get()->get_rect().get_height()));
  }

  new_control.get()->m_label = std::make_unique<InterfaceLabel>(Rectf(5.f, height, 135.f, height + 20.f), std::move(name));
  new_control.get()->m_on_change = std::function<void()>([this](){ this->push_version(); });
  m_controls.push_back(std::move(new_control));
}


void
ParticleEditor::save(const std::string& filepath_, bool retry)
{
  std::string filepath = filepath_;
  if (!StringUtil::has_suffix(filepath, ".stcp"))
    filepath += ".stcp";

  // FIXME: It tests for directory in supertux/data, but saves into .supertux2.
  //  Note: I remember writing this but I have no clue what I meant.  ~Semphris
  try {
    { // make sure the level directory exists
      std::string dirname = FileSystem::dirname(filepath);
      if (!PHYSFS_exists(dirname.c_str()))
      {
        if (!PHYSFS_mkdir(dirname.c_str()))
        {
          std::ostringstream msg;
          msg << "Couldn't create directory for particle config '"
              << dirname << "': " <<physfsutil::get_last_error();
          throw std::runtime_error(msg.str());
        }
      }

      if (!physfsutil::is_directory(dirname))
      {
        std::ostringstream msg;
        msg << "Particle config path '" << dirname << "' is not a directory";
        throw std::runtime_error(msg.str());
      }
    }

    Writer writer(filepath);
    save(writer);
    m_filename = filepath;
    m_saved_version = m_undo_stack.back();
    log_warning << "Particle config saved as " << filepath << "." << std::endl;
  } catch(std::exception& e) {
    if (retry) {
      std::stringstream msg;
      msg << "Problem when saving particle config '" << filepath << "': " << e.what();
      throw std::runtime_error(msg.str());
    } else {
      log_warning << "Failed to save the particle configuration, retrying..." << std::endl;
      { // create the level directory again
        std::string dirname = FileSystem::dirname(filepath);
        if (!PHYSFS_mkdir(dirname.c_str()))
        {
          std::ostringstream msg;
          msg << "Couldn't create directory for particle config '"
              << dirname << "': " <<physfsutil::get_last_error();
          throw std::runtime_error(msg.str());
        }
      }
      save(filepath, true);
    }
  }
}

void
ParticleEditor::save(Writer& writer)
{
  writer.start_list("supertux-custom-particle");
  // Starts writing to supertux particle file. Keep this at the very beginning.

  m_particles->save(writer);

  // Ends writing to supertux particle file. Keep this at the very end.
  writer.end_list("supertux-custom-particle");
}

void
ParticleEditor::request_save(bool is_save_as,
  const std::function<void(bool)>& callback)
{
  if (is_save_as || m_filename == "")
  {
    m_enabled = false;
    MenuManager::instance().set_menu(std::make_unique<ParticleEditorSaveAs>(callback));
  }
  else
  {
    save(m_filename);
    callback(true);
  }
}

void
ParticleEditor::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();

  m_particles->draw(context);

  context.color().draw_gradient(Color(0.05f, 0.1f, 0.1f, 1.f),
                                Color(0.1f, 0.15f, 0.15f, 1.f),
                                LAYER_GUI - 1,
                                GradientDirection::HORIZONTAL,
                                Rectf(0.f, 0.f, 355.f, float(context.get_height())));

  if (m_in_texture_tab)
  {
    context.color().draw_surface_scaled((m_particles->m_textures.begin() + m_texture_current)->texture,
                                         Rect(75, 200, 275, 400), LAYER_GUI);
    context.color().draw_text(Resources::control_font,
                              std::to_string(m_texture_current + 1) + "/"
                                  + std::to_string(m_particles->m_textures.size()),
                              Vector(175, 30),
                              FontAlignment::ALIGN_CENTER,
                              LAYER_GUI);
    for(const auto& control : m_controls_textures)
    {
      control->draw(context);
    }
  }
  else
  {
    for(const auto& control : m_controls)
    {
      control->draw(context);
    }
  }

  MouseCursor::current()->draw(context);
}

void
ParticleEditor::update(float dt_sec, const Controller& controller)
{

  update_keyboard(controller);

  if (m_quit_request) {
    quit_editor();
  }

  if (m_in_texture_tab) {
    for(const auto& control : m_controls_textures) {
      control->update(dt_sec);
    }
  } else {
    for(const auto& control : m_controls) {
      control->update(dt_sec);
    }
  }

  // Uncomment to make the particles stop updating on pause
  //if (!m_enabled)
  //  return;

  m_particles->update(dt_sec);
}

void
ParticleEditor::update_keyboard(const Controller& controller)
{
  if (!m_enabled)
    return;

  if (controller.pressed(Control::ESCAPE)) {
    m_enabled = false;
    MenuManager::instance().set_menu(MenuStorage::PARTICLE_EDITOR_MENU);
    return;
  }
}

void
ParticleEditor::quit_editor()
{
  m_quit_request = false;

  auto quit = [] ()
  {
    ScreenManager::current()->pop_screen();
    if (Editor::current()) {
      Editor::current()->m_reactivate_request = true;
    }
  };

  check_unsaved_changes([quit] {
    quit();
  });
}

void
ParticleEditor::leave()
{
  m_enabled = false;
}

void
ParticleEditor::setup()
{
  m_enabled = true;
}

void
ParticleEditor::check_unsaved_changes(const std::function<void ()>& action)
{
  if (m_undo_stack.back() != m_saved_version)
  {
    m_enabled = false;
    auto dialog = std::make_unique<Dialog>();
    dialog->set_text(_("This particle configuration contains unsaved changes,\ndo you want to save?"));
    dialog->add_button(_("Save"), [this, action] {
      request_save(false, [this, action] (bool was_saved) {
        m_enabled = true;
        if (was_saved)
          action();
      });
    });
    dialog->add_default_button(_("Save as"), [this, action] {
      request_save(true, [this, action] (bool was_saved) {
        m_enabled = true;
        if (was_saved)
          action();
      });
    });
    dialog->add_button(_("No"), [this, action] {
      action();
      m_enabled = true;
    });
    dialog->add_button(_("Cancel"), [this] {
      m_enabled = true;
    });
    MenuManager::instance().set_dialog(std::move(dialog));
  }
  else
  {
    action();
  }
}

void
ParticleEditor::reactivate()
{
  m_enabled = true;
}

void
ParticleEditor::open_particle_directory()
{
  auto path = FileSystem::join(PHYSFS_getWriteDir(), "/particles/custom/");
  FileSystem::open_path(path);
}

void
ParticleEditor::event(const SDL_Event& ev)
{
  if (!m_enabled) return;

  if (m_in_texture_tab) {
    for(const auto& control : m_controls_textures)
      if (control->event(ev))
        break;
  } else {
    for(const auto& control : m_controls)
      if (control->event(ev))
        break;
  }

  if (ev.type == SDL_KEYDOWN &&
      ev.key.keysym.sym == SDLK_z &&
      ev.key.keysym.mod & KMOD_CTRL) {
    undo();
  }

  if (ev.type == SDL_KEYDOWN &&
      ev.key.keysym.sym == SDLK_y &&
     ev.key.keysym.mod & KMOD_CTRL) {
    redo();
  }

  if (ev.type == SDL_KEYDOWN &&
      ev.key.keysym.sym == SDLK_s &&
      ev.key.keysym.mod & KMOD_CTRL) {
    request_save(ev.key.keysym.mod & KMOD_SHIFT);
  }

  if (ev.type == SDL_KEYDOWN &&
      ev.key.keysym.sym == SDLK_o &&
     ev.key.keysym.mod & KMOD_CTRL) {
    MenuManager::instance().set_menu(MenuStorage::PARTICLE_EDITOR_OPEN);
  }
}

void
ParticleEditor::push_version()
{
  m_redo_stack.clear();
  m_undo_stack.push_back(m_particles->get_props());
}

void
ParticleEditor::undo()
{
  if (m_undo_stack.size() < 2)
    return;

  m_redo_stack.push_back(m_undo_stack.back());
  m_undo_stack.pop_back();
  m_particles->set_props(m_undo_stack.back().get());

  m_particles->reinit_textures();
  if (m_texture_current > static_cast<int>(m_particles->m_textures.size()) - 1)
    m_texture_current = static_cast<int>(m_particles->m_textures.size()) - 1;
}

void
ParticleEditor::redo()
{
  if (m_redo_stack.size() < 1)
    return;

  m_undo_stack.push_back(m_redo_stack.back());
  m_particles->set_props(m_redo_stack.back().get());
  m_redo_stack.pop_back();

  m_particles->reinit_textures();
  if (m_texture_current > static_cast<int>(m_particles->m_textures.size()) - 1)
    m_texture_current = static_cast<int>(m_particles->m_textures.size()) - 1;
}

IntegrationStatus
ParticleEditor::get_status() const
{
  IntegrationStatus status;
  status.m_details.push_back("Editing particles");
  return status;
}

/* EOF */
