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

#include "../include/helper.h"

size_t
file_size(const char* file_path)
{
	if (file_path == NULL)
		return -1;

	FILE* f = fopen(file_path, "r");

	if (f == NULL)
		return -1;

	if (fseek(f, 0, SEEK_END) == -1)
	{
		fclose(f);
		return -1;
	}

	long sz = ftell(f);

	if (sz == -1)
	{
		fclose(f);
		return -1;
	}

	fclose(f);

	return sz;
}

size_t
digits(size_t num)
{
	size_t count = 0;

	while(num != 0)
	{
		num /= 10;
		count++;
	}

	return count;
}

void
str_cpy_from_to(char* dest, char* src, size_t from, size_t to)
{
	if (dest == NULL)
		return;

	if (src == NULL)
		return;

	size_t j = 0;

	for(size_t i = from; i < to; i++)
	{
		dest[j++] = src[i];
	}
}

void
str_set_from_to(char* dest, char c, size_t from, size_t to)
{
	if (dest == NULL)
		return;

	for(size_t i = from; i < to; i++)
	{
		dest[i] = c;
	}
}

void
str_to_upper(const char* str, char* buffer, size_t len)
{
	if (str == NULL)
		return;

	for(size_t i = 0; i < len; i++)
	{
		buffer[i] = toupper(str[i]);
	}
}

int
strcmp_from_to(const char* first, size_t from_first, size_t to_first,
               const char* sec, size_t from_sec, size_t to_sec)
{
	if (first == NULL && sec == NULL)
		return 0;

	if ( (to_first - from_first) != (to_sec - from_sec) )
		return -1;

	for(size_t i = from_first; i < to_first; i++)
	{
		#ifdef DEBUG
			printf("%c\n", first[i]);
			printf("%c\n", sec[from_sec + i]);
		#endif

		if (first[i] != sec[from_sec + i])
			return -1;
	}

	return 0;
}
