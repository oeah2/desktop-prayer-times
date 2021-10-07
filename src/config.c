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

#include "config.h"
#include "error.h"
#include "cJSON.h"
#include "file.h"
#include "socket.h"
#include "Salah_times_calc.h"
#include "prayer_times.h"

enum config_specifiers {
    spec_num_cities,
    spec_city,
    spec_lang,
    spec_pos,
    spec_num
};

enum config_json_specifiers {
    json_num_cities,
    json_cities,
    json_city,
    json_name,
    json_provider,
    json_id,
    json_method,
    json_longitude,
    json_latitude,
    json_asrmethod,
    json_highlats,
    json_filename,
    json_savepos,
    json_posX,
    json_posY,
    json_lang,
    json_check_for_updates,
    json_notifications,
    json_NUM,
};

static char const*const config_format_json_specifiers[json_NUM] = {
    [json_num_cities] = "Number Cities",
    [json_cities] = "cities",
    [json_city] = "city",
    [json_name] = "name",
    [json_provider] = "provider",
    [json_id] = "ID",
    [json_method] = "method",
    [json_longitude] = "longitude",
    [json_latitude] = "latitude",
    [json_asrmethod] = "asrmethod",
    [json_highlats] = "highlats",
    [json_filename] = "filename",
    [json_savepos] = "Save Position",
    [json_posX] = "PosX",
    [json_posY] = "PosY",
    [json_lang] = "language",
    [json_check_for_updates] = "Check for updates",
    [json_notifications] = "Enable notifications",
};

static char const*const config_format_specifiers[] = {
    [spec_num_cities] = "[num_cities]",
    [spec_city] = "[city]",
    [spec_lang] = "[language]",
    [spec_pos] = "[pos]",
};

#define CFG_USE_JSON
#ifndef CFG_USE_JSON
/** \brief parses a line of the config string
 * \details This function scans @p string_in for valid specifiers from @p config_format_specifiers. Depending on the specifier different parsings are done. Either cities are added, or other configurations are applied.
 *
 * \param string_in char* Line of string to be parsed
 * \param cfg Config* to which the changes shall be applied
 * \return int EXIT_FAILURE upon failure, EXIT_SUCCESS otherwise
 *
 */
static int config_parse_string(char *string_in, Config* cfg)
{
    assert(string_in);
    char const*const delimiters = ":;\n";
    char* str_result = strtok(string_in, ":;\n");
    assert(str_result);

    if(!str_result) {
        return EXIT_FAILURE;
    }

    size_t found_index = 0;
    for(size_t i = 0; i < spec_num; i++) {
        if(!strcmp(str_result, config_format_specifiers[i])) {
            found_index = i;
            break;
        }
    }
    str_result = strtok(0, delimiters);
    City* current_city = 0;
    switch(found_index) {
    case spec_city:
        current_city = &cfg->cities[cfg->counter_parsed_cities];
        assert(current_city);
        char* name = str_result;
        char* calc_provider = strtok(0, delimiters);
        char* filename = strtok(0, delimiters);
        size_t id = strtoul(strtok(0, delimiters), 0, 10);
        size_t provider = 0;
        assert(name && filename);
        for(size_t i = 0; i < prov_num; i++) {
            if(!strcmp(calc_provider, provider_names[i]))
                provider = i;
        }
        if(provider == prov_diyanet) {
            current_city = city_init_diyanet(current_city, name, provider, filename, id);
        } else if(provider == prov_calc) {
            char* method_str = strtok(0, delimiters);
            double longitude = strtof(strtok(0, delimiters), 0);
            double latitude = strtof(strtok(0, delimiters), 0);
            size_t asr_juristic = strtoul(strtok(0, delimiters), 0, 10);
            size_t adjust_high_lats_method = strtoul(strtok(0, delimiters), 0, 10);
            size_t method = 0;
            for(size_t i = 0; i < ST_cm_num; i++) {
                if(!strcmp(method_str, ST_cm_names[i]))
                    method = i;
            }

            current_city = city_init_calc(current_city, name, provider, method, id, longitude, latitude, asr_juristic, adjust_high_lats_method);
        }
        assert(current_city);
        cfg->counter_parsed_cities++;
        break;
    case spec_num_cities:
        cfg->num_cities = strtoul(str_result, 0, 10);
        assert(cfg->num_cities);
        cfg->cities = malloc(cfg->num_cities * sizeof(City));
        assert(cfg->cities);
        break;
    case spec_lang:
        cfg->lang = strtoul(str_result, 0, 10);
#ifdef OLDCODE
        if(str_result && (!strcmp(str_result, "\n") || !strcmp(str_result, ""))) {
            strcpy(cfg->lang,str_result);
        } else {
            strcpy(cfg->lang, "en");
        }
#endif
        break;
    case spec_pos:
        cfg->last_window_posX = strtoul(str_result, 0, 10);
        cfg->last_window_posY = strtoul(strtok(0, delimiters), 0, 10);
        assert(cfg->last_window_posX && cfg->last_window_posY);
        break;
    default:
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
#endif // CFG_USE_JSON

#ifndef CFG_USE_JSON
int config_read(char const* filename, Config* cfg)
{
    FILE* cfg_file = fopen(filename, "r");

    if(!cfg_file) {
        assert(cfg_file);
        return EXIT_FAILURE;
    }

    size_t buffer_length = 200;
    char buffer[buffer_length];

    while(fgets(buffer, buffer_length, cfg_file) && !feof(cfg_file)) {
        assert(strlen(buffer));
        if(config_parse_string(buffer, cfg) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }
    if(cfg->counter_parsed_cities != cfg->num_cities) {
        assert(cfg->counter_parsed_cities == cfg->num_cities);
        return EXIT_FAILURE;
    }

    cfg->config_changed = false;
    cfg->cfg_filename = filename;

    fclose(cfg_file);
    return EXIT_SUCCESS;
}
#else // CFG_USE_JSON
enum ST_calculation_method config_get_calc_method(const cJSON* element) {
	enum ST_calculation_method ret = ST_cm_Jafari;
	if(!element) return ret;
	for(size_t i = 0; i < ST_cm_num; i++) {
		if(!strcmp(element->valuestring, ST_cm_names[i])) {
			ret = i;
			break;
		}
	}
	return ret;
}

static int config_copy_cities(size_t num, City dest[num], City src[num]) {
	int ret = EXIT_FAILURE;
	if(num) {
		for(size_t i = 0; i < num; i++) {
			dest[i] = src[i];
		}
		ret = EXIT_SUCCESS;
	}
	return ret;
}

static size_t min(size_t const pos1, size_t const pos2) {
	assert(pos1 != pos2);
	if(pos1 < pos2) return pos1;
	return pos2;
}

static size_t max(size_t const pos1, size_t const pos2) {
	assert(pos1 != pos2);
	if(pos1 > pos2) return pos1;
	return pos2;
}

int config_move_city(size_t const id_initial, size_t const id_after, Config*const cfg) { // Todo verify function
	int ret = EXIT_FAILURE;
	if(cfg) {
		if(id_initial < cfg->num_cities && id_after < cfg->num_cities && id_initial != id_after) {
			int len = abs(id_initial - id_after);
			size_t start = min(id_initial, id_after);
			size_t end = max(id_initial, id_after);
			City buffer[len];

			size_t copy_start = start == id_initial ? start + 1 : start; // Copy from cfg to buffer. If city is to be moved backwards, copy from buffer to cfg startign at position start + 1.
			size_t fill_start = start == id_initial ? start : start + 1; // Copy from buffer to cfg. If city is to be moved backwards, copy from buffer to cfg startign at position start.
			City buffer2 = cfg->cities[id_initial];
			config_copy_cities(len, buffer, &(cfg->cities[copy_start]));
			config_copy_cities(len, &(cfg->cities[fill_start]), buffer);
			cfg->cities[id_after] = buffer2;

			for(size_t i = start; i <= end; i++) {
				cfg->cities[i].id = i;
			}/*
			for(size_t i = 0; i < cfg->num_cities; i++) {
				cfg->cities[i].id = i;
			}*/
			cfg->config_changed = true;
			ret = EXIT_SUCCESS;
		}
	}
	return ret;
}

int config_swap_cities(size_t const city1, size_t const city2, Config*const cfg) {
	int ret = EXIT_FAILURE;
	if(cfg) {
		if(city1 < cfg->num_cities && city2 < cfg->num_cities) {
			City city_buffer = cfg->cities[city1];
			cfg->cities[city1] = cfg->cities[city2];
			cfg->cities[city2] = city_buffer;
			cfg->cities[city1].id = city1; // Adjust ID of previously city 2
			cfg->cities[city2].id = city2; // Adjist ID of previously city 1
			cfg->config_changed = true;
			ret = EXIT_SUCCESS;
		}
	}
	return ret;
}

static bool config_calc_read_city(const cJSON* element, char* name, City* city) {
    bool ret = false;
    if(!element || !city || !name)
        return ret;

    cJSON* id = cJSON_GetObjectItem(element, config_format_json_specifiers[json_id]);
    cJSON* method = cJSON_GetObjectItem(element, config_format_json_specifiers[json_method]);
    cJSON* longitude = cJSON_GetObjectItem(element, config_format_json_specifiers[json_longitude]);
    cJSON* latitude = cJSON_GetObjectItem(element, config_format_json_specifiers[json_latitude]);
    cJSON* asr = cJSON_GetObjectItem(element, config_format_json_specifiers[json_asrmethod]);
    cJSON* high_lats = cJSON_GetObjectItem(element, config_format_json_specifiers[json_highlats]);
    assert(method); assert(longitude); assert(latitude); assert(asr); assert(high_lats);
    assert(id);

    City* c = city_init_calc(city, name, prov_calc, config_get_calc_method(method), id->valueint,
                             longitude->valuedouble, latitude->valuedouble, asr->valueint, high_lats->valueint);
    if(c) ret = true;
    return ret;
}

static bool config_diyanet_read_city(const cJSON* element, char* name, City* city) {
    bool ret = false;
    if(!element || !city || !name)
        return ret;

    cJSON* id = cJSON_GetObjectItem(element, config_format_json_specifiers[json_id]);
    cJSON* prayer_file = cJSON_GetObjectItem(element, config_format_json_specifiers[json_filename]);
    assert(prayer_file); assert(id);

    City* c = city_init_diyanet(city, name, prov_diyanet, prayer_file->valuestring, id->valueint);
    if(c) ret = true;

    return ret;
}

int config_read(char const* filename, Config* cfg)
{
    int ret = EXIT_FAILURE;
    FILE* cfg_file = fopen(filename, "r");

    if(!cfg_file) {
        return EXIT_FAILURE;
    }

    config_init(cfg);

    size_t file_length = file_find_length(cfg_file) + 1;
    if(!file_length) return EXIT_FAILURE;
    char* cfg_content = malloc(file_length * sizeof(char));
    if(!cfg_content) {
        return EXIT_FAILURE;
    }

    if(!file_read_all(cfg_file, file_length, cfg_content)) {
        return EXIT_FAILURE;
    }

    cJSON* json = cJSON_Parse(cfg_content);
    if(!json) {
    	myperror("Error parsing Config.");
        return EXIT_FAILURE;
    }

    cfg->cfg_filename = malloc(strlen(filename) * sizeof(char) + 1);
    if(!strcpy(cfg->cfg_filename, filename)) goto FREE_FILENAME;

    cJSON* json_ncities = cJSON_GetObjectItem(json, config_format_json_specifiers[json_num_cities]);
    if(json_ncities) {
        cfg->num_cities = json_ncities->valueint;
    }
    cfg->cities = malloc(cfg->num_cities * sizeof(City));
    assert(cfg->cities);

    cJSON* cities = cJSON_GetObjectItem(json, config_format_json_specifiers[json_cities]);
    if(cJSON_IsArray(cities)) {
        const cJSON* element = 0;
        cJSON_ArrayForEach(element, cities) {
            cJSON* city_name = cJSON_GetObjectItem(element, config_format_json_specifiers[json_name]);
            cJSON* provider = cJSON_GetObjectItem(element, config_format_json_specifiers[json_provider]);

            assert(city_name && provider);
            char* provider_name = provider->valuestring;
            City* current_city = &cfg->cities[cfg->counter_parsed_cities];
            if(!strcmp(provider_name, provider_names[prov_diyanet])) {
                if(!config_diyanet_read_city(element, city_name->valuestring, current_city))
                    goto FREE_CITIES;
            } else if(!strcmp(provider_name, provider_names[prov_calc])) {
                if(!config_calc_read_city(element, city_name->valuestring, current_city))
                    goto FREE_CITIES;
            }
            cfg->counter_parsed_cities++;
        }
        if(cfg->counter_parsed_cities != cfg->num_cities) {
        	myperror("Something went wrong while parsing cities. Not all cities could be parsed.\n");
        }
    }

    cJSON* savePos = cJSON_GetObjectItem(json, config_format_json_specifiers[json_savepos]);
    if(savePos && savePos->valueint) {
        cfg->save_position = true;
        cJSON* posX = cJSON_GetObjectItem(json, config_format_json_specifiers[json_posX]);
        cJSON* posY = cJSON_GetObjectItem(json, config_format_json_specifiers[json_posY]);
        if(posX && posY) {
            cfg->last_window_posX = posX->valueint;
            cfg->last_window_posY = posY->valueint;
        }
    }

    cJSON* checkForUpdates = cJSON_GetObjectItem(json, config_format_json_specifiers[json_check_for_updates]);
    if(checkForUpdates && checkForUpdates->valueint)
        cfg->check_for_updates = true;

    cJSON* enableNotifications = cJSON_GetObjectItem(json, config_format_json_specifiers[json_notifications]);
    if(enableNotifications && enableNotifications->valueint)
        cfg->enable_notification = true;

    cJSON* language = cJSON_GetObjectItem(json, config_format_json_specifiers[json_lang]);
    if(language)
        cfg->lang = language->valueint;

    ret = EXIT_SUCCESS;
    goto FREE_JSON;

FREE_CITIES:
    free(cfg->cities);
FREE_FILENAME:
	free(cfg->cfg_filename);
FREE_JSON:
    cJSON_Delete(json);
    fclose(cfg_file);

    return ret;
}
#endif

int config_save(char const*const filename, Config const*const cfg)
{
    if(!cfg || ! filename || !cfg->config_changed) return EXIT_FAILURE;

    FILE* cfg_file = fopen(filename, "w");

    if(!cfg_file) {
        assert(cfg_file);
        return EXIT_FAILURE;
    }

    fprintf(cfg_file, "%s:%zu\n", config_format_specifiers[spec_num_cities], cfg->num_cities);

    for(size_t i = 0; i < cfg->num_cities; i++) {
        char* format = "%s:%s;%s;%s;%zu;%s;%f;%f;%zu;%zu\n";    // [City]: City-Name; Provider-Name; Filename; ID; Method-Name; Longitude; Latitude; Asr-Juristic; Adjust high latitudes method
        City* curr_city = &cfg->cities[i];

        fprintf(cfg_file,
                format,
                config_format_specifiers[spec_city],
                curr_city->name,
                provider_names[curr_city->pr_time_provider],
                curr_city->filename ? curr_city->filename : " ",
                curr_city->id,
                curr_city->pr_time_provider == prov_calc ? ST_cm_names[curr_city->method] : " ",
                curr_city->longitude,
                curr_city->latitude,
                curr_city->asr_juristic,
                curr_city->adjust_high_lats);
    }

    fprintf(cfg_file, "%s:%zu\n", config_format_specifiers[spec_lang], cfg->lang);
    fprintf(cfg_file, "%s:%d;%d\n", config_format_specifiers[spec_pos], cfg->last_window_posX, cfg->last_window_posY);

    fclose(cfg_file);
    return EXIT_SUCCESS;
}

int config_json_save(char const*const filename, Config*const cfg) {
    int ret = EXIT_FAILURE;
    if(!cfg || !cfg->config_changed || !filename) return ret;

    FILE* cfg_file = fopen(filename, "w");

    if(!cfg_file) {
        assert(cfg_file);
        return EXIT_FAILURE;
    }

    cJSON* json = cJSON_CreateObject();
    if(!json) goto ERR;

    if(!cJSON_AddNumberToObject(json, config_format_json_specifiers[json_num_cities], cfg->num_cities)) goto ERR;
    if(!cJSON_AddNumberToObject(json, config_format_json_specifiers[json_posX], cfg->last_window_posX)) goto ERR;
    if(!cJSON_AddNumberToObject(json, config_format_json_specifiers[json_posY], cfg->last_window_posY)) goto ERR;
    if(!cJSON_AddNumberToObject(json, config_format_json_specifiers[json_lang], cfg->lang)) goto ERR;
    if(!cJSON_AddBoolToObject(json, config_format_json_specifiers[json_savepos], cfg->save_position)) goto ERR;
    if(!cJSON_AddBoolToObject(json, config_format_json_specifiers[json_check_for_updates], cfg->check_for_updates)) goto ERR;
    if(!cJSON_AddBoolToObject(json, config_format_json_specifiers[json_notifications], cfg->enable_notification)) goto ERR;

    cJSON* cities = cJSON_CreateArray();
    if(!cities) goto ERR;
    cJSON_AddItemToObject(json, config_format_json_specifiers[json_cities], cities);

    for(size_t i = 0; i < cfg->num_cities; i++) {
        cJSON* city = cJSON_CreateObject();
        if(!cJSON_AddStringToObject(city, config_format_json_specifiers[json_name], cfg->cities[i].name)) goto ERR;
        if(!cJSON_AddStringToObject(city, config_format_json_specifiers[json_provider], provider_names[cfg->cities[i].pr_time_provider])) goto ERR;
        if(!cJSON_AddNumberToObject(city, config_format_json_specifiers[json_id], cfg->cities[i].id)) goto ERR;
        char const*const calc_str = cfg->cities[i].pr_time_provider == prov_calc ? ST_cm_names[cfg->cities[i].method] : " ";
        if(!cJSON_AddStringToObject(city, config_format_json_specifiers[json_method], calc_str)) goto ERR;
        if(!cJSON_AddNumberToObject(city, config_format_json_specifiers[json_longitude], cfg->cities[i].longitude)) goto ERR;
        if(!cJSON_AddNumberToObject(city, config_format_json_specifiers[json_latitude], cfg->cities[i].latitude)) goto ERR;
        if(!cJSON_AddNumberToObject(city, config_format_json_specifiers[json_asrmethod], cfg->cities[i].asr_juristic)) goto ERR;
        if(!cJSON_AddNumberToObject(city, config_format_json_specifiers[json_highlats], cfg->cities[i].adjust_high_lats)) goto ERR;
        char const*const filename_prayers = cfg->cities[i].pr_time_provider == prov_diyanet ? cfg->cities[i].filename : " ";
        if(!cJSON_AddStringToObject(city, config_format_json_specifiers[json_filename], filename_prayers)) goto ERR;
        cJSON_AddItemToArray(cities, city);
    }

    char* string = cJSON_Print(json);
    if(!string) {
    	myperror("Error printing json");
        goto ERR;
    }
    fprintf(cfg_file, "%s\n", string);
    free(string);
    string = 0;

    cfg->config_changed = false;
    goto END;
ERR:
	myperror("Error while printing config json");
END:
    cJSON_Delete(json);
    fclose(cfg_file);
    return ret;
}

Config* config_init(Config* cfg)
{
    if(cfg) {
        *cfg = (Config) {
            .lang = LANG_EN,
            .config_changed = false,
        };
        //cfg->lang = LANG_EN;
        //cfg->config_changed = false;
    }
    return cfg;
}

/** \brief Returns true if ID is existing in cfg, false otherwise
 *
 * \param city_id size_t
 * \param cfg Config const*const
 * \return bool
 *
 */
static bool is_id_valid(size_t city_id, Config const*const cfg)
{
    for(size_t i = 0; i < cfg->num_cities; i++) {
        if(cfg->cities[i].id == city_id) return true;
    }
    return false;
}

static size_t config_find_last_used_id(Config const*const cfg) {
	return cfg->num_cities -1;
}

static size_t config_get_first_free_id(Config const*const cfg) {
	return cfg->num_cities;
}

size_t config_find_idpos(size_t city_id, Config const*const cfg)
{
    size_t ret = -1;
    if(is_id_valid(city_id, cfg)) {
        for(size_t i = 0; i < cfg->num_cities; i++) {
            if(cfg->cities[i].id == city_id) return i;
        }
    }
    return ret;
}

size_t config_find_next_id(Config const*const cfg)
{
    size_t next_id = 0;
    for(size_t i = 0; i <= cfg->num_cities; i++) {
        if(!is_id_valid(i, cfg)) {
            next_id = i;
            break;
        }
    }
    return next_id;
}

int config_add_city(City c, Config* cfg)
{
    int ret = EXIT_FAILURE;
    if(cfg) {
        City* new_pointer = realloc(cfg->cities, (cfg->num_cities + 1) * sizeof(City));
        if(!new_pointer) return EXIT_FAILURE;
        cfg->cities = new_pointer;
        c.id = config_get_first_free_id(cfg);
        cfg->cities[cfg->num_cities] = c;
        cfg->num_cities++;
        cfg->counter_parsed_cities++;
        cfg->config_changed = true;
        ret = EXIT_SUCCESS;
    }
    return ret;
}

int config_remove_city(size_t city_id, Config* cfg)
{
    int ret = EXIT_FAILURE;
    if(cfg) {
    	config_move_city(city_id, config_find_last_used_id(cfg), cfg);

        City buffer[cfg->num_cities - 1];
        for(size_t i = 0; i < cfg->num_cities - 1; i++) {
            buffer[i] = cfg->cities[i];
            buffer[i].id = i;
        }

        cfg->num_cities--;
        City* new_pointer = realloc(cfg->cities, cfg->num_cities * sizeof(City));     // Shrink size, should not fail
		if(!new_pointer) return EXIT_FAILURE;
		cfg->cities = new_pointer;

		for(size_t i = 0; i < cfg->num_cities; i++) {
			cfg->cities[i] = buffer[i];
		}
		cfg->config_changed = true;
		ret = EXIT_SUCCESS;
    }
    return ret;
}

Config* config_clear(Config* cfg)
{
    if(cfg) {
        free(cfg->cities);
    }
    return config_init(cfg);
}
