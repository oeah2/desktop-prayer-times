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

int main(int argc, char** argv)
{
    Config config = {0};
    char *config_file = "Config.cfg.txt";
    prayer prayer_times[prayers_num];

    int res = config_read(config_file, &config);
    assert(res == EXIT_SUCCESS);
    if(res == EXIT_FAILURE) {
        //config = init_config(config);
        // Do further initialization to add cities etc.
    }

    setlocale(LC_ALL, config.lang);

    /* Um den Code in der Loop einfacher zu halten */
    calc_function* calc_func = 0;
    City* curr_city = 0;

    for(size_t i = 0; i < config.num_cities; i++) {
        int ret = 0;
        assert(calc_functions[config.cities[i].pr_time_provider]);
        curr_city = &config.cities[i];
        calc_func = calc_functions[curr_city->pr_time_provider];
CALC_TIMES:
        ret = calc_func(&config, curr_city->id, prayer_times);
        switch(ret) {
        case EOF:
            // UPdate file
            goto CALC_TIMES;
            break;
        case ENOFILE:
            // Update file
            goto CALC_TIMES;
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

        //calc_functions[config.cities[i].pr_time_provider](&config, config.cities[i].id, prayer_times);
        //prayer_print_times(config.cities[i].name, prayer_times);
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
