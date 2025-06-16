#ifndef __findlocale_h_
#define __findlocale_h_

typedef const char* FL_Lang;
typedef const char* FL_Country;
typedef const char* FL_Variant;

typedef struct {
  FL_Lang    lang;
  FL_Country country;
  FL_Variant variant;
} FL_Locale;

typedef enum {
  /* for some reason we failed to even guess: this should never happen */
  FL_FAILED        = 0,
  /* couldn't query locale -- returning a guess (almost always English) */
  FL_DEFAULT_GUESS = 1,
  /* the returned locale type was found by successfully asking the system */
  FL_CONFIDENT     = 2
} FL_Success;

typedef enum {
  FL_MESSAGES = 0
} FL_Domain;

/* This allocates/fills in a FL_Locale structure with pointers to
   strings (which should be treated as static), or NULL for inappropriate /
   undetected fields. */
FL_Success FL_FindLocale(FL_Locale **locale);
/* This should be used to free the struct written by FL_FindLocale */
void FL_FreeLocale(FL_Locale **locale);

#endif /*__findlocale_h_*/
