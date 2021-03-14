#include <stdio.h>
//#define NDEBUG 1
#include <assert.h>
#include <stdbool.h>
#include <uchar.h>

#include "prayer_times.h"
#include "config.h"
#include "socket.h"
#include "gui.h"
#include "geolocation.h"

#ifdef _WIN32
#include <windows.h>
#include <locale.h>
#else
#include <stdlib.h>
#include <locale.h>
#endif // _WIN32

#include "cJSON.h"

extern calc_function* calc_functions[];

int main(int argc, char** argv)
{
    Config config = {0};
    char* config_file = "Config.cfg";
    prayer prayer_times[prayers_num];

    if(config_read(config_file, &config) == EXIT_FAILURE) {
        config = *config_init(&config);
        // Do further initialization to add cities etc.
    }

#ifdef _WIN32
    //SetConsoleOutputCP(65001);  // Set windows console to UTF8
    SetConsoleCP(65001);
#else
    //setlocale(LC_ALL, lang_names[config.lang]); // Set Unix console to locale
    setlocale(LC_ALL, "en_US.UTF-8");
#endif // _WIN32

//#define ADD_TESTCITIES
#ifdef ADD_TESTCITIES
    enum CityIDs {
        ID_LP = 0,
        ID_LP_calc = 1,
        ID_unknown = 6,
    };

    City newCity = *city_init_calc(&newCity, "TestCity", prov_calc, ST_cm_Makkah, ID_unknown, 0, 0);
    config_add_city(newCity, &config);
    config_remove_city(ID_unknown, &config);

    City lp = *city_init_diyanet(&lp, "Lippstadt", prov_diyanet, "10188.JSON", ID_LP);
    config_add_city(lp, &config);
    City lp_calc = *city_init_calc(&lp_calc, "Lippstadt_calc", prov_calc, ST_cm_MWL, ID_LP_calc, 8.346650, 51.676380, ST_jm_Shafii, ST_am_None);
    config_add_city(lp_calc, &config);
#endif

//#define TEST_REMOVE_CITY
#ifdef TEST_REMOVE_CITY
    config_remove_city(ID_LP_calc, &config);
#endif // TEST_REMOVE_CITY

//#define DIYANET_GET_COUNTRIES
#ifdef DIYANET_GET_COUNTRIES
    char* diyanet_country_codes = diyanet_get_country_codes(LANG_DE);
    puts(diyanet_country_codes);
    free(diyanet_country_codes);
    diyanet_country_codes = 0;
#endif // DIYANET_GET_COUNTRIES

//#define DIYANET_GET_PROVINCES
#ifdef DIYANET_GET_PROVINCES
    char* diyanet_province_codes = diyanet_get_provinces(13, LANG_DE);
    puts(diyanet_province_codes);
    free(diyanet_province_codes);
    diyanet_province_codes = 0;
#endif // DIYANET_GET_PROVINCES

//#define DIYANET_GET_CITIES
#ifdef DIYANET_GET_CITIES
    char* diyanet_city_codes = diyanet_get_cities(859, LANG_DE);
    puts(diyanet_city_codes);
    free(diyanet_city_codes);
    diyanet_city_codes = 0;
#endif // DIYANET_GET_CITIES

//#define TEST_GEOLOCATION
#ifdef TEST_GEOLOCATION

    while(1) {
        size_t buff_size = 200;
        char buffer[buff_size];
        memset(buffer, 0, buff_size);
        printf("Write a city name: ");
        fgets(buffer, buff_size, stdin);
        if(strchr(buffer, '\n')) *strchr(buffer, '\n') = '\0';

        char* results = geolocation_get(buffer);
        puts(results);
        free(results);

    }

#endif // TEST_GEOLOCATION

    calc_function* calc_func = 0;
    {
        City* curr_city = 0;
#ifdef CALC_ALL_CITIES
        for(size_t i = 0; i < config.num_cities; i++) {
            assert(calc_functions[config.cities[i].pr_time_provider]);
            curr_city = &config.cities[i];
            calc_func = calc_functions[curr_city->pr_time_provider];
            switch(calc_func(config.cities[i], prayer_times)) {
            case EXIT_SUCCESS:
                break;
            default:
                // Should not occur
                assert(0);
            }
            prayer_print_times(curr_city->name, prayer_times);
        }
#else // CALC_ALL_CITIES
        curr_city = &config.cities[0];
        calc_func = calc_functions[curr_city->pr_time_provider];
        if(calc_func(config.cities[0], prayer_times) != EXIT_SUCCESS) {
            assert(0);
        }
#endif // CALC_ALL_CITIES

    }

    config_save(config_file, &config);

    char prayer_puffer[prayers_num][6];
    for(size_t i = 0; i < prayers_num; i++) {
        sprintf(prayer_puffer[i], "%2d:%2d", prayer_times[i].time_at.tm_hour, prayer_times[i].time_at.tm_min);
    }
    char puffer_date[15];
    sprintf(puffer_date, "%2d.%2d.%4d", prayer_times->time_at.tm_mday, prayer_times->time_at.tm_mon +1, prayer_times->time_at.tm_year + 1900);

    char* gui_strings[gui_id_num] = {
        [gui_id_fajr_name]      = "Fajr",
        [gui_id_sunrise_name]   = "Sunrise",
        [gui_id_dhuhr_name]     = "Dhuhr",
        [gui_id_asr_name]       = "Asr",
        [gui_id_maghrib_name]   = "Maghrib",
        [gui_id_isha_name]      = "Isha",
        [gui_id_fajr_time]      = prayer_puffer[pr_fajr],
        [gui_id_sunrise_time]   = prayer_puffer[pr_fajr_end],
        [gui_id_dhuhr_time]     = prayer_puffer[pr_dhuhr],
        [gui_id_asr_time]       = prayer_puffer[pr_asr],
        [gui_id_maghrib_time]   = prayer_puffer[pr_maghreb],
        [gui_id_isha_time]      = prayer_puffer[pr_ishaa],
        [gui_id_cityname]       = config.cities[0].name,
        [gui_id_datename]       = puffer_date,
        [gui_id_hijridate]      = "Heute Hijri",
        [gui_id_remainingtime]  = "Noch bisschen",
        //[gui_id_randomhadith]   = "Hadith",
    };

    /* Now GUI Stuff */
    gtk_init(&argc, &argv);
    //build_assistant_glade();
    build_glade(&config, gui_id_num, gui_strings);

    gtk_main();

    return 0;
}
