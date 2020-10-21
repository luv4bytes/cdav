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
#include "requests.h"
#include "parser.h"
#include <curl/curl.h>

/// CDAV File pointer.
FILE* file;

/// Returns the size of the given file.
long
file_size(const char* file_path);

/// Defines a structure that is passed into callbacks writing files.
typedef struct cdav_write_file_params_t
{
	const char* save_as;
	CURL* curl;

} CDAV_WRITE_FILE_PARAMS;

/// Defines a structure that is passed into callbacks reading data.
typedef struct cdav_read_file_params_t
{
	const char* file_path;
	long file_sz;
	CURL* curl;

} CDAV_READ_FILE_PARAMS;

/// Basic recieve callback.
size_t
cdav_recieve(char* data, size_t size, size_t nmemb, void* params);

/// Callback for curl writing to file.
size_t
cdav_write_file(char* data, size_t size, size_t nmemb, void* params);

/// Read file callback for curl.
size_t
cdav_read_file(char* buffer, size_t size, size_t nitems, void* params);

/// WebDAV GET - The target can be saved under the path given by "save_as" parameter.
void
cdav_get(const char* url,
	 const char* save_as,
	 const char* user,
	 const char* passwd);

/// WebDAV PUT - Puts the given file to the given url.
void
cdav_put(const char* file,
	  const char* url,
	  const char* user,
	  const char* passwd);

#endif // DAV_H
