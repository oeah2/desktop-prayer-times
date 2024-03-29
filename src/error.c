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

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include "error.h"
#include "update.h"


void myperror(char const*const file, const int line, char const*const msg) {
	static bool firstrun = false;
	if(msg && file) {
		if(!firstrun) {
			firstrun = true;
			time_t t = time(0);
			struct tm tm = *localtime(&t);
			char buffer[150];
			char* version = update_get_current_version();
			sprintf(buffer, "\n\nDesktop Prayer Times App\nToday: %02d.%02d.%d, version: %s", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, version);

			int prev_error = errno;
			errno = 0;
			perror(buffer);
			errno = prev_error;
			free(version);
		}
	char buffer[150];
	sprintf(buffer, "%s %d: %s", file, line, msg);
	perror(buffer);
	errno = 0;
	}
}
