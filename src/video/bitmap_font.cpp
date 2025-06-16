//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
//                     Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/bitmap_font.hpp"

#include <algorithm>
#include <physfs.h>
#include <cmath>
#include <sstream>

#include "physfs/physfs_sdl.hpp"
#include "physfs/util.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/utf8_iterator.hpp"
#include "video/canvas.hpp"
#include "video/drawing_request.hpp"
#include "video/surface.hpp"
#include "video/sdl_surface.hpp"

namespace {

bool vline_empty(const SDLSurfacePtr& surface, int x, int start_y, int end_y, Uint8 threshold)
{
  const Uint8* pixels = static_cast<Uint8*>(surface->pixels);

  for (int y = start_y; y < end_y; ++y)
  {
    const Uint8& p = pixels[surface->pitch*y + x*surface->format->BytesPerPixel + 3];
    if (p > threshold)
    {
      return false;
    }
  }
  return true;
}

} // namespace

BitmapFont::BitmapFont(GlyphWidth glyph_width_,
           const std::string& filename,
           int shadowsize_) :
  glyph_width(glyph_width_),
  glyph_surfaces(),
  shadow_surfaces(),
  char_height(),
  shadowsize(shadowsize_),
  border(0),
  rtl(false),
  glyphs(65536)
{
  for (unsigned int i=0; i<65536;i++) glyphs[i].surface_idx = -1;

  const std::string fontdir = FileSystem::dirname(filename);
  const std::string fontname = FileSystem::basename(filename);

  // scan for prefix-filename in addons search path.
  physfsutil::enumerate_files(fontdir, [fontdir, fontname, this](const std::string& file) {
    const std::string filepath = FileSystem::join(fontdir, file);
    if (file.rfind(fontname) != std::string::npos)
    {
      try
      {
        loadFontFile(filepath);
      }
      catch(const std::exception& e)
      {
        log_fatal << "Couldn't load font file: " << e.what() << std::endl;
      }
    }
    return false;
  });
}

void
BitmapFont::loadFontFile(const std::string &filename)
{
  // FIXME: Workaround for a crash on MSYS2 when starting with --debug.
  log_debug_ << "Loading font: " << filename << std::endl;
  auto doc = ReaderDocument::from_file(filename);
  auto root = doc.get_root();
  if (root.get_name() != "supertux-font") {
    std::ostringstream msg;
    msg << "Font file:" << filename << ": is not a supertux-font file";
    throw std::runtime_error(msg.str());
  }

  auto config_l = root.get_mapping();

  int def_char_width=0;

  if ( !config_l.get("glyph-width",def_char_width) ) {
    log_warning << "Font:" << filename << ": misses default glyph-width" << std::endl;
  }

  if ( !config_l.get("glyph-height",char_height) ) {
    std::ostringstream msg;
    msg << "Font:" << filename << ": misses glyph-height";
    throw std::runtime_error(msg.str());
  }

  config_l.get("glyph-border", border);
  config_l.get("rtl", rtl);

  auto iter = config_l.get_iter();
  while (iter.next()) {
    const std::string& token = iter.get_key();
    if ( token == "surface" ) {
      auto glyphs_val = iter.as_mapping();
      int local_char_width;
      bool monospaced;
      GlyphWidth local_glyph_width;
      std::string glyph_image;
      std::string shadow_image;
      std::vector<std::string> chars;
      if ( ! glyphs_val.get("glyph-width", local_char_width) ) {
        local_char_width = def_char_width;
      }
      if ( ! glyphs_val.get("monospace", monospaced ) ) {
        local_glyph_width = glyph_width;
      }
      else {
        if ( monospaced ) local_glyph_width = FIXED;
        else local_glyph_width = VARIABLE;
      }
      if ( ! glyphs_val.get("glyphs", glyph_image) ) {
        std::ostringstream msg;
        msg << "Font:" << filename << ": missing glyphs image";
        throw std::runtime_error(msg.str());
      }
      if ( ! glyphs_val.get("shadows", shadow_image) ) {
        std::ostringstream msg;
        msg << "Font:" << filename << ": missing shadows image";
        throw std::runtime_error(msg.str());
      }
      if ( ! glyphs_val.get("chars", chars) || chars.size() == 0) {
        std::ostringstream msg;
        msg << "Font:" << filename << ": missing chars definition";
        throw std::runtime_error(msg.str());
      }

      if ( local_char_width==0 ) {
        std::ostringstream msg;
        msg << "Font:" << filename << ": misses glyph-width for some surface";
        throw std::runtime_error(msg.str());
      }

      loadFontSurface(glyph_image, shadow_image, chars,
                      local_glyph_width, local_char_width);
    }
  }
}

void
BitmapFont::loadFontSurface(const std::string& glyphimage,
                      const std::string& shadowimage,
                      const std::vector<std::string>& chars,
                      GlyphWidth glyph_width_,
                      int char_width)
{
  SurfacePtr glyph_surface  = Surface::from_file("images/engine/fonts/" + glyphimage);
  SurfacePtr shadow_surface = Surface::from_file("images/engine/fonts/" + shadowimage);

  int surface_idx = static_cast<int>(glyph_surfaces.size());
  glyph_surfaces.push_back(glyph_surface);
  shadow_surfaces.push_back(shadow_surface);

  int row = 0;
  int col = 0;
  int wrap = glyph_surface->get_width() / char_width;

  SDLSurfacePtr surface;

  if ( glyph_width_ == VARIABLE )
  {
    surface = SDLSurface::from_file("images/engine/fonts/" + glyphimage);
    SDL_LockSurface(surface.get());
  }

  for (unsigned int i = 0; i < chars.size(); ++i) {
    for (UTF8Iterator chr(chars[i]); !chr.done(); ++chr) {
      int y = row * (char_height + 2*border) + border;
      int x = col * (char_width + 2*border) + border;
      if ( ++col == wrap ) { col=0; row++; }
      if ( *chr == 0x0020 && glyphs[0x20].surface_idx != -1) continue;

      Glyph glyph;
      glyph.surface_idx   = surface_idx;

      if ( glyph_width_ == FIXED || (*chr <= 255 && isdigit(*chr)) )
      {
        glyph.rect    = Rectf(static_cast<float>(x),
                              static_cast<float>(y),
                              static_cast<float>(x + char_width),
                              static_cast<float>(y + char_height));
        glyph.offset  = Vector(0, 0);
        glyph.advance = static_cast<float>(char_width);
      }
      else
      {
        if (y + char_height > surface->h)
        {
          log_warning << "error: font definition contains more letter then the images: " << glyphimage << std::endl;
          goto abort;
        }

        int left = x;
        while (left < x + char_width && vline_empty(surface, left, y, y + char_height, 64))
          left += 1;
        int right = x + char_width - 1;
        while (right > left && vline_empty(surface, right, y, y + char_height, 64))
          right -= 1;

        if (left <= right)
        {
          glyph.offset  = Vector(static_cast<float>(x) - static_cast<float>(left), 0.0f);
          glyph.advance = static_cast<float>(right - left + 1 + 1); // FIXME: might be useful to make spacing configurable.
        }
        else
        { // Glyph is completely transparent.
          glyph.offset  = Vector(0, 0);
          glyph.advance = static_cast<float>(char_width + 1); // FIXME: might be useful to make spacing configurable.
        }

        glyph.rect = Rectf(static_cast<float>(x),
                           static_cast<float>(y),
                           static_cast<float>(x + char_width),
                           static_cast<float>(y + char_height));
      }

      glyphs[*chr] = glyph;
    }
    if ( col>0 && col <= wrap ) {
      col = 0;
      row++;
    }
  }
abort:

  if (surface) {
    SDL_UnlockSurface(surface.get());
  }
}

BitmapFont::~BitmapFont()
{
}

float
BitmapFont::get_text_width(const std::string& text) const
{
  float curr_width = 0;
  float last_width = 0;

  for (UTF8Iterator it(text); !it.done(); ++it)
  {
    if (*it == '\n')
    {
      last_width = std::max(last_width, curr_width);
      curr_width = 0;
    }
    else
    {
      if ( glyphs.at(*it).surface_idx != -1 )
        curr_width += glyphs[*it].advance;
      else
        curr_width += glyphs[0x20].advance;
    }
  }

  return std::max(curr_width, last_width);
}

float
BitmapFont::get_text_height(const std::string& text) const
{
  std::string::size_type text_height = char_height;

  for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
  { // Since UTF8 multibyte characters are decoded with values
    // outside the ASCII range there is no risk of overlapping and
    // thus we don't need to decode the utf-8 string.
    if (*it == '\n')
      text_height += char_height + 2;
  }

  return static_cast<float>(text_height);
}

float
BitmapFont::get_height() const
{
  return static_cast<float>(char_height);
}

std::string
BitmapFont::wrap_to_width(const std::string& s_, float width, std::string* overflow)
{
  std::string s = s_;

  // If text is already smaller, return full text.
  if (get_text_width(s) <= width) {
    if (overflow) *overflow = "";
    return s;
  }

  // If we can find a whitespace character to break at, return text up to this character.
  for (int i = static_cast<int>(s.length()) - 1; i >= 0; i--) {
    std::string s2 = s.substr(0,i);
    if (s[i] != ' ') continue;
    if (get_text_width(s2) <= width) {
      if (overflow) *overflow = s.substr(i+1);
      return s.substr(0, i);
    }
  }

  // Hard-wrap at width, taking care of multibyte characters.
  unsigned int char_bytes = 1;
  for (int i = 0; i < static_cast<int>(s.length()); i += char_bytes) {

    // Calculate the number of bytes in the character.
    char_bytes = 1;
    auto iter = s.begin() + i + 1; // Iter points to next byte.
    while ( iter != s.end() && (*iter & 128) && !(*iter & 64) ) {
      // This is a "continuation" byte in the form 10xxxxxx.
      ++iter;
      ++char_bytes;
    }

    // Check whether text now goes over allowed width, and if so
    // return everything up to the character and put the rest in the overflow.
    std::string s2 = s.substr(0,i+char_bytes);
    if (get_text_width(s2) > width) {
      if (i == 0) i += char_bytes; // Edge case when even one char is too wide.
      if (overflow) *overflow = s.substr(i);
      return s.substr(0, i);
    }
  }

  // Should in theory never reach here.
  if (overflow) *overflow = "";
  return s;
}


Rectf
BitmapFont::draw_text(Canvas& canvas, const std::string& text,
                      const Vector& pos, FontAlignment alignment, int layer, const Color& color)
{
  float min_x = pos.x;
  float last_y = pos.y;
  float max_width = 0.f;

  std::string::size_type last = 0;
  for (std::string::size_type i = 0; i <= text.size(); ++i)
  {
    if (i != text.size() && text[i] != '\n')
      continue;

    const std::string temp = text.substr(last, i - last);
    const float width = get_text_width(temp);

    // Calculate X positions based on the alignment type.
    Vector new_pos = Vector(pos.x, last_y);

    if (alignment == ALIGN_CENTER)
      new_pos.x -= width / 2.0f;
    else if (alignment == ALIGN_RIGHT)
      new_pos.x -= width;

    // Cast font position to integer to get a clean drawing result and
    // no blurring as we would get with subpixel positions.
    new_pos.x = std::truncf(new_pos.x);

    if (new_pos.x < min_x)
      min_x = new_pos.x;
    if (width > max_width)
      max_width = width;

    draw_text(canvas, temp, new_pos, layer, color);

    last_y += static_cast<float>(char_height) + 2.0f;
    last = i + 1;
  }

  return Rectf(min_x, pos.y, min_x + max_width, last_y);
}

void
BitmapFont::draw_text(Canvas& canvas, const std::string& text, const Vector& pos, int layer, Color color) const
{
  if (shadowsize > 0)
    draw_chars(canvas, false, rtl ? std::string(text.rbegin(), text.rend()) : text,
               pos + Vector(static_cast<float>(shadowsize), static_cast<float>(shadowsize)), layer,
               Color(1,1,1));

  draw_chars(canvas, true, rtl ? std::string(text.rbegin(), text.rend()) : text, pos, layer, color);
}

void
BitmapFont::draw_chars(Canvas& canvas, bool notshadow, const std::string& text, const Vector& pos, int layer, Color color) const
{
  Vector p = pos;

  for (UTF8Iterator it(text); !it.done(); ++it)
  {
    if (*it == '\n')
    {
      p.x = pos.x;
      p.y += static_cast<float>(char_height) + 2.0f;
    }
    else if (*it == ' ')
    {
      p.x += glyphs[0x20].advance;
    }
    else
    {
      Glyph glyph;
      if ( glyphs.at(*it).surface_idx != -1 )
        glyph = glyphs[*it];
      else
        glyph = glyphs[0x20];

      // FIXME: not supported! request.color = color;
      canvas.draw_surface_part(notshadow ?
                               glyph_surfaces[glyph.surface_idx] :
                               shadow_surfaces[glyph.surface_idx],
                               glyph.rect,
                               Rectf(p + glyph.offset, glyph.rect.get_size()),
                               layer,
                               PaintStyle().set_color(color));

      p.x += glyph.advance;
    }
  }
}

/* EOF */
