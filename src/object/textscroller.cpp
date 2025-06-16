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

#include "object/textscroller.hpp"

#include <optional>
#include <sexp/value.hpp>

#include "control/input_manager.hpp"
#include "editor/editor.hpp"
#include "supertux/globals.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/info_box_line.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

const float LEFT_BORDER = 0;
const float DEFAULT_SPEED = 60;
const float SCROLL_JUMP = 60;

} // namespace

TextScroller::TextScroller(const ReaderMapping& mapping) :
  LayerObject(mapping),
  controller(&InputManager::current()->get_controller()),
  m_filename(),
  m_finish_script(),
  m_lines(),
  m_scroll(),
  m_default_speed(DEFAULT_SPEED),
  m_x_offset(),
  m_controllable(true),
  m_finished(),
  m_fading(),
  m_x_anchor(XAnchor::SCROLLER_ANCHOR_CENTER),
  m_text_align(TextAlign::SCROLLER_ALIGN_CENTER)
{
  mapping.get("file", m_filename);

  if (!Editor::is_active())
  {
    if (m_filename.empty())
      throw std::runtime_error("Cannot load text scroller: No file specified!");

    parse_file(m_filename);
  }

  mapping.get("finish-script", m_finish_script, "");
  mapping.get("speed", m_default_speed);
  mapping.get("x-offset", m_x_offset);
  mapping.get("controllable", m_controllable, true);

  std::string x_anchor_str;
  if (mapping.get("x-anchor", x_anchor_str))
  {
    if (x_anchor_str == "left")
      m_x_anchor = XAnchor::SCROLLER_ANCHOR_LEFT;
    else if (x_anchor_str == "right")
      m_x_anchor = XAnchor::SCROLLER_ANCHOR_RIGHT;
    else
      m_x_anchor = XAnchor::SCROLLER_ANCHOR_CENTER;
  }

  std::string text_align_str;
  if (mapping.get("text-align", text_align_str))
  {
    if (text_align_str == "left")
      m_text_align = TextAlign::SCROLLER_ALIGN_LEFT;
    else if (text_align_str == "right")
      m_text_align = TextAlign::SCROLLER_ALIGN_RIGHT;
    else
      m_text_align = TextAlign::SCROLLER_ALIGN_CENTER;
  }

}

TextScroller::TextScroller(const ReaderObject& root) :
  controller(&InputManager::current()->get_controller()),
  m_filename(),
  m_finish_script(),
  m_lines(),
  m_scroll(),
  m_default_speed(DEFAULT_SPEED),
  m_x_offset(),
  m_controllable(true),
  m_finished(),
  m_fading(),
  m_x_anchor(XAnchor::SCROLLER_ANCHOR_CENTER),
  m_text_align(TextAlign::SCROLLER_ALIGN_CENTER)
{
  parse_root(root);
}

void
TextScroller::parse_file(const std::string& filename)
{
  register_translation_directory(filename);
  auto doc = ReaderDocument::from_file(filename);
  auto root = doc.get_root();
  parse_root(root);
}

void
TextScroller::parse_root(const ReaderObject& root)
{
  if (root.get_name() != "supertux-text")
  {
    throw std::runtime_error("File isn't a supertux-text file");
  }
  else
  {
    auto mapping = root.get_mapping();

    int version = 1;
    mapping.get("version", version);
    if (version == 1)
    {
      log_info << "[" << mapping.get_doc().get_filename() << "] Text uses old format: version 1" << std::endl;

      std::string text;
      if (!mapping.get("text", text)) {
        throw std::runtime_error("File doesn't contain a text field");
      }

      // Split text string lines into a vector
      m_lines = InfoBoxLine::split(text, static_cast<float>(SCREEN_WIDTH) - 2.0f * LEFT_BORDER);
    }
    else if (version == 2)
    {
      std::optional<ReaderCollection> content_collection;
      if (!mapping.get("content", content_collection)) {
        throw std::runtime_error("File doesn't contain content");
      } else {
        parse_content(*content_collection);
      }
    }
  }
}

void
TextScroller::parse_content(const ReaderCollection& collection)
{
  for (const auto& item : collection.get_objects())
  {
    if (item.get_name() == "image")
    {
      std::string image_file = item.get_sexp().as_array()[1].as_string();
      m_lines.emplace_back(new InfoBoxLine('!', image_file));
    }
    else if (item.get_name() == "person")
    {
      bool simple;
      std::string name, info, image_file;

      item.get_mapping().get("simple", simple, false);

      if (simple) {
        if (!item.get_mapping().get("name", name) || !item.get_mapping().get("info", info)) {
          throw std::runtime_error("Simple entry requires both name and info specified");
        }

        if (item.get_mapping().get("image", image_file)) {
          log_warning << "[" << collection.get_doc().get_filename() << "] Simple person entry shouldn't specify images" << std::endl;
        }

        m_lines.emplace_back(new InfoBoxLine(' ', name + " (" + info + ")")); // NOLINT
      } else {
        if (item.get_mapping().get("name", name)) {
          m_lines.emplace_back(new InfoBoxLine('\t', name));
        }

        if (item.get_mapping().get("image", image_file)) {
          m_lines.emplace_back(new InfoBoxLine('!', image_file));
        }

        if (item.get_mapping().get("info", info)) {
          m_lines.emplace_back(new InfoBoxLine(' ', info));
        }
      }
    }
    else if (item.get_name() == "blank")
    {
      // Empty line
      m_lines.emplace_back(new InfoBoxLine('\t', ""));
    }
    else if (item.get_name() == "text")
    {
      std::string type, string;

      if (!item.get_mapping().get("type", type)) {
        type = "normal";
      }

      if (!item.get_mapping().get("string", string)) {
        throw std::runtime_error("Text entry requires a string");
      }

      if (type == "normal")
        m_lines.emplace_back(new InfoBoxLine('\t', string));
      else if (type == "normal-left")
        m_lines.emplace_back(new InfoBoxLine('#', string));
      else if (type == "small")
        m_lines.emplace_back(new InfoBoxLine(' ', string));
      else if (type == "heading")
        m_lines.emplace_back(new InfoBoxLine('-', string));
      else if (type == "reference")
        m_lines.emplace_back(new InfoBoxLine('*', string));
      else {
        log_warning << "[" << item.get_doc().get_filename() << "] Unknown text type '" << type << "'" << std::endl;
        m_lines.emplace_back(new InfoBoxLine('\t', string));
      }
    }
    else
    {
      log_warning << "[" << item.get_doc().get_filename() << "] Unknown token '" << item.get_name() << "'" << std::endl;
    }
  }
}

void
TextScroller::draw(DrawingContext& context)
{
  context.push_transform();
  context.set_translation(Vector(0, 0));
  context.transform().scale = 1.f;

  const float ctx_w = context.get_width();
  const float ctx_h = context.get_height();

  float y = floorf(ctx_h - m_scroll);

  { // draw text
    for (const auto& line : m_lines)
    {
      if (y + line->get_height() >= 0 && ctx_h - y >= 0) {
        line->draw(context, Rectf(LEFT_BORDER, y, ctx_w * (m_x_anchor == XAnchor::SCROLLER_ANCHOR_LEFT ? 0.f :
           m_x_anchor == XAnchor::SCROLLER_ANCHOR_RIGHT ? 2.f : 1.f) + m_x_offset, y), LAYER_GUI,
          (m_text_align == TextAlign::SCROLLER_ALIGN_LEFT ? line->LineAlignment::LEFT :
           m_text_align == TextAlign::SCROLLER_ALIGN_RIGHT ? line->LineAlignment::RIGHT :
           line->LineAlignment::CENTER));
      }

      y += floorf(line->get_height());
    }
  }

  context.pop_transform();

  // close when done
  if (y < 0)
  {
    m_finished = true;
    set_default_speed(0.f);
  }
}

void
TextScroller::update(float dt_sec)
{
  float speed = m_default_speed;

  if (controller && m_controllable) {
    // allow changing speed with up and down keys
    if (controller->hold(Control::UP)) {
      speed = -m_default_speed * 5;
    } else if (controller->hold(Control::DOWN)) {
      speed = m_default_speed * 5;
    }

    // allow jumping ahead with certain keys
    if (controller->pressed_any(Control::JUMP, Control::ACTION, Control::MENU_SELECT) &&
        !(controller->pressed(Control::UP))) { // prevent skipping if jump with up is enabled
      scroll(SCROLL_JUMP);
    }

    // use start or escape keys to exit
    if (controller->pressed_any(Control::START, Control::ESCAPE) &&
        !m_fading  && m_finish_script.empty()) {
      m_fading = true;
      ScreenManager::current()->pop_screen(std::make_unique<FadeToBlack>(FadeToBlack::FADEOUT, 0.5f));
      return;
    }
  }

  m_scroll += speed * dt_sec;

  if (m_scroll < 0)
    m_scroll = 0;
  if (!m_finish_script.empty())
  {
    Sector::get().run_script(m_finish_script, "finishscript");
  }
  else
  {
    // close when done
    if (m_finished && !m_fading)
    {
	  m_fading = true;
      ScreenManager::current()->pop_screen(std::unique_ptr<ScreenFade>(new FadeToBlack(FadeToBlack::FADEOUT, 0.25f)));
    }
  }
}

void
TextScroller::scroll(float offset)
{
  m_scroll += offset;
  if (m_scroll < 0.0f)
  {
    m_scroll = 0.0f;
  }
}

ObjectSettings
TextScroller::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_file(_("File"), &m_filename, "file");
  result.add_script(_("Finish Script"), &m_finish_script, "finish-script");
  result.add_float(_("Speed"), &m_default_speed, "speed", DEFAULT_SPEED);
  result.add_float(_("X-offset"), &m_x_offset, "x-offset");
  result.add_bool(_("Controllable"), &m_controllable, "controllable", true);
  result.add_enum(_("Anchor"), reinterpret_cast<int*>(&m_x_anchor),
    { _("Left"), _("Center"), _("Right") },
    { "left", "center", "right" },
    static_cast<int>(XAnchor::SCROLLER_ANCHOR_CENTER), "x-anchor");
  result.add_enum(_("Text Alignment"), reinterpret_cast<int*>(&m_text_align),
    { _("Left"), _("Center"), _("Right") },
    { "left", "center", "right" },
    static_cast<int>(TextAlign::SCROLLER_ALIGN_CENTER), "text-align");
  result.add_remove();

  return result;
}

/* EOF */
