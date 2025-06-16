/*
 * src/util/reader.cpp - Utility functions for config handling.
 * Copyright (C) 2010  Florian Forster
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Authors:
 *   Florian "octo" Forster <supertux at octo.it>
 */

#include "util/reader.hpp"

#include <physfs.h>

#include "editor/editor.hpp"
#include "util/gettext.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

int reader_get_layer(const ReaderMapping& reader, int def)
{
  int tmp = 0;
  bool status;

  // 'z-pos' is the canonical name
  status = reader.get("z-pos", tmp);

  // 'layer' is the old name kept for backward compatibility
  if (!status)
    status = reader.get("layer", tmp);

  if (!status)
    tmp = def;

  if (!Editor::is_active()) {
    if (tmp > (LAYER_GUI - 100))
      tmp = LAYER_GUI - 100;
  }

  return (tmp);
}

namespace {

std::string dirname(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if (p == std::string::npos) {
    return {};
  } else {
    return filename.substr(0, p);
  }
}

} // namespace

void register_translation_directory(const std::string& filename)
{
  if (g_dictionary_manager) {
    std::string rel_dir = dirname(filename);
    if (rel_dir.empty()) {
      // Relative dir inside PhysFS search path?
      // Get full path from search path, instead.
      const char* rel_dir_c = PHYSFS_getRealDir(filename.c_str());
      if (rel_dir_c) {
        rel_dir = rel_dir_c;
      }
    }

    if (!rel_dir.empty()) {
      g_dictionary_manager->add_directory(rel_dir);
    }
  }
}

/* EOF */
