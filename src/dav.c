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

#include "../include/dav.h"

#define PROVIDE_URL "Please provide a url!"
#define PROVIDE_PATH "Please provide a path!"
#define PROVIDE_FILE "Please provide a file!"
#define PROVIDE_PROPS "Please provide properties!"

#define INIT_ERROR "Error initializing libcurl!"

#define LIBCURL_AGENT "libcurl-agent/1.0"

long
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

void
cdav_handle_rescode(CURL* curl)
{
	long res_code = 0;
	curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &res_code);

	char msg[] = "Response: %ld - %s\n";

	switch (res_code)
	{
		case 401: // UNAUTHORIZED

			fprintf(stderr, msg, res_code, "Unauthorized");
			error_exit(NULL);

			break;

		case 404: // NOT FOUND

			fprintf(stderr, msg, res_code, "Not found");
			error_exit(NULL);

			break;

		default:
			break;
	}
}

size_t
cdav_receive(char* data, size_t size, size_t nmemb, void* params)
{
	CURL* curl = (CURL*) params;

	cdav_handle_rescode(curl);

	return nmemb * size;
}

size_t
cdav_receive_into_buffer(char* data, size_t size, size_t nmemb, void* params)
{
	if (params == NULL)
		return -1;

	// TODO: Rescode

	char* buffer = (char*) params;

	size_t max = size * nmemb;
	int bufferlen = strlen(buffer);

	if (bufferlen == max)
		buffer = (char*)realloc(buffer, bufferlen + max);

	strcat(buffer, data);

	return max;
}

size_t
cdav_write_file(char* data, size_t size, size_t nmemb, void* params)
{
	CDAV_WRITE_FILE_PARAMS* w_params = (CDAV_WRITE_FILE_PARAMS*) params;
	CURL* curl = w_params->curl;

	cdav_handle_rescode(curl);

	const char* save_as = w_params->save_as;
	size_t res_sz = size * nmemb;
	char mode[1] = "w";

	if (file == NULL)
	{
		file = fopen(save_as, mode);

		if (file == NULL)
		{
			int err = errno;
			const char* errstr = strerror(err);

			fprintf(stderr, "Error writing data: %d - %s\n", err, errstr);
			error_exit(NULL);
		}
	}

	int written = fwrite(data, size, nmemb, file);

	if (written <= 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		fclose(file);
		file = NULL;

		fprintf(stderr, "Error writing data: %d - %s\n", err, errstr);
		error_exit(NULL);
	}

	return res_sz;
}

size_t
cdav_read_file(char* buffer, size_t size, size_t nitems, void* params)
{
	CDAV_READ_FILE_PARAMS* r_params = (CDAV_READ_FILE_PARAMS*) params;
	CURL* curl = r_params->curl;
	const char* file_path = r_params->file_path;

	cdav_handle_rescode(curl);

	char mode[1] = "r";

	if (file == NULL)
	{
		file = fopen(file_path, mode);

		if (file == NULL)
		{
			int err = errno;
			const char* errstr = strerror(err);

			fprintf(stderr, "Error writing data: %d - %s\n", err, errstr);
			error_exit(NULL);
		}
	}

	int bytes_read = fread(buffer, size, (size * nitems), file);

	if (bytes_read <= 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		fclose(file);
		file = NULL;

		fprintf(stderr, "Error reading data: %d - %s\n", err, errstr);
		error_exit(NULL);
	}

	return bytes_read;
}

void
cdav_set_user_pw(CURL* curl, const char* user, const char* passwd)
{
	if (user != NULL && passwd != NULL)
	{
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

		curl_easy_setopt(curl, CURLOPT_USERNAME, user);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, passwd);
	}
}

void
cdav_get(const char* url,
	 const char* save_as,
	 const char* user,
	 const char* passwd)
{
	if (url == NULL)
		error_exit(PROVIDE_URL);

	if (save_as == NULL)
		error_exit(PROVIDE_PATH);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

	CDAV_WRITE_FILE_PARAMS params;
	params.save_as = save_as;
	params.curl = curl;

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_write_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	cdav_set_user_pw(curl, user, passwd);

	printf("GET - %s\n", url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	fclose(file);
	file = NULL;

	printf("Done\n");

	curl_easy_cleanup(curl);
}

void
cdav_put(const char* file_path,
	  const char* url,
	  const char* user,
	  const char* passwd)
{
	if (file_path == NULL)
		error_exit(PROVIDE_FILE);

	if (url == NULL)
		error_exit(PROVIDE_URL);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

	long file_sz = file_size(file_path);

	if (file_sz == -1)
	{
		int err = errno;
		const char* errstr = strerror(err);

		fprintf(stderr, "Error reading file size: %d - %s\n", err, errstr);
		error_exit(NULL);
	}

	CDAV_READ_FILE_PARAMS params;
	params.file_path = file_path;
	params.file_sz = file_sz;
	params.curl = curl;

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, &cdav_read_file);
	curl_easy_setopt(curl, CURLOPT_READDATA, (void*) &params);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) curl);
	curl_easy_setopt(curl, CURLOPT_PUT, 1);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);
	curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)params.file_sz);

	cdav_set_user_pw(curl, user, passwd);

	printf("PUT - %s\n", url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	fclose(file);
	file = NULL;

	printf("Done\n");

	curl_easy_cleanup(curl);
}

void
cdav_propfind(const char* url, CDAV_PROP** props, size_t count, const char* user, const char* passwd)
{
	if (url == NULL)
		error_exit(PROVIDE_URL);

	if (props == NULL)
		error_exit(PROVIDE_PROPS);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "PROPFIND";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	int buflen = 1024;

	char* buffer = malloc(sizeof(char) * buflen); // Buffer to put response in to
	memset(buffer, '\0', buflen);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) buffer);

	char* request = cdav_req_propfind(props, count);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);

	cdav_set_user_pw(curl, user, passwd);

	printf("PROPFIND - %s\n", url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

#ifdef TEST
	printf("%s\n", buffer);
#endif

	// TODO: Format response and print

	free(request);
	free(buffer);

	printf("Done\n");

	curl_easy_cleanup(curl);
}
