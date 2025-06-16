//  SuperTux
//  Copyright (C) 2004 Tobas Glaesser <tobi.web@gmx.de>
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

#include "supertux/menu/language_menu.hpp"
#include "addon/addon_manager.hpp"

extern "C" {
#include <findlocale.h>
}
#include "gui/item_action.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/title_screen.hpp"
#include "util/gettext.hpp"
#include "video/ttf_font.hpp"

enum {
  MNID_LANGUAGE_AUTO_DETECT = 0,
  MNID_LANGUAGE_ENGLISH     = 1,
  MNID_LANGUAGE_NEXT        = 10
};

LanguageMenu::LanguageMenu()
{
  add_label(_("Language"));
  add_hl();
  add_entry(MNID_LANGUAGE_AUTO_DETECT, _("<auto-detect>"));
  add_entry(MNID_LANGUAGE_ENGLISH, "English");

  int mnid = MNID_LANGUAGE_NEXT;
  auto languages = g_dictionary_manager->get_languages();
  for (auto& lang : languages)
  {
    auto& item = add_entry(mnid++, lang.get_localized_name());
    auto font = Resources::default_font;
    if(Resources::needs_custom_font(lang))
    {
      auto font_path = Resources::get_font_for_locale(lang);
      font = std::make_shared<TTFFont>(font_path, 18, 1.25f, 2, 1);
    }
    item.set_font(font);
  }

  add_hl();
  add_back(_("Back"));
}

void
LanguageMenu::menu_action(MenuItem& item)
{
  if (item.get_id() == MNID_LANGUAGE_AUTO_DETECT) // auto detect
  {
    FL_Locale *locale;
    FL_FindLocale(&locale);
    tinygettext::Language language = tinygettext::Language::from_spec( locale->lang?locale->lang:"", locale->country?locale->country:"", locale->variant?locale->variant:"");
    FL_FreeLocale(&locale);

    g_dictionary_manager->set_language(language); // set currently detected language
    g_config->locale = ""; // do auto detect every time on startup
    g_config->save();
    MenuManager::instance().clear_menu_stack();
  }
  else if (item.get_id() == MNID_LANGUAGE_ENGLISH) // english
  {
    g_config->locale = "en";
    g_dictionary_manager->set_language(tinygettext::Language::from_name(g_config->locale));
    g_config->save();
    MenuManager::instance().clear_menu_stack();
  }
  else
  {
    int mnid = MNID_LANGUAGE_NEXT;
    std::set<tinygettext::Language> languages = g_dictionary_manager->get_languages();

    for (auto& lang : languages)
    {
      if (item.get_id() == mnid++)
      {
        g_config->locale = lang.str();
        g_dictionary_manager->set_language(lang);
        g_config->save();
        break;
      }
    }
  }

  Resources::load();

  if (TitleScreen::current())
    TitleScreen::current()->refresh_copyright_text();

  if (g_dictionary_manager->get_language().get_language() != "en" &&
      !AddonManager::current()->is_addon_installed("language-pack"))
  {
    MenuManager::instance().push_menu(MenuStorage::LANGPACK_AUTO_UPDATE_MENU, true);
  }
  else
  {
    MenuManager::instance().clear_menu_stack();
  }
}

/* EOF */
