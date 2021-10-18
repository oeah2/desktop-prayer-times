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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "socket.h"
#include "hadith.h"

extern char const*const hadith_api_key;
static char const*const start = "\"body\":\"";
static char const*const end = "\",\"grades\"";

/** \brief Check if string @hadith is valid, cut out only english part
 *
 */
static bool hadith_is_valid(char* hadith) {
	bool ret = false;
	if(hadith) {
		if(strstr(hadith, start) && strstr(hadith, end)) {
			ret = true;
		}
	}
	return ret;
}

/** \brief Detect markup letters
 *
 */
static bool hadith_letter_is_markup(char letter) {
	bool ret = false;
	switch(letter) {
	case '<':
	case '>':
	case '\\':
		ret = true;
		break;
	default:
		break;
	}
	return ret;
}

/** \brief Detect possible markup letters
 *
 */
static bool hadith_letter_could_be_markup(char letter) {
	bool ret = false;
	switch(letter) {
	case 'b':
	case 'p':
	case 'r':
	case '/':
		ret = true;
		break;
	default:
		break;
	}
	return ret;
}

/** \brief Detect salawat.
 *
 */
static bool hadith_letter_is_salawat(char* start_pos) {
	return start_pos == strstr(start_pos, "\\ufdfa");
}

#ifdef REMOVE
static void hadith_replace_salawat(size_t len, char position[len]) {
	assert(hadith_letter_is_salawat(position));

	size_t salawat_length = strlen("\\ufdfa");
	char letter_after_salawat = position[salawat_length];
	assert(letter_after_salawat == ')');

	wchar_t* salawat_utf = '\uFDFA';
	strcpy(position, salawat_utf);
	position[letter_after_salawat] = ')';
	/*
	char* start_copy = position + salawat_length;

	char buffer[length - salawat_length];
	strcpy(buffer, start_copy);
	*/

}
#endif

/** \brief Determine markup length at @p start_pos
 *
 */
static size_t hadith_find_markup_length(size_t len, char start_pos[len]) {
	size_t ret = 0;
	assert(hadith_letter_is_markup(*start_pos));
	size_t counter = 1;
	while( start_pos[counter] != '\0' && (
			hadith_letter_is_markup(start_pos[counter]) ||
			hadith_letter_could_be_markup(start_pos[counter]))) {
		counter++;
	}
	ret = counter;
	return ret;
}

/** \brief Remove next markup from @p start_pos
 *
 */
static void hadith_remove_markup(size_t len, char start_pos[len]) {
	assert(hadith_letter_is_markup(start_pos[0]));
	size_t markup_length = hadith_find_markup_length(len, start_pos);

	size_t hadith_length = strlen(start_pos);
	char buffer[hadith_length - markup_length + 1];
	strcpy(buffer, start_pos + markup_length);
	strcpy(start_pos, buffer);
}

/** \brief Remove all markups from @p hadith
 *
 */
static void hadith_remove_all_markups(size_t len, char hadith[len]) {
	for(size_t i = 0; i < strlen(hadith); i++) {
		if(hadith_letter_is_markup(hadith[i]) &&
			!hadith_letter_is_salawat(&hadith[i])) {
			hadith_remove_markup(len - i, hadith + i);
			i--;
		} else if(hadith_letter_is_salawat(&hadith[i])) {
			//hadith_replace_salawat(len - i, &hadith[i]);
		}
	}
}

/** \brief Take english part of @p hadith and remove all markups
 *
 */
static char* hadith_cut_relevant(char* hadith) {
	char* ret = 0;
	if(hadith) {
		if(hadith_is_valid(hadith)) {
			char* pos_end = strstr(hadith, end);
			char* pos_start = strstr(hadith, start);
			assert(pos_end && pos_start);

			size_t len = pos_end - pos_start + 10;
			assert(len);
			char buffer[len];
			*pos_end = '\0';
			strcpy(buffer, pos_start + strlen(start));

			hadith_remove_all_markups(len, buffer);

			char* new = realloc(hadith, strlen(buffer) + 10);
			if(new) {
				strcpy(new, buffer);
				ret = new;
			}
		}
	}
	return ret;
}

char* hadith_get_random(void) {
	char* ret = 0;

	size_t len = strlen(hadith_api_key) + strlen("x-api-key: ") + 10;
	char buffer[len];
	strcpy(buffer, "x-api-key: ");
	strcat(buffer, hadith_api_key);

	ret = https_get("api.sunnah.com", "/v1/hadiths/random", buffer);
	if(ret) {
		ret = hadith_cut_relevant(ret);
	}
	return ret;
}
