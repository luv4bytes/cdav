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
basic_params_check(CDAV_BASIC_PARAMS* params)
{
	if (params == NULL)
		error_exit(PROVIDE_PARAMS);

	if (params->url == NULL)
		error_exit(PROVIDE_URL);
}

void
cdav_handle_rescode(CURL* curl)
{
	long res_code = 0;
	curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &res_code);

	char msg[] = "Response: %ld\n";

	if (res_code >= 400)
	{
		fprintf(stderr, msg, res_code);
		error_exit(NULL);
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

	CDAV_RECV_BUFFER_PARAMS* p = (CDAV_RECV_BUFFER_PARAMS*)params;

	cdav_handle_rescode(p->curl);

	int datalen = size * nmemb;

	size_t oldlen = p->buffer_sz;
	p->buffer_sz = p->buffer_sz + datalen;

	if (p->buffer == NULL)
	{
		p->buffer = (char*) realloc(NULL, p->buffer_sz);
		memset(p->buffer, '\0', p->buffer_sz);
	}
	else
	{
		p->buffer = (char*) realloc(p->buffer, p->buffer_sz);
	}

	size_t j = 0;
	for(size_t i = oldlen; i < p->buffer_sz; i++)
	{
		p->buffer[i] = data[j++];
	}

	return datalen;
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

	if (w_params->file == NULL)
	{
		w_params->file = fopen(save_as, mode);

		if (w_params->file == NULL)
		{
			int err = errno;
			const char* errstr = strerror(err);

			fprintf(stderr, "Error writing data: %d - %s\n", err, errstr);
			error_exit(NULL);
		}
	}

	int written = fwrite(data, size, nmemb, w_params->file);

	if (written <= 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		fclose(w_params->file);
		w_params->file = NULL;

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

	if (r_params->file == NULL)
	{
		r_params->file = fopen(file_path, mode);

		if (r_params->file == NULL)
		{
			int err = errno;
			const char* errstr = strerror(err);

			fprintf(stderr, "Error writing data: %d - %s\n", err, errstr);
			error_exit(NULL);
		}
	}

	int bytes_read = fread(buffer, size, (size * nitems), r_params->file);

	if (bytes_read <= 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		fclose(r_params->file);
		r_params->file = NULL;

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
cdav_get(CDAV_BASIC_PARAMS* basic_params,
	 const char* save_as)
{
	basic_params_check(basic_params);

	if (save_as == NULL)
		error_exit(PROVIDE_PATH);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

	CDAV_WRITE_FILE_PARAMS params;
	params.file = NULL;
	params.save_as = save_as;
	params.curl = curl;

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_write_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("GET - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.file != NULL)
		fclose(params.file);

	curl_easy_cleanup(curl);
}

void
cdav_head(CDAV_BASIC_PARAMS* basic_params)
{
	basic_params_check(basic_params);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "HEAD";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*) &params);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("HEAD - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.buffer != NULL)
		printf("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	curl_easy_cleanup(curl);
}

void
cdav_put(CDAV_BASIC_PARAMS* basic_params,
	 const char* file_path)
{
	basic_params_check(basic_params);

	if (file_path == NULL)
		error_exit(PROVIDE_FILE);

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
	params.file = NULL;
	params.file_path = file_path;
	params.file_sz = file_sz;
	params.curl = curl;

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, &cdav_read_file);
	curl_easy_setopt(curl, CURLOPT_READDATA, (void*) &params);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) curl);
	curl_easy_setopt(curl, CURLOPT_PUT, 1);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);
	curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)params.file_sz);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("PUT - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.file != NULL)
		fclose(params.file);

	curl_easy_cleanup(curl);
}

void
cdav_propfind(CDAV_BASIC_PARAMS* basic_params,
	      CDAV_PROP** props,
	      size_t count,
	      char* depth)
{
	basic_params_check(basic_params);

	if (props == NULL)
		error_exit(PROVIDE_PROPS);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "PROPFIND";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	struct curl_slist* headers = NULL;
	struct curl_slist* headers_check = NULL;

	char* d = NULL;

	if (depth != NULL)
	{
		int len_depth = strlen("Depth: ");
		int len_arg_d = strlen(depth);
		int len = len_depth + len_arg_d;
		d = (char*)malloc(sizeof(char) * len);
		memset(d, '\0', len);
		sprintf(d, "Depth: %s", depth);

		headers_check = curl_slist_append(headers, d);

		if (headers_check == NULL)
			error_exit("Error creating Depth header! - Exiting.");

		headers = headers_check;
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	char* request = cdav_req_propfind(props, count);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("PROPFIND - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	printf("%s\n", params.buffer);

	free(request);

	if (params.buffer != NULL)
		free(params.buffer);

	if (headers != NULL)
		curl_slist_free_all(headers);

	if (d != NULL)
		free(d);

	curl_easy_cleanup(curl);
}

void
cdav_proppatch(CDAV_BASIC_PARAMS* basic_params,
	       CDAV_PROP** set_props,
	       size_t set_count,
	       CDAV_PROP** rm_props,
	       size_t rm_count)
{
	basic_params_check(basic_params);

	if (set_props == NULL && set_count > 0)
		error_exit(INVALID_PROPERTIES);

	if (rm_props == NULL && rm_count > 0)
		error_exit(INVALID_PROPERTIES);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "PROPPATCH";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	char* request = cdav_req_proppatch(set_props, set_count, rm_props, rm_count);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("PROPPATCH - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	printf("%s\n", params.buffer);

	free(request);

	if (params.buffer != NULL)
		free(params.buffer);

	curl_easy_cleanup(curl);
}

void
cdav_mkcol(CDAV_BASIC_PARAMS* basic_params)
{
	basic_params_check(basic_params);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "MKCOL";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("MKCOL - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.buffer != NULL)
		printf("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	curl_easy_cleanup(curl);
}

void
cdav_delete(CDAV_BASIC_PARAMS* basic_params)
{
	basic_params_check(basic_params);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "DELETE";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("DELETE - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.buffer != NULL)
		printf("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	curl_easy_cleanup(curl);
}

void
cdav_copy(CDAV_BASIC_PARAMS* basic_params,
	  const char* destination,
	  int overwrite)
{
	basic_params_check(basic_params);

	if (destination == NULL)
		error_exit(PROVIDE_DESTINATION);

	if (overwrite < 0 || overwrite > 1)
		error_exit(INVALID_OVERWRITE);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "COPY";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	struct curl_slist* headers = NULL;

	size_t len = strlen(destination) + strlen("Destination: ");
	char des[len];
	sprintf(des, "Destination: %s", destination);

	headers = curl_slist_append(headers, des);

	if (headers == NULL)
		error_exit("Error creating Destination header! - Exiting.");

	if (overwrite == 0)
	{
		char ovwr[] = "Overwrite: F";

		headers = curl_slist_append(headers, ovwr);

		if (headers == NULL)
			error_exit("Error creating Destination header! - Exiting.");
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("COPY - %s\n", basic_params->url);
	printf("%s\n", des);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.buffer != NULL)
		printf("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	if (headers != NULL)
		curl_slist_free_all(headers);

	curl_easy_cleanup(curl);
}

void
cdav_move(CDAV_BASIC_PARAMS* basic_params,
	  const char* destination,
	  int overwrite)
{
	basic_params_check(basic_params);

	if (destination == NULL)
		error_exit(PROVIDE_DESTINATION);

	if (overwrite < 0 || overwrite > 1)
		error_exit(INVALID_OVERWRITE);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "MOVE";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	struct curl_slist* headers = NULL;

	size_t len = strlen(destination) + strlen("Destination: ");
	char des[len];
	sprintf(des, "Destination: %s", destination);

	headers = curl_slist_append(headers, des);

	if (headers == NULL)
		error_exit("Error creating Destination header! - Exiting.");

	if (overwrite == 0)
	{
		char ovwr[] = "Overwrite: F";

		headers = curl_slist_append(headers, ovwr);

		if (headers == NULL)
			error_exit("Error creating Destination header! - Exiting.");
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("MOVE - %s\n", basic_params->url);
	printf("%s\n", des);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.buffer != NULL)
		printf("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	if (headers != NULL)
		curl_slist_free_all(headers);

	curl_easy_cleanup(curl);
}

void
cdav_check_lockscope(const char* scope)
{
	if (scope == NULL)
		error_exit(PROVIDE_LOCKSCOPE);

	if ( (strcmp(scope, "exclusive") != 0) && (strcmp(scope, "shared") != 0) )
		error_exit(PROVIDE_LOCKSCOPE);
}

void
cdav_lock(CDAV_BASIC_PARAMS* basic_params,
	  const char* scope,
	  const char* owner,
	  const char* depth)
{
	basic_params_check(basic_params);

	cdav_check_lockscope(scope);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "LOCK";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	struct curl_slist* headers = NULL;

	if (depth == NULL)
	{
		depth = "Depth: infity";

		headers = curl_slist_append(headers, depth);

		if (headers == NULL)
			error_exit("Error creating Destination header! - Exiting.");
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	char* request = cdav_req_lock(scope, owner);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("LOCK - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.buffer != NULL)
		printf("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	if (headers != NULL)
		curl_slist_free_all(headers);

	curl_easy_cleanup(curl);

}
void
cdav_unlock(CDAV_BASIC_PARAMS* basic_params, const char* lock_token)
{
	basic_params_check(basic_params);

	if (lock_token == NULL)
		error_exit(PROVIDE_LOCKTOKEN);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		error_exit(INIT_ERROR);

#ifdef TEST
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, basic_params->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, LIBCURL_AGENT);

	char p[] = "UNLOCK";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, p);

	struct curl_slist* headers = NULL;

	size_t len = strlen(lock_token) + strlen("Lock-Token: ");
	char tok[len];
	sprintf(tok, "Lock-Token: %s", lock_token);

	headers = curl_slist_append(headers, tok);

	if (headers == NULL)
		error_exit("Error creating Lock-Token header! - Exiting.");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	printf("UNLOCK - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		fprintf(stderr, "CURL ERR: %d - %s\n", result, err);

		error_exit("CURL ERR: Exiting");
	}

	if (params.buffer != NULL)
		printf("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	if (headers != NULL)
		curl_slist_free_all(headers);

	curl_easy_cleanup(curl);
}
