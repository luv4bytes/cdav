/*
 * Copyright (C) 2020 Lukas Pfeifer
 * for cdav version 1.0
 *      https://github.com/luv4bytes/cdav
 *
 * This file is part of cdav.
 *
 * cdav is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cdav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with cdav. If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef HELPER_H
#define HELPER_H

#include "cdav.h"
#include <ctype.h>

#define str_is_equal(str1, str2) (strcmp(str1, str2) == 0)
#define str_null_empty(str) (str == NULL || str[0] == 0)

/// Returns the size of the given file.
size_t
file_size(const char* file_path);

/// Returns the number of digits in given number.
size_t
digits(size_t num);

/// Copies bytes from index "from" to "to" from "src" into "dest".
void
str_cpy_from_to(char* dest, char* src, size_t from, size_t to);

/// Sets byte "c" from index "from" to "to" in "dest".
void
str_set_from_to(char* dest, char c, size_t from, size_t to);

/// Turns the given string to upper case.
void
str_to_upper(const char* str, char* buffer, size_t len);

/// Compares the first string from 'from_first' to 'to_first' with the second string from 'from_sec' to 'to_sec'.
int
strcmp_from_to(const char* first, size_t from_first, size_t to_first,
               const char* sec, size_t from_sec, size_t to_sec);

#endif // HELPER_H
