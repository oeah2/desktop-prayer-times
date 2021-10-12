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

#include "prayer_times.h"
#include "socket.h"
#include "lang.h"
#include "file.h"
#include "cJSON.h"
#include "error.h"

static char const*const diyanet_format_keywords[prayers_num] = {
    [pr_fajr] = "Imsak",
    [pr_fajr_end] = "Gunes",
    [pr_sunrise] = "GunesDogus",
    [pr_dhuhr] = "Ogle",
    [pr_asr] = "Ikindi",
    [pr_sunset] = "GunesBatis",
    [pr_maghreb] = "Aksam",
    [pr_ishaa] = "Yatsi"
};

char const*const diyanet_prayer_times_file_destination = "./Diyanet/";

//#define OLD_CODE
#ifdef OLD_CODE
static struct tm diyanet_mk_prayer_time(char* hr, char* date)
{
    char* current_pos = date;
    do {
        current_pos++;
    } while(!isdigit(*current_pos));

    struct tm tm = {0};
    if(sscanf(current_pos, "%d.%d.%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year) != 3) {       // 3 numbers to be detected
    	myperror(__FILE__, __LINE__, "Error: mk_prayer_time could not parse date");
        exit(EXIT_FAILURE);
    }
    tm.tm_mon--;
    tm.tm_year -= 1900;

    current_pos = hr;
    do {
        current_pos++;
    } while(!isdigit(*current_pos));

    if(sscanf(current_pos, "%d:%d", &tm.tm_hour, &tm.tm_min) != 2) {                  // 2 number to be detected
    	myperror(__FILE__, __LINE__, "Error: mk_prayer_time could not parse hours");
        exit(EXIT_FAILURE);
    }
    return tm;
}

static struct tm diyanet_mk_hijri_date(char* string_in)
{
    char* current_pos = string_in;
    do {
        current_pos++;
    } while(!isdigit(*current_pos));

    struct tm tm = {0};
    if(sscanf(current_pos, "%d.%d.%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year) != 3) {       // 3 numbers to be detected
    	myperror(__FILE__, __LINE__, "Error: mk_prayer_time could not parse date");
        exit(EXIT_FAILURE);
    }

    return tm;
}

#else
/** \brief Creates struct tm for given @p date and @p hr
 *
 * \param hr char* string containing time, format "hr:min"
 * \param date char* string containing date, format "dd.mm.yyyy"
 * \return structtm structure containing time
 *
 */
static struct tm diyanet_mk_prayer_time(char* hr, char* date)
{
    struct tm tm = {0};
    if(sscanf(date, "%d.%d.%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year) != 3) {
    	myperror(__FILE__, __LINE__, "Error: mk_prayer_time could not prase date");
        exit(EXIT_FAILURE);
    }

    tm.tm_mon--;
    tm.tm_year -= 1900;

    if(sscanf(hr, "%d:%d", &tm.tm_hour, &tm.tm_min) != 2) {
    	myperror(__FILE__, __LINE__, "Error: mk_prayer_time could not parse hours");
        exit(EXIT_FAILURE);
    }
    return tm;
}
#endif

#ifdef OLD_CODE
static int diyanet_parse_prayer_times(char* str, prayer times[prayers_num], size_t* rewind_neg)
{
    int ret = EXIT_FAILURE;
    if(str && times) {
        char* pos_end = strchr(str, '}');
        *rewind_neg = str + strlen(str) - pos_end - strlen("},") - 1;       // -1 for EOF checking
        char* pos_prayers[prayers_num];
        for(size_t i = 0; i < prayers_num; i++) {
            pos_prayers[i] = strstr(str, diyanet_format_keywords[i]);
        }
        if(!pos_end
                || !pos_prayers[pr_fajr]
                || !pos_prayers[pr_sunrise]
                || !pos_prayers[pr_dhuhr]
                || !pos_prayers[pr_asr]
                || !pos_prayers[pr_sunset]
                || !pos_prayers[pr_maghreb]
                || !pos_prayers[pr_ishaa]
                || !strstr(str, "MiladiTarihKisa\"")) {
        	myperror(__FILE__, __LINE__, "Format not accepted.");
            assert(pos_prayers[pr_fajr]);
            assert(pos_prayers[pr_sunrise]);
            assert(pos_prayers[pr_dhuhr]);
            assert(pos_prayers[pr_asr]);
            assert(pos_prayers[pr_maghreb]);
            assert(pos_prayers[pr_ishaa]);
            exit(EXIT_FAILURE);
        }
        str[pos_end - str +1] = '\0';
        char* pos_date = strstr(str, "MiladiTarihKisa\"");
        assert(pos_date);
        char* pos_hijri = strstr(str, "HicriTarihKisa");
        assert(pos_hijri);

        for(size_t i = 0; i < prayers_num; i++) {
            times[i].time_at = diyanet_mk_prayer_time(pos_prayers[i], pos_date);
            times[i].hicri_date = diyanet_mk_hijri_date(pos_hijri);
            times[i].name = prayer_names[i];
        }
        ret = EXIT_SUCCESS;
    }
    return ret;
}
#endif // OLD_CODE

#ifdef OLD_CODE
static int diyanet_get_prayer_times_for_date(FILE* file_times, prayer times[prayers_num], time_t date)
{
    double const secs_per_day = 60*60*24;

    if(!date)   // today
        date = time(0);

    if(difftime(date, time(0)) < 0) {
        return EFAULT;
    }

    size_t buffer_length = 700;
    char buffer[buffer_length];

GET_PRAYER_TIMES:
    if(!fgets(buffer, buffer_length, file_times)) {
    	myperror(__FILE__, __LINE__, "Error reading file.");
    }

    if(EOF == fgetc(file_times) && feof(file_times)) {
        /* end of file reached, update file */
    	myperror(__FILE__, __LINE__, "End of file reached.");
        return EOF;
    }

    size_t rewind_neg = 0;
    if(EXIT_FAILURE == diyanet_parse_prayer_times(buffer, times, &rewind_neg)) {
    	myperror(__FILE__, __LINE__, "Error while prasing prayer times.");
        exit(EXIT_FAILURE);
    }

    struct tm day = times[0].time_at;
    day.tm_hour = 0;
    day.tm_min = 0;
    fseek(file_times, -rewind_neg, SEEK_CUR);
    if(difftime(date, mktime(&day)) >= secs_per_day) {
        goto GET_PRAYER_TIMES;
    }
    return EXIT_SUCCESS;
}
#else



/** \brief Determines prayer times for given date
 * \details this function is used in prayer time functions of diyanet
 *
 * \param file_times FILE* source file of prayer times
 * \param times[prayers_num] prayer destination array for prayer times
 * \param date time_t date for the prayer times
 * \return int EXIT_SUCCESS on success, EFAULT when date is in past, EOF when end of file is reached, EXIT_FAILURE otherwise
 *
 */
static int diyanet_get_prayer_times_for_date(FILE* file_times, prayer times[prayers_num], time_t date)
{
    int ret = 0;
    long long int const secs_per_day = 60*60*24;

    if(!date) date = time(0);

    time_t day_start = time(0);
    day_start -= day_start % secs_per_day;
    if(difftime(date, day_start) < - secs_per_day / 2) {        // K�nnte sein, dass hier ein Problem bei anderen Timezones entsteht....
        return EFAULT;
    }

    size_t file_length = file_find_length(file_times) + 10;
    char* file_content = malloc(sizeof(char) * (file_length));
    if(!file_read_all(file_times, file_length, file_content)) {
        ret = ENOFILE;
    	myperror(__FILE__, __LINE__, "Error reading file");
        goto FREE_FILE_CONTENT;
    }

    cJSON* json = cJSON_Parse(file_content);
    if(!json) {
        ret = EFAULT;
    	myperror(__FILE__, __LINE__, "Error parsing JSON");
        goto FREE_JSON;
    }

    if(cJSON_IsArray(json)) {
        cJSON* element = 0;
        bool check_max_date = true;
        cJSON_ArrayForEach(element, json) {
            cJSON* json_date = cJSON_GetObjectItem(element, "MiladiTarihKisa");
            if(json_date) {
                struct tm tag = diyanet_mk_prayer_time("00:00", cJSON_GetStringValue(json_date));
                if(check_max_date && difftime(date, mktime(&tag)) >= 30 * secs_per_day) {
                    check_max_date = false;
                    ret = EOF;
                    goto FREE_JSON;
                }
                if(difftime(date, mktime(&tag)) >= secs_per_day)
                    continue;
                char* hijri_date = cJSON_GetStringValue(cJSON_GetObjectItem(element, "HicriTarihKisa"));
                struct tm hijri = {0};
                sscanf(hijri_date, "%d.%d.%d", &hijri.tm_mday, &hijri.tm_mon, &hijri.tm_year);
                for(size_t i = 0; i < prayers_num; i++) {
                    cJSON* json_prayer  = cJSON_GetObjectItem(element, diyanet_format_keywords[i]);
                    times[i].time_at = diyanet_mk_prayer_time(cJSON_GetStringValue(json_prayer), cJSON_GetStringValue(json_date));
                    times[i].name = prayer_names[i];
                    times[i].hicri_date = hijri;
                }
                ret = EXIT_SUCCESS;
                break;
            }
        }
    }

FREE_JSON:
    cJSON_Delete(json);
FREE_FILE_CONTENT:
    free(file_content);
    return ret;
}

#endif // OLD_CODE

int diyanet_get_todays_prayers(City city, prayer prayer_times[prayers_num])
{
    if(!prayer_times) {
        return EXIT_FAILURE;
    }
    if(!city.file_times) {
        return ENOFILE;
    }
    static bool first_event = true;
    static size_t counter = 0;
    rewind(city.file_times);
    switch(diyanet_get_prayer_times_for_date(city.file_times, prayer_times, 0)) {
    case ENOFILE:
    case EFAULT:    // will be thrown if file is corrupt
    case EOF:
        if(first_event) {
            if(counter >= 10) first_event = false;
            counter++;
            diyanet_update_file(&city, false);
            return diyanet_get_todays_prayers(city, prayer_times);
        } else {
        	myperror(__FILE__, __LINE__, "Error getting file!");
            return ENETDOWN;
        }
    case EXIT_SUCCESS:
    default:
        return EXIT_SUCCESS;
    }
}

int diyanet_get_preview_for_date(City city, prayer prayer_times[prayers_num], struct tm date)
{
    if(!prayer_times) {
        return EXIT_FAILURE;
    }
    date.tm_hour = 0;
    date.tm_min = 0;
    date.tm_sec = 0;

    rewind(city.file_times);
    return diyanet_get_prayer_times_for_date(city.file_times, prayer_times, mktime(&date));
}

int diyanet_get_preview_prayers(City city, size_t days, prayer prayer_times[days][prayers_num])
{
    if(!prayer_times || !days) {
        return EXIT_FAILURE;
    }
    double const secs_per_day = 60*60*24;
    double const max_days = 30;
    int ret = EXIT_SUCCESS;

    if(days > max_days) {
    	myperror(__FILE__, __LINE__, "Date too far away.");
        return EXIT_FAILURE;
    }

    time_t current_time = time(0);
    struct tm tm = *localtime(&current_time);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    rewind(city.file_times);
    for(size_t i = 0; i < days; i++) {
        ret = diyanet_get_prayer_times_for_date(city.file_times, prayer_times[i], mktime(&tm) + secs_per_day * (i + 1));
        if(ret == EXIT_FAILURE || ret == EOF) {
            break;
        }
    }
    return ret;
}

int diyanet_update_file(City* city, bool preserve_old_data)
{
	if(city) {
		char const*const host = "ezanvakti.herokuapp.com";
		char filename[strlen(city->filename)];
		strcpy(filename, city->filename);
		char* filename_without_type = 0;
		if(!strstr(filename, diyanet_prayer_times_file_destination))
			filename_without_type = strtok(filename, ".");
		else
			filename_without_type = strtok(filename + strlen(diyanet_prayer_times_file_destination), ".");
		size_t buff_len = strlen("/vakitler/") + strlen(filename_without_type) + 1;
		assert(buff_len);
		char buffer[buff_len];
		strcpy(buffer, "/vakitler/");
		strcat(buffer, filename_without_type);

		char* ret = http_get(host, buffer, 0);
		char* mode = preserve_old_data ? "a" : "w";     // Append file or not
		if(city->file_times)
			city->file_times = freopen(city->filename, mode, city->file_times);
		else
			city->file_times = fopen(city->filename, "w");
		fputs(ret, city->file_times);
		fflush(city->file_times);

		city->file_times = freopen(city->filename, "r", city->file_times);  // reopen for reading
	}
    return 0;
}

/** \brief Replaces wrong english country names
 * \details In the JSON source, some english country names are wrong. This function corrects them
 *
 * \param name char const* original name from JSON
 * \return char const* actual new
 *
 */
static char const* replace_wrong_names(char const* name)
{
    char const* ret = name;
    if(name) {
        if(!strcmp(name, "ANTIGUA VE BARBUDA"))
            return "Antigua and Barbuda";
        else if(!strcmp(name, "BOLIVYA"))
            return "Bolivia";
        else if(!strcmp(name, "BUTAN"))
            return "Bhutan";
        else if(!strcmp(name, "CAD"))
            return "Chad";
        else if(!strcmp(name, "CIBUTI"))
            return "Djibouti";
        else if(!strcmp(name, "DOGU TIMOR"))
            return "East Timor";
        else if(!strcmp(name, "DOMINIK CUMHURIYETI"))
            return "Dominican Republic";
        else if(!strcmp(name, "EKVATOR"))
            return "Ecuador";
        else if(!strcmp(name, "EKVATOR GINESI"))
            return "Equatorial Guinea";
        else if(!strcmp(name, "ERITRE"))
            return "Eritrea";
        else if(!strcmp(name, "ESTONYA"))
            return "Estonia";
        else if(!strcmp(name, "FILDISI SAHILI"))
            return "Ivory Coast";
        else if(!strcmp(name, "FILISTIN"))
            return "Palestine";
        else if(!strcmp(name, "GAMBIYA"))
            return "The Gambia";
        else if(!strcmp(name, "GRONLAND"))
            return "Greenland";
        else if(!strcmp(name, "GUADELOPE"))
            return "Guadeloupe";
        else if(!strcmp(name, "HIRVATISTAN"))
            return "Croatia";
        else if(!strcmp(name, "HOLLANDA ANTILLERI"))
            return "Netherlands Antilles";
        else if(!strcmp(name, "IZLANDA"))
            return "Iceland";
        else if(!strcmp(name, "JAMAIKA"))
            return "Jamaica";
        else if(!strcmp(name, "KAMBOCYA"))
            return "Cambodia";
        else if(!strcmp(name, "KARADAG"))
            return "Montenegro";
        else if(!strcmp(name, "KATAR"))
            return "Qatar";
        else if(!strcmp(name, "KOLOMBIYA"))
            return "COLOMBIA";
        else if(!strcmp(name, "KOMORLAR"))
            return "Comoros";
        else if(!strcmp(name, "KOSOVA"))
            return "Kosovo";
        else if(!strcmp(name, "KOSTARIKA"))
            return "Costa Rica";
        else if(!strcmp(name, "KUDUS"))
            return "Al-Quds";
        else if(!strcmp(name, "LESOTO"))
            return "Lesotho";
        else if(!strcmp(name, "LIBERYA"))
            return "LIBERIA";
        else if(!strcmp(name, "MADAGASKAR"))
            return "MADAGASCAR";
        else if(!strcmp(name, "MAKAO"))
            return "Macau";
        else if(!strcmp(name, "MALAVI"))
            return "Malawi";
        else if(!strcmp(name, "MALDIVLER"))
            return "MALDIVES";
        else if(!strcmp(name, "MARTINIK"))
            return "Martinique";
        else if(!strcmp(name, "MAURITIUS ADASI"))
            return "Mauritius";
        else if(!strcmp(name, "MIKRONEZYA"))
            return "Micronesia";
        else if(!strcmp(name, "MOLDAVYA"))
            return "Moldova";
        else if(!strcmp(name, "MORITANYA"))
            return "Mauritania";
        else if(!strcmp(name, "MOZAMBIK"))
            return "Mozambique";
        else if(!strcmp(name, "NAMIBYA"))
            return "Namibia";
        else if(!strcmp(name, "NIJER"))
            return "Niger";
        else if(!strcmp(name, "NIKARAGUA"))
            return "Nicaragua";
        else if(!strcmp(name, "ORTA AFRIKA CUMHURIYETI"))
            return "CENTRAL AFRICAN REPUBLIC";
        else if(!strcmp(name, "PAPUA YENI GINE"))
            return "Papua New Guinea";
        else if(!strcmp(name, "PITCAIRN ADASI"))
            return "Pitcairn Islands";
        else if(!strcmp(name, "RUANDA"))
            return "Rwanda";
        else if(!strcmp(name, "SEYSEL ADALARI"))
            return "Seychelles";
        else if(!strcmp(name, "SIRBISTAN"))
            return "Serbia";
        else if(!strcmp(name, "SURINAM"))
            return "Suriname";
        else if(!strcmp(name, "SVAZILAND"))
            return "Eswatini";
        else if(!strcmp(name, "TANZANYA"))
            return "Tanzania";
        else if(!strcmp(name, "TRINIDAT VE TOBAGO"))
            return "Trinidad and Tobago";
        else if(!strcmp(name, "TURKMENISTAN"))
            return "Turkmenistan";
        else if(!strcmp(name, "UMMAN"))
            return "Oman";
        else if(!strcmp(name, "VATIKAN"))
            return "Vatican";
        else if(!strcmp(name, "YENI KALEDONYA"))
            return "New Caledonia";
        else if(!strcmp(name, "YESIL BURUN"))
            return "Cape Verde";
        else if(!strcmp(name, "ZAMBIYA"))
            return "Zambia";
        else if(!strcmp(name, "ZIMBABVE"))
            return "Zimbabwe";
    }
    return ret;
}

char* diyanet_get_country_codes(enum Languages lang)
{
    char* ret = 0;
    char* json_identifier_country = 0;

    switch (lang) {
    case LANG_TR:
        json_identifier_country = "UlkeAdi";
        break;
    case LANG_DE:
    case LANG_EN:
    default:
        json_identifier_country = "UlkeAdiEn";
        break;
    case LANG_NUM:
        assert(0);
    }

    char const*const host = "ezanvakti.herokuapp.com";
    char* http_response = http_get(host, "/ulkeler", 0);

    cJSON* json = cJSON_Parse(http_response);
    if(!json) {
    	myperror(__FILE__, __LINE__, "Error parsing JSON.");
        return ret;
    }

    ret = calloc(1E5, sizeof(char));
    if(!ret) goto FREE_JSON;
    if(cJSON_IsArray(json)) {
        const cJSON* element = 0;
        cJSON_ArrayForEach(element, json) {
            cJSON* name = cJSON_GetObjectItem(element, json_identifier_country);
            strcat(ret, replace_wrong_names(name->valuestring));
            strcat(ret, ",");
            strcat(ret, cJSON_GetObjectItem(element, "UlkeID")->valuestring);
            strcat(ret, ";");
        }
    }
    char* ret_new = realloc(ret, strlen(ret)+1);
    if(!ret_new) return ret;
    ret = ret_new;

FREE_JSON:
    free(json);
    return ret;
}

char* diyanet_get_provinces(size_t country_code, enum Languages lang)
{
    char* ret = 0;
    if(country_code) {
        char const*const host = "ezanvakti.herokuapp.com";
        char file[32];
        sprintf(file, "/sehirler/%zu", country_code);
        char* http_response = http_get(host, file, 0);

cJSON* json = cJSON_Parse(http_response);
        if(!json) {
        	myperror(__FILE__, __LINE__, "Error parsing JSON.");
            return ret;
        }

        char* json_identifier_province = 0;
        switch (lang) {
        case LANG_DE:
        case LANG_EN:
        default:
            json_identifier_province = "SehirAdi";
            break;
        case LANG_TR:
            json_identifier_province = "SehirAdiEn";
            break;
        case LANG_NUM:
            assert(0);
        }

        ret = calloc(1E5, sizeof(char));
        if(!ret) goto FREE_JSON;
        if(cJSON_IsArray(json)) {
            const cJSON* element = 0;
            cJSON_ArrayForEach(element, json) {
                strcat(ret, cJSON_GetObjectItem(element, json_identifier_province)->valuestring);
                strcat(ret, ",");
                strcat(ret, cJSON_GetObjectItem(element, "SehirID")->valuestring);
                strcat(ret, ";");
            }
        }
        char* ret_new = realloc(ret, strlen(ret)+1);
        if(!ret_new) return ret;
        ret = ret_new;
FREE_JSON:
        free(json);
    }
    return ret;
}

char* diyanet_get_cities(size_t province_code, enum Languages lang)
{
    char* ret = 0;
    if(province_code) {
        char const*const host = "ezanvakti.herokuapp.com";
        char file[31];
        sprintf(file, "/ilceler/%zu", province_code);
        char* http_response = http_get(host, file, 0);

        cJSON* json = cJSON_Parse(http_response);
        if(!json) {
        	myperror(__FILE__, __LINE__, "Error parsing JSON.");
            return ret;
        }

        char* json_identifier_cities = 0;
        switch (lang) {
        case LANG_EN:
            json_identifier_cities = "IlceAdiEn";
            break;
        case LANG_DE:
        case LANG_TR:
        default:
            json_identifier_cities = "IlceAdi";
            break;
        case LANG_NUM:
            assert(0);
        }

        ret = calloc(1E6, sizeof(char));
        if(!ret) goto FREE_JSON;
        if(cJSON_IsArray(json)) {
            const cJSON* element = 0;
            cJSON_ArrayForEach(element, json) {
                strcat(ret, cJSON_GetObjectItem(element, json_identifier_cities)->valuestring);
                strcat(ret, ",");
                strcat(ret, cJSON_GetObjectItem(element, "IlceID")->valuestring);
                strcat(ret, ";");
            }
        }
        char* ret_new = realloc(ret, strlen(ret) + 1);
        if(!ret_new) return ret;
        ret = ret_new;
FREE_JSON:
        free(json);
    }
    return ret;
}
