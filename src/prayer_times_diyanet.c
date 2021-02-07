#include "prayer_times.h"

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

static struct tm diyanet_mk_prayer_time(char* hr, char* date)
{
    char* current_pos = date;
    do {
        current_pos++;
    } while(!isdigit(*current_pos));

    struct tm tm = {0};
    if(sscanf(current_pos, "%d.%d.%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year) != 3) {       // 3 numbers to be detected
        perror("Error: mk_prayer_time could not parse date");
        exit(EXIT_FAILURE);
    }
    tm.tm_mon--;
    tm.tm_year -= 1900;

    current_pos = hr;
    do {
        current_pos++;
    } while(!isdigit(*current_pos));

    if(sscanf(current_pos, "%d:%d", &tm.tm_hour, &tm.tm_min) != 2) {                  // 2 number to be detected
        perror("Error: mk_prayer_time could not parse hours");
        exit(EXIT_FAILURE);
    }
    return tm;
}

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
            perror("Format not accepted.");
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

        for(size_t i = 0; i < prayers_num; i++) {
            times[i].time_at = diyanet_mk_prayer_time(pos_prayers[i], pos_date);
            times[i].name = prayer_names[i];
        }
        ret = EXIT_SUCCESS;
    }
    return ret;
}

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
        perror("Error reading file.");
        assert(0);
        return ENOFILE;
    }

    if(EOF == fgetc(file_times) && feof(file_times) && !strlen(buffer)) {
        /* end of file reached, update file */
        perror("End of file reached.");
        assert(0);
        return EOF;
    }

    size_t rewind_neg = 0;
    if(EXIT_FAILURE == diyanet_parse_prayer_times(buffer, times, &rewind_neg)) {
        perror("Error while prasing prayer times.");
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

int diyanet_get_todays_prayers(Config* cfg, size_t city_id, prayer prayer_times[prayers_num])
{
    if(!cfg ||!prayer_times) {
        return EXIT_FAILURE;
    }
    size_t pos = 0;
    for(size_t i = 0; i < cfg->num_cities; i++) {
        if(cfg->cities[i].id == city_id) {
            pos = i;
            rewind(cfg->cities[i].file_times);
            break;
        }
    }

    return diyanet_get_prayer_times_for_date(cfg->cities[pos].file_times, prayer_times, 0);
}

int diyanet_get_preview_for_date(Config* cfg, size_t city_id, prayer prayer_times[prayers_num], struct tm date)
{
    if(!cfg || !prayer_times) {
        return EXIT_FAILURE;
    }
    date.tm_hour = 0;
    date.tm_min = 0;
    date.tm_sec = 0;

    size_t pos = 0;
    for(size_t i = 0; i < cfg->num_cities; i++) {
        if(cfg->cities[i].id == city_id) {
            pos = i;
            rewind(cfg->cities[i].file_times);
            break;
        }
    }

    return diyanet_get_prayer_times_for_date(cfg->cities[pos].file_times, prayer_times, mktime(&date));
}

int diyanet_get_preview_prayers(Config* cfg, size_t city_id, size_t days, prayer prayer_times[days][prayers_num])
{
    if(!cfg || !prayer_times || !days) {
        return EXIT_FAILURE;
    }
    double const secs_per_day = 60*60*24;
    double const max_days = 30;
    int ret = EXIT_SUCCESS;

    if(days > max_days) {
        perror("Date too far away.");
        return EXIT_FAILURE;
    }

    time_t current_time = time(0);
    struct tm tm = *localtime(&current_time);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    size_t pos = 0;
    for(size_t i = 0; i < cfg->num_cities; i++) {
        if(cfg->cities[i].id == city_id) {
            pos = i;
            rewind(cfg->cities[i].file_times);
            break;
        }
    }

    for(size_t i = 0; i < days; i++) {
        if(diyanet_get_prayer_times_for_date(cfg->cities[pos].file_times, prayer_times[i], mktime(&tm) + secs_per_day * (i + 1)) == EXIT_FAILURE) {
            ret = EXIT_FAILURE;
            break;
        }
    }
    return ret;
}
