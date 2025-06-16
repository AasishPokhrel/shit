
#include <stdlib.h>
#include <stdio.h>

#include "findlocale.h"

int
main(int in_argc, char **in_argv) {
  FL_Locale *locale;
  FL_FindLocale(&locale, FL_MESSAGES);
  printf("I think that your current settings are...\n"
         "Language = '%s'\nCountry = '%s'\nVariant = '%s'\n",
         locale->lang ? locale->lang : "(null)",
         locale->country ? locale->country : "(null)",
         locale->variant ? locale->variant : "(null)");
  FL_FreeLocale(&locale);
  return 0;
}

