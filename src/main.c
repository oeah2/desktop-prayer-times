#include <stdio.h>
#include <stdlib.h>
//#define NDEBUG 1
#include <assert.h>
#include <stdbool.h>
#include <locale.h>

#include "prayer_times.h"
#include "config.h"
#include "socket.h"
#include "gui.h"

extern calc_function* calc_functions[];

enum Version {
    VERSION_MAIN_MAJOR = 0,
    VERSION_MAIN_MINOR = 1,
};

int main(int argc, char** argv)
{
    Config config = {0};
    char *config_file = "Config.cfg.txt";
    prayer prayer_times[prayers_num];

    int res = config_read(config_file, &config);
    assert(res == EXIT_SUCCESS);
    if(res == EXIT_FAILURE) {
        config = *config_init(&config);
        // Do further initialization to add cities etc.
    }

    setlocale(LC_ALL, config.lang);

    /* Um den Code in der Loop einfacher zu halten */
    calc_function* calc_func = 0;
    City* curr_city = 0;

    enum CityIDs {
        ID_LP = 0,
        ID_LP_calc = 1,
        ID_unknown = 6,
    };

#if TESTCITY
    City newCity = *city_init_calc(&newCity, "TestCity", prov_calc, ST_cm_Makkah, ID_unknown, 0, 0);
    config_add_city(newCity, &config);
    config_remove_city(ID_unknown, &config);

    City lp = *city_init_diyanet(&lp, "Lippstadt", prov_diyanet, "10188.JSON", ID_LP);
    config_add_city(lp, &config);
    City lp_calc = *city_init_calc(&lp_calc, "Lippstadt_calc", prov_calc, ST_cm_MWL, ID_LP_calc, 8.346650, 51.676380, ST_jm_Shafii, ST_am_None);
    config_add_city(lp_calc, &config);
#endif

    //config_remove_city(6, &config);

    for(size_t i = 0; i < config.num_cities; i++) {
        int ret = 0;
        assert(calc_functions[config.cities[i].pr_time_provider]);
        curr_city = &config.cities[i];
        calc_func = calc_functions[curr_city->pr_time_provider];
CALC_TIMES:
        ret = calc_func(config.cities[i], prayer_times);
        switch(ret) {
        case ENOFILE:   // In case of no file, the behaviour is the same like with EOF
        case EOF:
            /*******************************
                Implement update of file
            ********************************/
            perror("Main: end of file reached!");
#if ACTIVATE_WHEN_FILEUPDATE_IS_IMPLEMENTED
            goto CALC_TIMES;
#endif
            break;
        case EFAULT:
            // Should not occur, prayer times of past
            break;
        case EXIT_SUCCESS:
            break;
        default:
            // Should not occur
            assert(0);
        }
        prayer_print_times(curr_city->name, prayer_times);
    }

    config_save(config_file, &config);

    /* GTK */
    /* Call the gtk initialization function */
    gtk_init(&argc, &argv);

    /* Call our function to build the complete program window. */
    build_window();

    /* Enter Gtk+ main event loop (sleeps waiting for input events,
     * like mouse or keyboard input, then wakes up calling the registered
     * event callbacks and goes back to sleep until exit is requested). */
    gtk_main();

    return 0;
}
