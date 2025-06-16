//  SuperTux
//  Copyright (C) 2006 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_UTIL_GETTEXT_HPP
#define HEADER_SUPERTUX_UTIL_GETTEXT_HPP

#include <tinygettext/tinygettext.hpp>
#include <memory>

extern std::unique_ptr<tinygettext::DictionaryManager> g_dictionary_manager;

/*
 * If you need to do a nontrivial substitution of values into a pattern, use
 * fmt::format rather than an ad-hoc concatenation.  That way, translators can
 * translate the format string as a whole (and even rearrange the values if
 * necessary with "%1$s"-style codes) instead of multiple pieces.  Patterns like
 * "Label: {}" with only one string piece are a borderline case where
 * fmt::format is not really necessary.
 *
 * http://www.mihai-nita.net/article.php?artID=20060430a
 * https://fmt.dev/latest/syntax.html
 *
 * Bad:
 *     std::string greeting = _("Hello ") + name + _("!");
 *     std::cout << _("Hello ") << name << _("!");
 * Good:
 *     #include <fmt/format.h>
 *     std::string greeting = fmt::format(fmt::runtime(_("Hello {}!")), name);
 *     std::cout << fmt::format(fmt::runtime(_("Hello {}!")), name);
 *
 * If you need singular and plural forms use __ instead of _ and fmt::format
 * if necessary.
 *
 * https://www.gnu.org/software/gettext/manual/html_node/Plural-forms.html
 *
 * Bad:
 *     std::cout << _("You collected ") << num << _(" coins");
 * Good:
 *     #include <fmt/format.h>
 *     std::cout << fmt::format(fmt::runtime(__("You collected {} coin",
 *                                              "You collected {} coins", num)),
 *                              num));
 */

static inline std::string _(const std::string& message)
{
  if (g_dictionary_manager)
  {
    return g_dictionary_manager->get_dictionary().translate(message);
  }
  else
  {
    return message;
  }
}

static inline std::string __(const std::string& message,
    const std::string& message_plural, int num)
{
  if (g_dictionary_manager)
  {
    return g_dictionary_manager->get_dictionary().translate_plural(message,
        message_plural, num);
  }
  else if (num == 1)
  {
    return message;
  }
  else
  {
    return message_plural;
  }
}

#endif

/* EOF */
