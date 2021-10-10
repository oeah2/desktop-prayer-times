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
//#define NDEBUG 1
#include <assert.h>
#include <stdbool.h>
#include <uchar.h>

#include "prayer_times.h"
#include "config.h"
#include "socket.h"
#include "gui.h"
#include "geolocation.h"
#include "update.h"
#include "error.h"

#ifdef _WIN32
#include <windows.h>
#include <locale.h>
#else
#include <stdlib.h>
#include <locale.h>
#endif // _WIN32

#define WAIT_USER
#ifdef WAIT_USER
#ifdef _WIN32
#include <winuser.h>
#endif // _WIN32
#endif // WAIT_USER

#include "cJSON.h"

extern calc_function* calc_functions[];

int main(int argc, char** argv)
{
//#define ENABLE_LOG
#ifdef ENABLE_LOG
    char const*const log_filename = "errors.log";
    if(!freopen(log_filename, "a+", stderr)) {
    	myperror("Error opening log file");
    }
#endif // ENABLE_LOG

    Config config = {0};
    char* config_file = "Config.cfg";
    prayer prayer_times[prayers_num];

    {
		int cfg_read_result = config_read(config_file, &config);
		if(cfg_read_result == EXIT_FAILURE || !config.num_cities) {
			if(cfg_read_result == EXIT_FAILURE) config = *config_init(&config);
			// Create Makkah as reference city
			City c;
			city_init_calc(&c, "Makkah", prov_calc, ST_cm_Makkah, 0, 39.814838, 21.427378, ST_jm_Shafii, ST_am_None);
			config_add_city(c, &config);
		}
    }

#define SKIP_UPDATE
#ifndef SKIP_UPDATE
    if(config.check_for_updates) {
        if(update_check_is_available()) {
            // Update
            char* version_this = update_get_current_version();
            char* version_available = update_get_available_version();
            if(version_this && version_available) {
                puts("Update available!");
                printf("Your version: %s\nAvailable version: %s\n", version_this, version_available);
                fflush(stdout);
            }
            if(version_this) free(version_this);
            if(version_available) free(version_available);
        } else {
            puts("No updates available");
        }
    }
#endif // SKIP_UPDATE

#ifdef CONSOLE
#ifdef _WIN32
    //SetConsoleOutputCP(65001);  // Set windows console to UTF8
    SetConsoleCP(65001);
#else
    //setlocale(LC_ALL, lang_names[config.lang]); // Set Unix console to locale
    setlocale(LC_ALL, "en_US.UTF-8");
#endif // _WIN32
#endif // CONSOLE

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

//#define TEST_IMAGE_DOWNLOAD
#ifdef TEST_IMAGE_DOWNLOAD
    char* ezanvakti_resp = "http://namazvakti.diyanet.gov.tr/images/r3.gif";
    char* host = "namazvakti.diyanet.gov.tr";
    char* file = ezanvakti_resp + strlen(host);
    char* data = http_get(host, file, NULL);
    puts(data);
    return 0;
#endif // TEST_IMAGE_DOWNLOAD

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
        if(config.num_cities) {
			curr_city = &config.cities[0];
			calc_func = calc_functions[curr_city->pr_time_provider];
			int CALC_RESULT = calc_func(config.cities[0], prayer_times);
			if(CALC_RESULT == ENETDOWN) {
				config.cities[0].pr_time_provider = prov_empty;
				calc_functions[curr_city->pr_time_provider](config.cities[0], prayer_times);              // Reset to 00:00
			} else if(CALC_RESULT != EXIT_SUCCESS) {
				assert(0);
			}
        }
#endif // CALC_ALL_CITIES

    }

#ifdef WAIT_USER
#ifdef _WIN32
    HCURSOR h_old_cursor = NULL, h_new_cursor = NULL;      // cursor backup
    puts("Cursor set");
    h_new_cursor = LoadCursor(NULL, IDC_APPSTARTING);
    h_old_cursor = SetCursor(h_new_cursor);
    assert(h_new_cursor);
#endif // _WIN32
#endif // WAIT_USER

    config_json_save(config_file, &config);

    size_t const buff_len = 15;
    char prayer_puffer[prayers_num][buff_len];
    for(size_t i = 0; i < prayers_num; i++) {
        sprint_prayer_time(prayer_times[i], buff_len, prayer_puffer[i]);
    }
    char puffer_julian_date[buff_len];
    char puffer_hijri_date[buff_len];
    prayer pr_hijri_date = {0};
    if(config.cities[0].pr_time_provider == prov_calc) {
        time_t now = time(0);
        struct tm tm = *localtime(&now);
        pr_hijri_date.hicri_date = calc_get_hijri_date(tm); // Todo, das muss auch für Diyanet erfolgen
	}

    sprint_prayer_date(prayer_times[0], buff_len, puffer_julian_date, false);
    sprint_prayer_date(pr_hijri_date, buff_len, puffer_hijri_date, true);

    char* gui_strings[gui_id_num] = {
        [gui_id_fajr_time]      = prayer_puffer[pr_fajr],
        [gui_id_fajrend]        = prayer_puffer[pr_fajr_end],
        [gui_id_dhuhr_time]     = prayer_puffer[pr_dhuhr],
        [gui_id_asr_time]       = prayer_puffer[pr_asr],
        [gui_id_maghrib_time]   = prayer_puffer[pr_maghreb],
        [gui_id_isha_time]      = prayer_puffer[pr_ishaa],
        [gui_id_sunrise_time]   = prayer_puffer[pr_sunrise],
        [gui_id_sunset_time]    = prayer_puffer[pr_sunset],
        [gui_id_cityname]       = config.cities[0].name,
        [gui_id_datename]       = puffer_julian_date,
        [gui_id_hijridate]      = puffer_hijri_date,
        [gui_id_remainingtime]  = "",
    };

    /* Now GUI Stuff */
    gtk_init(&argc, &argv);
    char* glade_filename = 0;
    if(lang_is_available(config.lang)) {
    	glade_filename = lang_get_filename(config.lang);
    } else {
    	char buffer[100];
    	sprintf(buffer, "%s: %d, Error finding glade file. Lang: %ld", __FILE__, __LINE__, config.lang);
    	myperror(buffer);

    	// Switch to english
        if(lang_is_available(LANG_EN)) {
        	glade_filename = lang_get_filename(LANG_EN);

        	sprintf(buffer, "%s: %d, English is available, switching to english.", __FILE__, __LINE__);
        	myperror(buffer);
        }
        else
        	return EXIT_FAILURE;
    }
    build_glade(&config, gui_id_num, glade_filename, gui_strings);
    free(glade_filename);

#ifdef WAIT_USER
#ifdef _WIN32
    SetCursor(h_old_cursor);
#endif // _WIN32
#endif // WAIT_USER

    gtk_main();
    if(config.config_changed)
        config_json_save(config.cfg_filename, &config);

    if(config.num_cities)
    	free(config.cities);
    free(config.cfg_filename);

    return 0;
}
