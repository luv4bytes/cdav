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

#ifndef DAV_H
#define DAV_H

#include "cdav.h"
#include "err.h"
#include <curl/curl.h>

/// FILE pointer for writing to files.
FILE* file;

/// Defines a structure that is passed into write callbacks.
typedef struct cdav_write_params_t
{
	const char* save_as;
	CURL* curl;

} WRITE_PARAMS;

/// Write callback for curl.
size_t
cdav_write(char* data, size_t size, size_t nmemb, void* params);

/// WebDAV GET - The target can be saved under the path given by "save_as" parameter.
void
cdav_get(const char* url,
	 const char* save_as,
	 const char* user,
	 const char* passwd);

#endif // DAV_H
