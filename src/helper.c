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
