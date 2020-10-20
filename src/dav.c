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
cdav_write_file(char* data, size_t size, size_t nmemb, void* params)
{
	WRITE_PARAMS* w_params = (WRITE_PARAMS*) params;
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

void
cdav_get(const char* url,
	 const char* save_as,
	 const char* user,
	 const char* passwd)
{
	if (url == NULL)
		error_exit("Please specify a url!");

	if (save_as == NULL)
		error_exit("Please specify a path!");

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit("Error initializing curl!");

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_write_file);

	WRITE_PARAMS params;
	params.save_as = save_as;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	if (user != NULL && passwd != NULL)
	{
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

		curl_easy_setopt(curl, CURLOPT_USERNAME, user);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, passwd);
	}

	printf("GET - %s\n", url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting\n");
	}

	fclose(file);
	file = NULL;

	printf("Done\n");

	curl_easy_cleanup(curl);
}
