#include "config.h"
#include "socket.h"

enum config_specifiers {
    spec_num_cities,
    spec_city,
    spec_lang,
    spec_pos,
    spec_num
};

static char const*const config_format_specifiers[] = {
    [spec_num_cities] = "[num_cities]",
    [spec_city] = "[city]",
    [spec_lang] = "[language]",
    [spec_pos] = "[pos]",
};

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

int config_save(char const*const filename, Config const*const cfg)
{
    if(!cfg->config_changed) return EXIT_FAILURE;

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

Config* config_init(Config* cfg)
{
    if(cfg) {
        cfg->cities = 0;
        cfg->counter_parsed_cities = 0;
        cfg->lang = LANG_EN;
        cfg->num_cities = 0;
        cfg->config_changed = false;
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
    if(cfg && !is_id_valid(c.id, cfg)) {
        City* new_pointer = realloc(cfg->cities, (cfg->num_cities + 1) * sizeof(City));
        if(!new_pointer) return EXIT_FAILURE;
        cfg->cities = new_pointer;
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
    if(cfg && is_id_valid(city_id, cfg)) {
        City buffer[cfg->num_cities - 1];
        for(size_t i = 0, counter = 0; i < cfg->num_cities; i++) {       // Copy cities to array
            if(cfg->cities[i].id == city_id) continue;
            buffer[counter] = cfg->cities[i];
            counter++;
        }
        cfg->num_cities--;
        City* new_pointer = realloc(cfg->cities, cfg->num_cities * sizeof(City));     // Shrink size, should not fail
        if(!new_pointer) return EXIT_FAILURE;
        cfg->cities = new_pointer;

        for(size_t i = 0; i < cfg->num_cities; i++) {           // Copy array back to config
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
