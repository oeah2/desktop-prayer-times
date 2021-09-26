/*
 * error.c
 *
 *  Created on: 26.09.2021
 *      Author: ahmet
 */

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <error.h>
#include "error.h"


void myperror(char const*const msg) {
	static bool firstrun = false;
	if(msg) {
		if(!firstrun) {
			firstrun = true;
			time_t t = time(0);
			struct tm tm = *localtime(&t);
			char buffer[50];
			sprintf(buffer, "\n\nToday: %02d.%02d.%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
			perror(buffer);
		}
	perror(msg);
	}
}
