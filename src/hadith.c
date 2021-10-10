/*
 * hadith.c
 *
 *  Created on: 09.10.2021
 *      Author: ahmet
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

static bool hadith_is_valid(char* hadith) {
	bool ret = false;
	if(hadith) {
		if(strstr(hadith, start) && strstr(hadith, end)) {
			ret = true;
		}
	}
	return ret;
}

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

static bool hadith_contains_salawat(char* hadith) {
	return strstr(hadith, "(\\ufdfa)");
}

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

static void hadith_remove_markup(size_t len, char start_pos[len]) {
	assert(hadith_letter_is_markup(start_pos[0]));
	size_t markup_length = hadith_find_markup_length(len, start_pos);

	size_t hadith_length = strlen(start_pos);
	char buffer[hadith_length - markup_length + 1];
	strcpy(buffer, start_pos + markup_length);
	strcpy(start_pos, buffer);
}

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
