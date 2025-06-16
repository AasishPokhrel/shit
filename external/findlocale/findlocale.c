
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
#include <windows.h>
#include <winnt.h>
#endif

#include "findlocale.h"

static int
is_lcchar(const int c) {
  return isalnum(c);
}

static void
lang_country_variant_from_envstring(const char *str,
                                    char **lang,
                                    char **country,
                                    char **variant) {
  int end = 0;
  int start;

  /* get lang, if any */
  start = end;
  while (is_lcchar(str[end])) {
    ++end;
  }
  if (start != end) {
    int i;
    int len = end - start;
    char *s = malloc(len + 1);
    for (i=0; i<len; ++i) {
      s[i] = tolower(str[start + i]);
    }
    s[i] = '\0';
    *lang = s;
  } else {
    *lang = NULL;
  }

  if (str[end] && str[end]!=':') { /* not at end of str */
    ++end;
  }

  /* get country, if any */
  start = end;
  while (is_lcchar(str[end])) {
    ++end;
  }
  if (start != end) {
    int i;
    int len = end - start;
    char *s = malloc(len + 1);
    for (i=0; i<len; ++i) {
      s[i] = toupper(str[start + i]);
    }
    s[i] = '\0';
    *country = s;
  } else {
    *country = NULL;
  }

  if (str[end] && str[end]!=':') { /* not at end of str */
    ++end;
  }

  /* get variant, if any */
  start = end;
  while (str[end] && str[end]!=':') {
    ++end;
  }
  if (start != end) {
    int i;
    int len = end - start;
    char *s = malloc(len + 1);
    for (i=0; i<len; ++i) {
      s[i] = str[start + i];
    }
    s[i] = '\0';
    *variant = s;
  } else {
    *variant = NULL;
  }
}


static int
accumulate_locstring(const char *str, FL_Locale *l) {
  char *lang = NULL;
  char *country = NULL;
  char *variant = NULL;
  if (str) {
    lang_country_variant_from_envstring(str, &lang, &country, &variant);
    if (lang) {
      l->lang = lang;
      l->country = country;
      l->variant = variant;
      return 1;
    }
  }
  free(lang); free(country); free(variant);
  return 0;
}

#ifndef WIN32

static int
accumulate_env(const char *name, FL_Locale *l) {
  char *env;
  char *lang = NULL;
  char *country = NULL;
  char *variant = NULL;
  env = getenv(name);
  if (env) {
    return accumulate_locstring(env, l);
  }
  free(lang); free(country); free(variant);
  return 0;
}

#endif

static void
canonise_fl(FL_Locale *l) {
  /* this function fixes some common locale-specifying mistakes */
  /* en_UK -> en_GB */
  if (l->lang && 0 == strcmp(l->lang, "en")) {
    if (l->country && 0 == strcmp(l->country, "UK")) {
      free((void*)l->country);
      l->country = (FL_Country)malloc(strlen("GB") + 1);
      strcpy((char*)l->country, "GB");
    }
  }
  /* ja_JA -> ja_JP */
  if (l->lang && 0 == strcmp(l->lang, "ja")) {
    if (l->country && 0 == strcmp(l->country, "JA")) {
      free((void*)l->country);
      l->country = (FL_Country)malloc(strlen("JP") + 1);
      strcpy((char*)l->country, "JP");
    }
  }
}


#ifdef WIN32
#include <stdio.h>
#define ML(pn,sn) MAKELANGID(LANG_##pn, SUBLANG_##pn##_##sn)
#define MLN(pn) MAKELANGID(LANG_##pn, SUBLANG_DEFAULT)
#define RML(pn,sn) MAKELANGID(LANG_##pn, SUBLANG_##sn)
typedef struct {
  LANGID id;
  char*  code;
} IDToCode;
static const IDToCode both_to_code[] = {
  {ML(ENGLISH,US),           "en_US.ISO_8859-1"},
  {ML(ENGLISH,CAN),          "en_CA"}, /* english / canadian */
  {ML(ENGLISH,UK),           "en_GB"},
  {ML(ENGLISH,EIRE),         "en_IE"},
  {ML(ENGLISH,AUS),          "en_AU"},
  {MLN(GERMAN),              "de_DE"},
  {MLN(SPANISH),             "es_ES"},
  {ML(SPANISH,MEXICAN),      "es_MX"},
  {MLN(FRENCH),              "fr_FR"},
  {ML(FRENCH,CANADIAN),      "fr_CA"},
  {ML(FRENCH,BELGIAN),       "fr_BE"}, /* ? */
  {ML(DUTCH,BELGIAN),        "nl_BE"}, /* ? */
  {ML(PORTUGUESE,BRAZILIAN), "pt_BR"},
  {MLN(PORTUGUESE),          "pt_PT"},
  {MLN(SWEDISH),             "sv_SE"},
  {ML(CHINESE,HONGKONG),     "zh_HK"},
  /* these are machine-generated and not yet verified */
  {RML(AFRIKAANS,DEFAULT), "af_ZA"},
  {RML(ALBANIAN,DEFAULT), "sq_AL"},
  {RML(ARABIC,ARABIC_ALGERIA), "ar_DZ"},
  {RML(ARABIC,ARABIC_BAHRAIN), "ar_BH"},
  {RML(ARABIC,ARABIC_EGYPT), "ar_EG"},
  {RML(ARABIC,ARABIC_IRAQ), "ar_IQ"},
  {RML(ARABIC,ARABIC_JORDAN), "ar_JO"},
  {RML(ARABIC,ARABIC_KUWAIT), "ar_KW"},
  {RML(ARABIC,ARABIC_LEBANON), "ar_LB"},
  {RML(ARABIC,ARABIC_LIBYA), "ar_LY"},
  {RML(ARABIC,ARABIC_MOROCCO), "ar_MA"},
  {RML(ARABIC,ARABIC_OMAN), "ar_OM"},
  {RML(ARABIC,ARABIC_QATAR), "ar_QA"},
  {RML(ARABIC,ARABIC_SAUDI_ARABIA), "ar_SA"},
  {RML(ARABIC,ARABIC_SYRIA), "ar_SY"},
  {RML(ARABIC,ARABIC_TUNISIA), "ar_TN"},
  {RML(ARABIC,ARABIC_UAE), "ar_AE"},
  {RML(ARABIC,ARABIC_YEMEN), "ar_YE"},
  {RML(ARMENIAN,DEFAULT), "hy_AM"},
  {RML(AZERI,AZERI_CYRILLIC), "az_AZ"},
  {RML(AZERI,AZERI_LATIN), "az_AZ"},
  {RML(BASQUE,DEFAULT), "eu_ES"},
  {RML(BELARUSIAN,DEFAULT), "be_BY"},
/*{RML(BRETON,DEFAULT), "br_FR"},*/
  {RML(BULGARIAN,DEFAULT), "bg_BG"},
  {RML(CATALAN,DEFAULT), "ca_ES"},
  {RML(CHINESE,CHINESE_HONGKONG), "zh_HK"},
  {RML(CHINESE,CHINESE_MACAU), "zh_MO"},
  {RML(CHINESE,CHINESE_SIMPLIFIED), "zh_CN"},
  {RML(CHINESE,CHINESE_SINGAPORE), "zh_SG"},
  {RML(CHINESE,CHINESE_TRADITIONAL), "zh_TW"},
/*{RML(CORNISH,DEFAULT), "kw_GB"},*/
  {RML(CZECH,DEFAULT), "cs_CZ"},
  {RML(DANISH,DEFAULT), "da_DK"},
  {RML(DUTCH,DUTCH), "nl_NL"},
  {RML(DUTCH,DUTCH_BELGIAN), "nl_BE"},
/*{RML(DUTCH,DUTCH_SURINAM), "nl_SR"},*/
  {RML(ENGLISH,ENGLISH_AUS), "en_AU"},
  {RML(ENGLISH,ENGLISH_BELIZE), "en_BZ"},
  {RML(ENGLISH,ENGLISH_CAN), "en_CA"},
  {RML(ENGLISH,ENGLISH_CARIBBEAN), "en_CB"},
  {RML(ENGLISH,ENGLISH_EIRE), "en_IE"},
  {RML(ENGLISH,ENGLISH_JAMAICA), "en_JM"},
  {RML(ENGLISH,ENGLISH_NZ), "en_NZ"},
  {RML(ENGLISH,ENGLISH_PHILIPPINES), "en_PH"},
  {RML(ENGLISH,ENGLISH_SOUTH_AFRICA), "en_ZA"},
  {RML(ENGLISH,ENGLISH_TRINIDAD), "en_TT"},
  {RML(ENGLISH,ENGLISH_UK), "en_GB"},
  {RML(ENGLISH,ENGLISH_US), "en_US"},
  {RML(ENGLISH,ENGLISH_ZIMBABWE), "en_ZW"},
/*{RML(ESPERANTO,DEFAULT), "eo_"},*/
  {RML(ESTONIAN,DEFAULT), "et_EE"},
  {RML(FAEROESE,DEFAULT), "fo_FO"},
  {RML(FARSI,DEFAULT), "fa_IR"},
  {RML(FINNISH,DEFAULT), "fi_FI"},
  {RML(FRENCH,FRENCH), "fr_FR"},
  {RML(FRENCH,FRENCH_BELGIAN), "fr_BE"},
  {RML(FRENCH,FRENCH_CANADIAN), "fr_CA"},
  {RML(FRENCH,FRENCH_LUXEMBOURG), "fr_LU"},
  {RML(FRENCH,FRENCH_MONACO), "fr_MC"},
  {RML(FRENCH,FRENCH_SWISS), "fr_CH"},
/*{RML(GAELIC,GAELIC), "ga_IE"},*/
/*{RML(GAELIC,GAELIC_MANX), "gv_GB"},*/
/*{RML(GAELIC,GAELIC_SCOTTISH), "gd_GB"},*/
/*{RML(GALICIAN,DEFAULT), "gl_ES"},*/
  {RML(GEORGIAN,DEFAULT), "ka_GE"},
  {RML(GERMAN,GERMAN), "de_DE"},
  {RML(GERMAN,GERMAN_AUSTRIAN), "de_AT"},
  {RML(GERMAN,GERMAN_LIECHTENSTEIN), "de_LI"},
  {RML(GERMAN,GERMAN_LUXEMBOURG), "de_LU"},
  {RML(GERMAN,GERMAN_SWISS), "de_CH"},
  {RML(GREEK,DEFAULT), "el_GR"},
  {RML(GUJARATI,DEFAULT), "gu_IN"},
  {RML(HEBREW,DEFAULT), "he_IL"},
  {RML(HINDI,DEFAULT), "hi_IN"},
  {RML(HUNGARIAN,DEFAULT), "hu_HU"},
  {RML(ICELANDIC,DEFAULT), "is_IS"},
  {RML(INDONESIAN,DEFAULT), "id_ID"},
  {RML(ITALIAN,ITALIAN), "it_IT"},
  {RML(ITALIAN,ITALIAN_SWISS), "it_CH"},
  {RML(JAPANESE,DEFAULT), "ja_JP"},
  {RML(KANNADA,DEFAULT), "kn_IN"},
  {RML(KAZAK,DEFAULT), "kk_KZ"},
  {RML(KONKANI,DEFAULT), "kok_IN"},
  {RML(KOREAN,KOREAN), "ko_KR"},
/*{RML(KYRGYZ,DEFAULT), "ky_KG"},*/
  {RML(LATVIAN,DEFAULT), "lv_LV"},
  {RML(LITHUANIAN,LITHUANIAN), "lt_LT"},
  {RML(MACEDONIAN,DEFAULT), "mk_MK"},
  {RML(MALAY,MALAY_BRUNEI_DARUSSALAM), "ms_BN"},
  {RML(MALAY,MALAY_MALAYSIA), "ms_MY"},
  {RML(MARATHI,DEFAULT), "mr_IN"},
/*{RML(MONGOLIAN,DEFAULT), "mn_MN"},*/
  {RML(NORWEGIAN,NORWEGIAN_BOKMAL), "nb_NO"},
  {RML(NORWEGIAN,NORWEGIAN_NYNORSK), "nn_NO"},
  {RML(POLISH,DEFAULT), "pl_PL"},
  {RML(PORTUGUESE,PORTUGUESE), "pt_PT"},
  {RML(PORTUGUESE,PORTUGUESE_BRAZILIAN), "pt_BR"},
  {RML(PUNJABI,DEFAULT), "pa_IN"},
  {RML(ROMANIAN,DEFAULT), "ro_RO"},
  {RML(RUSSIAN,DEFAULT), "ru_RU"},
  {RML(SANSKRIT,DEFAULT), "sa_IN"},
  {RML(SERBIAN,DEFAULT), "hr_HR"},
  {RML(SERBIAN,SERBIAN_CYRILLIC), "sr_SP"},
  {RML(SERBIAN,SERBIAN_LATIN), "sr_SP"},
  {RML(SLOVAK,DEFAULT), "sk_SK"},
  {RML(SLOVENIAN,DEFAULT), "sl_SI"},
  {RML(SPANISH,SPANISH), "es_ES"},
  {RML(SPANISH,SPANISH_ARGENTINA), "es_AR"},
  {RML(SPANISH,SPANISH_BOLIVIA), "es_BO"},
  {RML(SPANISH,SPANISH_CHILE), "es_CL"},
  {RML(SPANISH,SPANISH_COLOMBIA), "es_CO"},
  {RML(SPANISH,SPANISH_COSTA_RICA), "es_CR"},
  {RML(SPANISH,SPANISH_DOMINICAN_REPUBLIC), "es_DO"},
  {RML(SPANISH,SPANISH_ECUADOR), "es_EC"},
  {RML(SPANISH,SPANISH_EL_SALVADOR), "es_SV"},
  {RML(SPANISH,SPANISH_GUATEMALA), "es_GT"},
  {RML(SPANISH,SPANISH_HONDURAS), "es_HN"},
  {RML(SPANISH,SPANISH_MEXICAN), "es_MX"},
  {RML(SPANISH,SPANISH_MODERN), "es_ES"},
  {RML(SPANISH,SPANISH_NICARAGUA), "es_NI"},
  {RML(SPANISH,SPANISH_PANAMA), "es_PA"},
  {RML(SPANISH,SPANISH_PARAGUAY), "es_PY"},
  {RML(SPANISH,SPANISH_PERU), "es_PE"},
  {RML(SPANISH,SPANISH_PUERTO_RICO), "es_PR"},
  {RML(SPANISH,SPANISH_URUGUAY), "es_UY"},
  {RML(SPANISH,SPANISH_VENEZUELA), "es_VE"},
  {RML(SWAHILI,DEFAULT), "sw_KE"},
  {RML(SWEDISH,SWEDISH), "sv_SE"},
  {RML(SWEDISH,SWEDISH_FINLAND), "sv_FI"},
/*{RML(SYRIAC,DEFAULT), "syr_SY"},*/
  {RML(TAMIL,DEFAULT), "ta_IN"},
  {RML(TATAR,DEFAULT), "tt_TA"},
  {RML(TELUGU,DEFAULT), "te_IN"},
  {RML(THAI,DEFAULT), "th_TH"},
  {RML(TURKISH,DEFAULT), "tr_TR"},
  {RML(UKRAINIAN,DEFAULT), "uk_UA"},
  {RML(URDU,URDU_PAKISTAN), "ur_PK"},
  {RML(UZBEK,UZBEK_CYRILLIC), "uz_UZ"},
  {RML(UZBEK,UZBEK_LATIN), "uz_UZ"},
  {RML(VIETNAMESE,DEFAULT), "vi_VN"},
/*{RML(WALON,DEFAULT), "wa_BE"},*/
/*{RML(WELSH,DEFAULT), "cy_GB"},*/
};
static const IDToCode primary_to_code[] = {
  {LANG_AFRIKAANS,  "af"},
  {LANG_ARABIC,     "ar"},
  {LANG_AZERI,      "az"},
  {LANG_BULGARIAN,  "bg"},
/*{LANG_BRETON,     "br"},*/
  {LANG_BELARUSIAN, "by"},
  {LANG_CATALAN,    "ca"},
  {LANG_CZECH,      "cs"},
/*{LANG_WELSH,      "cy"},*/
  {LANG_DANISH,     "da"},
  {LANG_GERMAN,     "de"},
  {LANG_GREEK,      "el"},
  {LANG_ENGLISH,    "en"},
/*{LANG_ESPERANTO,  "eo"},*/
  {LANG_SPANISH,    "es"},
  {LANG_ESTONIAN,   "et"},
  {LANG_BASQUE,     "eu"},
  {LANG_FARSI,      "fa"},
  {LANG_FINNISH,    "fi"},
  {LANG_FAEROESE,   "fo"},
  {LANG_FRENCH,     "fr"},
/*{LANG_GAELIC,     "ga"},*/
/*{LANG_GALICIAN,   "gl"},*/
  {LANG_GUJARATI,   "gu"},
  {LANG_HEBREW,     "he"},
  {LANG_HINDI,      "hi"},
  {LANG_SERBIAN,    "hr"},
  {LANG_HUNGARIAN,  "hu"},
  {LANG_ARMENIAN,   "hy"},
  {LANG_INDONESIAN, "id"},
  {LANG_ITALIAN,    "it"},
  {LANG_JAPANESE,   "ja"},
  {LANG_GEORGIAN,   "ka"},
  {LANG_KAZAK,      "kk"},
  {LANG_KANNADA,    "kn"},
  {LANG_KOREAN,     "ko"},
/*{LANG_KYRGYZ,     "ky"},*/
  {LANG_LITHUANIAN, "lt"},
  {LANG_LATVIAN,    "lv"},
  {LANG_MACEDONIAN, "mk"},
/*{LANG_MONGOLIAN,  "mn"},*/
  {LANG_MARATHI,    "mr"},
  {LANG_MALAY,      "ms"},
  {LANG_NORWEGIAN,  "nb"},
  {LANG_DUTCH,      "nl"},
  {LANG_NORWEGIAN,  "nn"},
  {LANG_NORWEGIAN,  "no"},/* unofficial? */
  {LANG_PUNJABI,    "pa"},
  {LANG_POLISH,     "pl"},
  {LANG_PORTUGUESE, "pt"},
  {LANG_ROMANIAN,   "ro"},
  {LANG_RUSSIAN,    "ru"},
  {LANG_SLOVAK,     "sk"},
  {LANG_SLOVENIAN,  "sl"},
  {LANG_ALBANIAN,   "sq"},
  {LANG_SERBIAN,    "sr"},
  {LANG_SWEDISH,    "sv"},
  {LANG_SWAHILI,    "sw"},
  {LANG_TAMIL,      "ta"},
  {LANG_THAI,       "th"},
  {LANG_TURKISH,    "tr"},
  {LANG_TATAR,      "tt"},
  {LANG_UKRAINIAN,  "uk"},
  {LANG_URDU,       "ur"},
  {LANG_UZBEK,      "uz"},
  {LANG_VIETNAMESE, "vi"},
/*{LANG_WALON,      "wa"},*/
  {LANG_CHINESE,    "zh"},
};
static int num_primary_to_code =
  sizeof(primary_to_code) / sizeof(*primary_to_code);
static int num_both_to_code =
  sizeof(both_to_code) / sizeof(*both_to_code);

static int
lcid_to_fl(LCID lcid,
           FL_Locale *rtn) {
  LANGID langid       = LANGIDFROMLCID(lcid);
  LANGID primary_lang = PRIMARYLANGID(langid);
//  LANGID sub_lang     = SUBLANGID(langid);
  int i;
  /* try to find an exact primary/sublanguage combo that we know about */
  for (i=0; i<num_both_to_code; ++i) {
    if (both_to_code[i].id == langid) {
      accumulate_locstring(both_to_code[i].code, rtn);
      return 1;
    }
  }
  /* fallback to just checking the primary language id */
  for (i=0; i<num_primary_to_code; ++i) {
    if (primary_to_code[i].id == primary_lang) {
      accumulate_locstring(primary_to_code[i].code, rtn);
      return 1;
    }
  }
  return 0;
}
#endif


FL_Success
FL_FindLocale(FL_Locale **locale) {
  FL_Success success = FL_FAILED;
  FL_Locale *rtn = malloc(sizeof(FL_Locale));
  rtn->lang = NULL;
  rtn->country = NULL;
  rtn->variant = NULL;

#ifdef WIN32
  /* win32 >= mswindows95 */
  {
    LCID lcid = GetThreadLocale();
    if (lcid_to_fl(lcid, rtn)) {
      success = FL_CONFIDENT;
    }
    if (success == FL_FAILED) {
      /* assume US English on mswindows systems unless we know otherwise */
      if (accumulate_locstring("en_US.ISO_8859-1", rtn)) {
        success = FL_DEFAULT_GUESS;
      }
    }
  }
#else
  /* assume unixoid */
  {
    /* examples: */
    /* sv_SE.ISO_8859-1 */
    /* fr_FR.ISO8859-1 */
    /* no_NO_NB */
    /* no_NO_NY */
    /* no_NO */
    /* de_DE */
    /* try the various vars in decreasing order of authority */
    if (accumulate_env("LC_ALL", rtn) ||
        accumulate_env("LC_MESSAGES", rtn) ||
        accumulate_env("LANG", rtn) ||
        accumulate_env("LANGUAGE", rtn)) {
      success = FL_CONFIDENT;
    }
    if (success == FL_FAILED) {
      /* assume US English on unixoid systems unless we know otherwise */
      if (accumulate_locstring("en_US.ISO_8859-1", rtn)) {
        success = FL_DEFAULT_GUESS;
      }
    }
  }
#endif

  if (success != FL_FAILED) {
    canonise_fl(rtn);
  }

  *locale = rtn;
  return success;
}


void
FL_FreeLocale(FL_Locale **locale) {
  if (locale) {
    FL_Locale *l = *locale;
    if (l) {
      if (l->lang) {
        free((void*)l->lang);
      }
      if (l->country) {
        free((void*)l->country);
      }
      if (l->variant) {
        free((void*)l->variant);
      }
      free(l);
      *locale = NULL;
    }
  }
}
