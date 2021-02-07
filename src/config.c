#include "config.h"

static enum config_specifiers {spec_num_cities, spec_city, spec_lang, spec_num};
static char const*const config_format_specifiers[] = {
    [spec_num_cities] = "[num_cities]",
    [spec_city] = "[city]",
    [spec_lang] = "[language]",
};

static char const*const format_config[] = {
    [spec_city] = "%s;%s;%u",               // Cityname, calc-method, path to file, city_id
    [spec_num_cities] = "%u",
    [spec_lang] = "%s",                     // Filename
};

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
            size_t method = 0;
            for(size_t i = 0; i < calculation_method_num; i++) {
                if(!strcmp(method_str, method_names[i]))
                    method = i;
            }

            current_city = city_init_calc(current_city, name, provider, method, id, longitude, latitude);
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
        if(str_result && (!strcmp(str_result, "\n") || !strcmp(str_result, ""))) {
            strcpy(cfg->lang,str_result);
        } else {
            strcpy(cfg->lang, "en");
        }
        break;
    default:
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int config_read(char const*const filename, Config* cfg)
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

    fclose(cfg_file);
    return EXIT_SUCCESS;
}

int config_save(char const*const filename, Config const* cfg)
{
    if(!cfg->config_changed) return EXIT_FAILURE;

    FILE* cfg_file = fopen(filename, "w");

    if(!cfg_file) {
        assert(cfg_file);
        return EXIT_FAILURE;
    }

    fprintf(cfg_file, "%s:%zu\n", config_format_specifiers[spec_num_cities], cfg->num_cities);

    for(size_t i = 0; i < cfg->num_cities; i++) {
        char* format = "%s:%s;%s;%s;%zu;%s;%f;%f\n";    // City-Name; Provider-Name; Filename; ID; Method-Name; Longitude; Latitude
        City* curr_city = &cfg->cities[i];
        switch(cfg->cities[i].pr_time_provider) {
        case prov_diyanet:
            fprintf(cfg_file,
                    format,
                    config_format_specifiers[spec_city],
                    curr_city->name,
                    provider_names[curr_city->pr_time_provider],
                    curr_city->filename,
                    curr_city->id,
                    "",
                    0,
                    0);
            break;
        case prov_calc:
            fprintf(cfg_file,
                    format,
                    config_format_specifiers[spec_city],
                    curr_city->name,
                    provider_names[curr_city->pr_time_provider],
                    "",
                    curr_city->id,
                    method_names[curr_city->method],
                    curr_city->longitude,
                    curr_city->latitude);
            break;
        }
    }

    fprintf(cfg_file, "%s:%s\n", config_format_specifiers[spec_lang], cfg->lang);

    fclose(cfg_file);
    return EXIT_SUCCESS;
}

/**********************
** Binary read and save currently not implemented.
*********************/

/*
int config_save_binary(char const*const filename, Config cfg)
{
    assert(filename);
    if(!filename) return -EFAULT;
    //if(!cfg) return -EFAULT;

    FILE* file = fopen(filename, "wb");
    assert(file);
    if(!file) return -EFAULT;

    int err = 0;
    for(size_t i = 0; i < cfg.num_cities; i++) {
        cfg.cities[i].file_times = 0;
    }

    City* cities = cfg.cities;
    cfg.cities = 0;
    char *city_names[cfg.num_cities];
    char *file_names[cfg.num_cities];

    for(size_t i = 0; i < cfg.num_cities; i++) {
        city_names[i] = cities[i].name;
        cities[i].name = 0;
        file_names[i] = cities[i].filename;
        cities[i].filename = 0;
    }

    if(fwrite(&cfg, sizeof(cfg), 1, file) != sizeof(cfg)) { // Save struct Config
        err = ferror(file);
        goto RET_FILE;
    }
    if(fwrite(cities, sizeof(City), cfg.num_cities, file) != cfg.num_cities * sizeof(City)) { // Save array of Struct City
        err = ferror(file);
        goto RET_FILE;
    }

    goto RET_FILE;

RET_FILE:
    fclose(file);
    return err;
}

int config_read_binary(char const*const filename, Config* cfg)
{
    assert(filename && cfg);
    if(!filename) return -EFAULT;
    if(!cfg) return -EFAULT;

    FILE* file = fopen(filename, "rb");
    int err = 0;
    if(fread(cfg, sizeof(Config), 1, file) != sizeof(Config)) {
        err = ferror(file);
        goto RET_FILE;
    }

    return err;

    for(size_t i = 0; i < cfg->num_cities; i++) {
        cfg->cities[i].filename;
    }

RET_FILE:
    fclose(file);
    return err;
}
*/

Config* config_init(Config* cfg)
{
    if(cfg) {
        cfg->cities = 0;
        cfg->counter_parsed_cities = 0;
        strcpy(cfg->lang, "en");
        cfg->num_cities = 0;
        cfg->config_changed = false;
    }
    return cfg;
}

size_t config_find_next_id(Config const* cfg)
{
    size_t next_id = 0;
    if(cfg && cfg->num_cities) {
        next_id = cfg->cities[cfg->num_cities - 1].id + 1;
    }
    return next_id;
}


int config_add_city(City c, Config* cfg)
{
    int ret = EXIT_FAILURE;
    if(cfg) {
        cfg->cities = realloc(cfg->cities, (cfg->num_cities + 1) * sizeof(City));
        cfg->cities[cfg->num_cities] = c;
        cfg->num_cities++;
        cfg->counter_parsed_cities++;
        cfg->config_changed = true;
    }
    return ret;
}

Config* config_remove_city(size_t city_id, Config* cfg)
{
    /*    if(cfg) {
            for(size_t i = 0; i < cfg->num_cities; i++) {
                if(cfg->cities[i].id == city_id) {

                }
            }
        }
    */
    return cfg;
}

Config* config_clear(Config* cfg)
{
    if(cfg) {
        free(cfg->cities);
    }
    return config_init(cfg);
}
