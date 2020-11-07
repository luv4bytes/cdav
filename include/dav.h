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
#include "helper.h"
#include <curl/curl.h>

/// CDAV File pointer.
FILE* file;

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

/// Defines a structure containing a char buffer and a curl instance.
typedef struct cdav_recv_buffer_params_t
{
	char* buffer;
	size_t buffer_sz;
	CURL* curl;

} CDAV_RECV_BUFFER_PARAMS;

/// Defines a structure for basic parameters such as URL, user and password.
typedef struct cdav_basic_params_t
{
	const char* url;
	const char* user;
	const char* passwd;

} CDAV_BASIC_PARAMS;

/// Checks if the CDAV_BASIC_PARAMS is ok.
void
basic_params_check(CDAV_BASIC_PARAMS* params);

/// Basic receive callback.
size_t
cdav_receive(char* data, size_t size, size_t nmemb, void* params);

/// Puts the received bytes into a buffer.
size_t
cdav_receive_into_buffer(char* data, size_t size, size_t nmemb, void* buffer);

/// Callback for curl writing to file.
size_t
cdav_write_file(char* data, size_t size, size_t nmemb, void* params);

/// Read file callback for curl.
size_t
cdav_read_file(char* buffer, size_t size, size_t nitems, void* params);

/// Sets the user and password for the current CURL instance.
void
cdav_set_user_pw(CURL* curl, const char* user, const char* passwd);

/// WebDAV GET - The target can be saved under the path given by "save_as" parameter.
void
cdav_get(CDAV_BASIC_PARAMS* params,
	 const char* save_as);

/// WebDAV PUT - Puts the given file to the given url.
void
cdav_put(CDAV_BASIC_PARAMS* params,
	 const char* file);

/// WebDAV PROPFIND - Gets the requested properties and prints them to stdout.
void
cdav_propfind(CDAV_BASIC_PARAMS* params,
	      CDAV_PROP** props,
	      size_t count,
	      char* depth);

/// WebDAV PROPPATCH - Sets and removes properties of resources.
void
cdav_proppatch(CDAV_BASIC_PARAMS* params,
	       CDAV_PROP** set_props,
	       size_t set_count,
	       CDAV_PROP** rm_props,
	       size_t rm_count);

/// WebDAV MKCOL - Creates a collection with specified name at specified url.
void
cdav_mkcol(CDAV_BASIC_PARAMS* params);

/// WebDAV DELETE - Deletes the resource at the url.
void
cdav_delete(CDAV_BASIC_PARAMS* params);

/// WebDAV COPY - Copies specified resource to destination.
void
cdav_copy(CDAV_BASIC_PARAMS* params, const char* destination, int overwrite);

/// WebDAV MOVE - Moves specified resource to destination.
void
cdav_move(CDAV_BASIC_PARAMS* params, const char* destination, int overwrite);

/// Checks if the given losck scope is valid.
void
cdav_check_lockscope(const char* scope);

/// WebDAV LOCK - Locks given resource with specified scope and type.
void
cdav_lock(CDAV_BASIC_PARAMS* params,
	  const char* scope,
	  const char* owner,
	  const char* depth);

/// WebDAV UNLOCK - Unlocks the resource with given lock token.
void
cdav_unlock(CDAV_BASIC_PARAMS* params, const char* lock_token);

#endif // DAV_H
