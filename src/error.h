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

#ifndef SRC_MYPERROR_H_
#define SRC_MYPERROR_H_

/** \brief Print into error stream
 *
 * For the first run of this function an specific error msg is printed containing debug information.
 *
 * \param msg Error msg
 *
 */
void myperror(char const*const file, const int line, char const*const msg);

#endif /* SRC_MYPERROR_H_ */
