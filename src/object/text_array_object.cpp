//  SuperTux
//  Copyright (C) 2018 Nir <goproducti@gmail.com>
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

#include "object/text_array_object.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "control/input_manager.hpp"
#include "supertux/sector.hpp"

TextArrayObject::TextArrayObject(const std::string& name) :
  LayerObject(name),
  m_finished(false),
  m_is_auto(false),
  m_keep_visible(false),
  m_fade_transition(true),
  m_fadetime(1.0),
  m_texts(),
  m_curTextIndex(0),
  m_lastTextIndex(0),
  m_waiting()
{
}

void
TextArrayObject::clear()
{
  m_texts.clear();
  reset_automation();
}

void
TextArrayObject::add_text(const std::string& text)
{
  add_text_duration(text, 3.f);
}

void
TextArrayObject::add_text_duration(const std::string& text, float duration)
{
  auto pText = std::make_unique<TextArrayItem>();
  assert(pText);

  pText->duration = duration;
  pText->text_object.set_text(text);

  m_texts.push_back(std::move(pText));
}

void
TextArrayObject::set_text_index(ta_index index)
{
  if (index < m_texts.size())
    m_curTextIndex = index;
}

void
TextArrayObject::next_text()
{
  if (m_finished)
    return;

  if (m_curTextIndex + 1 >= m_texts.size()) {
    m_finished = true;
    return;
  }

  m_lastTextIndex = m_curTextIndex++;

  override_properties();
  reset_automation();
}

void
TextArrayObject::prev_text()
{
  if (m_finished)
    return;

  if (m_curTextIndex == 0)
    return;

  m_lastTextIndex = m_curTextIndex--;

  override_properties();
  reset_automation();
}

TextArrayItem*
TextArrayObject::get_text_item(ta_index index) const
{
  auto vecSize = m_texts.size();

  if (vecSize == 0 || index >= vecSize)
    return nullptr;

  return m_texts.at(index).get();
}

TextArrayItem*
TextArrayObject::get_current_text_item() const
{
  return get_text_item(m_curTextIndex);
}

TextArrayItem*
TextArrayObject::get_last_text_item() const
{
  return get_text_item(m_lastTextIndex);
}

void
TextArrayObject::set_auto(bool is_auto)
{
  m_is_auto = is_auto;
  reset_automation();
}

void
TextArrayObject::update(float dt_sec)
{
  if (m_finished)
    return;

  // make sure there's anything to update
  if (m_texts.size() == 0)
    return;

  // detect change request
  handle_input_requests();

  // check if if should update auto narration
  if (m_is_auto && m_waiting.check()) {
    next_text();
  }

  // update current
  auto* curTextItem = get_current_text_item();
  if (curTextItem)
    curTextItem->text_object.update(dt_sec);

  // update last (if transition is enabled)

  if (should_fade()) {
    auto* lastTextItem = get_last_text_item();
    if (lastTextItem)
      lastTextItem->text_object.update(dt_sec);
  }
}

void
TextArrayObject::draw(DrawingContext& context)
{
  if (m_finished)
    return;

  auto* curTextItem = get_current_text_item();
  if (!curTextItem)
    return;

  // draw last if transition enabled
  if (should_fade()) {
    auto* lastTextItem = get_last_text_item();
    if (lastTextItem)
      lastTextItem->text_object.draw(context);
  }

  // draw current
  curTextItem->text_object.draw(context);
}

void
TextArrayObject::override_properties()
{
  if (!(should_fade() || m_keep_visible))
    return;

  auto* curTextItem = get_current_text_item();
  if (!curTextItem)
    return;

  // apply overrides
  if (should_fade()) { // make fade transition
    auto* lastTextItem = get_last_text_item();
    if (lastTextItem) {
      lastTextItem->text_object.fade_out(m_fadetime);
      curTextItem->text_object.fade_in(m_fadetime);
    }
  } else if (m_keep_visible) { // keep visible
    curTextItem->text_object.set_visible(true);
  }
}

void
TextArrayObject::reset_automation()
{
  m_waiting.stop();

  if (m_is_auto) {
    auto* text = get_current_text_item();
    if (text)
      m_waiting.start(text->duration);
  }
}

void
TextArrayObject::handle_input_requests()
{
  const Controller& controller = InputManager::current()->get_controller();

  if (controller.pressed(Control::MENU_SELECT)) {
    m_is_auto = false;
    next_text();
  } else if (controller.pressed(Control::REMOVE)) {
    m_is_auto = false;
    prev_text();
  }
}

bool
TextArrayObject::should_fade() const
{
  return m_fade_transition && (m_curTextIndex != m_lastTextIndex);
}


/** TextObject functions */

#define TEXT_OBJECT_FUNCTION_VOID(F)              \
  TextArrayItem* item = get_current_text_item();  \
  if (item) {                                     \
    item->text_object.F;                          \
    return;                                       \
  }

#define TEXT_OBJECT_FUNCTION_RETURN(F)            \
  TextArrayItem* item = get_current_text_item();  \
  if (item)                                       \
    return item->text_object.F;

#define TEXT_OBJECT_FUNCTION_VOID_THROW(F)                              \
  TEXT_OBJECT_FUNCTION_VOID(F)                                          \
  throw std::runtime_error("No current TextObject in TextArrayObject!");

#define TEXT_OBJECT_FUNCTION_RETURN_THROW(F)                            \
  TEXT_OBJECT_FUNCTION_RETURN(F)                                        \
  throw std::runtime_error("No current TextObject in TextArrayObject!");

void
TextArrayObject::set_text(const std::string& text)
{
  TEXT_OBJECT_FUNCTION_VOID(set_text(text))

  add_text(text);
}

const std::string&
TextArrayObject::get_text() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_text())
}

void
TextArrayObject::set_font(const std::string& fontname)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_font(fontname))
}

void
TextArrayObject::fade_in(float fadetime)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(fade_in(fadetime))
}

void
TextArrayObject::fade_out(float fadetime)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(fade_out(fadetime))
}

void
TextArrayObject::grow_in(float fadetime)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(grow_in(fadetime))
}

void
TextArrayObject::grow_out(float fadetime)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(grow_out(fadetime))
}

void
TextArrayObject::set_visible(bool visible)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_visible(visible))
}

bool
TextArrayObject::get_visible() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_visible())
}

void
TextArrayObject::set_centered(bool centered)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_centered(centered))
}

bool
TextArrayObject::get_centered() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_centered())
}

void
TextArrayObject::set_pos(float x, float y)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_pos(Vector(x, y)))
}

float
TextArrayObject::get_x() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_pos().x)
}

float
TextArrayObject::get_y() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_pos().y)
}

void
TextArrayObject::set_anchor_point(int anchor)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_anchor_point(static_cast<AnchorPoint>(anchor)))
}

int
TextArrayObject::get_anchor_point() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_anchor_point())
}

void
TextArrayObject::set_anchor_offset(float x, float y)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_anchor_offset(x, y))
}

float
TextArrayObject::get_wrap_width() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_wrap_width())
}

void
TextArrayObject::set_wrap_width(float width)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_wrap_width(width))
}

void
TextArrayObject::set_front_fill_color(float red, float green, float blue, float alpha)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_front_fill_color(red, green, blue, alpha))
}

void
TextArrayObject::set_back_fill_color(float red, float green, float blue, float alpha)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_back_fill_color(red, green, blue, alpha))
}

void
TextArrayObject::set_text_color(float red, float green, float blue, float alpha)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_text_color(red, green, blue, alpha))
}

void
TextArrayObject::set_roundness(float roundness)
{
  TEXT_OBJECT_FUNCTION_VOID_THROW(set_roundness(roundness))
}

float
TextArrayObject::get_roundness() const
{
  TEXT_OBJECT_FUNCTION_RETURN_THROW(get_roundness())
}

#undef TEXT_OBJECT_FUNCTION_VOID
#undef TEXT_OBJECT_FUNCTION_RETURN
#undef TEXT_OBJECT_FUNCTION_VOID_THROW
#undef TEXT_OBJECT_FUNCTION_RETURN_THROW


void
TextArrayObject::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addClass("TextArrayObject", []()
    {
      if (!Sector::current())
        throw std::runtime_error("Tried to create 'TextArrayObject' without an active sector.");

      return &Sector::get().add<TextArrayObject>();
    },
    {},
    false /* Do not free pointer from Squirrel */,
    vm.findClass("GameObject"));

  cls.addFunc("clear", &TextArrayObject::clear);
  cls.addFunc("add_text", &TextArrayObject::add_text);
  cls.addFunc("add_text_duration", &TextArrayObject::add_text_duration);
  cls.addFunc("set_text_index", &TextArrayObject::set_text_index);
  cls.addFunc("set_keep_visible", &TextArrayObject::set_keep_visible);
  cls.addFunc("set_fade_transition", &TextArrayObject::set_fade_transition);
  cls.addFunc("set_fade_time", &TextArrayObject::set_fade_time);
  cls.addFunc("set_done", &TextArrayObject::set_done);
  cls.addFunc("set_auto", &TextArrayObject::set_auto);
  cls.addFunc("next_text", &TextArrayObject::next_text);
  cls.addFunc("prev_text", &TextArrayObject::prev_text);

  cls.addVar("keep_visible", &TextArrayObject::m_keep_visible);
  cls.addVar("fade_transition", &TextArrayObject::m_fade_transition);
  cls.addVar("finished", &TextArrayObject::m_finished);

  /* TextObject API related */
  cls.addFunc("set_text", &TextArrayObject::set_text);
  cls.addFunc("get_text", &TextArrayObject::get_text);
  cls.addFunc("set_font", &TextArrayObject::set_font);
  cls.addFunc("fade_in", &TextArrayObject::fade_in);
  cls.addFunc("fade_out", &TextArrayObject::fade_out);
  cls.addFunc("grow_in", &TextArrayObject::grow_in);
  cls.addFunc("grow_out", &TextArrayObject::grow_out);
  cls.addFunc("set_visible", &TextArrayObject::set_visible);
  cls.addFunc("get_visible", &TextArrayObject::get_visible);
  cls.addFunc("set_centered", &TextArrayObject::set_centered);
  cls.addFunc("get_centered", &TextArrayObject::get_centered);
  cls.addFunc("set_pos", &TextArrayObject::set_pos);
  cls.addFunc("get_x", &TextArrayObject::get_x);
  cls.addFunc("get_y", &TextArrayObject::get_y);
  cls.addFunc("get_pos_x", &TextArrayObject::get_x); // Deprecated
  cls.addFunc("get_pos_y", &TextArrayObject::get_y); // Deprecated
  cls.addFunc("set_anchor_point", &TextArrayObject::set_anchor_point);
  cls.addFunc("get_anchor_point", &TextArrayObject::get_anchor_point);
  cls.addFunc("set_anchor_offset", &TextArrayObject::set_anchor_offset);
  cls.addFunc("get_wrap_width", &TextArrayObject::get_wrap_width);
  cls.addFunc("set_wrap_width", &TextArrayObject::set_wrap_width);
  cls.addFunc("set_front_fill_color", &TextArrayObject::set_front_fill_color);
  cls.addFunc("set_back_fill_color", &TextArrayObject::set_back_fill_color);
  cls.addFunc("set_text_color", &TextArrayObject::set_text_color);
  cls.addFunc("set_roundness", &TextArrayObject::set_roundness);
  cls.addFunc("get_roundness", &TextArrayObject::set_roundness);

  cls.addVar("text", &TextArrayObject::get_text, &TextArrayObject::set_text);
  cls.addVar("visible", &TextArrayObject::get_visible, &TextArrayObject::set_visible);
  cls.addVar("centered", &TextArrayObject::get_centered, &TextArrayObject::set_centered);
  cls.addVar("anchor_point", &TextArrayObject::get_anchor_point, &TextArrayObject::set_anchor_point);
  cls.addVar("wrap_width", &TextArrayObject::get_wrap_width, &TextArrayObject::set_wrap_width);
  cls.addVar("roundness", &TextArrayObject::get_roundness, &TextArrayObject::set_roundness);
}

/* EOF */
