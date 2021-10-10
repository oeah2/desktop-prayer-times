/*
   Desktop Prayer Times app
   Copyright (C) 2021 Ahmet Öztürk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lang.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**< @brief Array containing language names */
char const*const lang_names[LANG_NUM] = {
    [LANG_EN] = "en",
    [LANG_DE] = "de",
    [LANG_TR] = "tr",
};

/**< @brief Array containing specific language dependent strings */
char const*const* language_specific_strings;

static char const*const language_specific_strings_german[LangStrings_Num] = {
	[LangStrings_CalcError_NoConnection] = "\n\nBitte überprüfe deine Internetverbindung",
	[LangStrings_CalcError_Unknown] = "\n\nUnbekannter Fehler. Bitte wähle eine andere Stadt aus und melde den Fehler an unsere Entwickler.",
	[LangStrings_Statusicon_Text] = "Nächstes Gebet für",
};

static char const*const language_specific_strings_english[LangStrings_Num] = {
	[LangStrings_CalcError_NoConnection] = "\n\nPlease check your internet connection",
	[LangStrings_CalcError_Unknown] = "\n\nUnknown error. Please switch to another city and report this error to our developers.",
	[LangStrings_Statusicon_Text] = "Next prayer for",
};

void lang_strings_init(enum Languages ID) {
	switch(ID) {
		case LANG_DE:
			language_specific_strings = language_specific_strings_german;
			break;
		
		case LANG_TR:
		case LANG_EN:
		default:
			language_specific_strings = language_specific_strings_english;
		break;
	}
}

bool lang_is_available(enum Languages ID)
{
    bool ret = false;

    char* filename = lang_get_filename(ID);
    if(filename) {
    	FILE* glade_file = fopen(filename, "r");
    	if(glade_file) {
    		fclose(glade_file);
    		ret = true;
    	}
    }
    return ret;
}

char* lang_get_filename(enum Languages ID)
{
	char const*const filename_format = "./lang/%s.glade";
    char* ret = malloc(strlen(filename_format) + strlen(lang_names[ID]) + 1);
    sprintf(ret, filename_format, lang_names[ID]);
    return ret;
}
