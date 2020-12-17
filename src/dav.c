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
print_redirect_info(CDAV_BASIC_PARAMS* params, CURL* curl)
{
	if (params->follow_redirect == 0)
	{
		char* loc = NULL;

		curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &loc);

		if (loc != NULL)
			OUT_INFO("Redirect not allowed. Redirect location would be %s\n", loc);
	}
}

void
basic_params_check(CDAV_BASIC_PARAMS* params)
{
	if (params == NULL)
		ERROR_EXIT("%s\n", PROVIDE_PARAMS);

	if (params->url == NULL)
		ERROR_EXIT("%s\n", PROVIDE_URL);
}

void
cdav_handle_rescode(CURL* curl)
{
	long res_code = 0;
	curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &res_code);

	char msg[] = "Response: %ld\n";

	if (res_code >= 400)
		ERROR_EXIT(msg, res_code);
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

			ERROR_EXIT("Error writing data: %d - %s\n", err, errstr);
		}
	}

	int written = fwrite(data, size, nmemb, w_params->file);

	if (written <= 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		fclose(w_params->file);
		w_params->file = NULL;

		ERROR_EXIT("Error writing data: %d - %s\n", err, errstr);
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

			ERROR_EXIT("Error reading data: %d - %s\n", err, errstr);
		}
	}

	int bytes_read = fread(buffer, size, (size * nitems), r_params->file);

	if (bytes_read <= 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		fclose(r_params->file);
		r_params->file = NULL;

		ERROR_EXIT("Error reading data: %d - %s\n", err, errstr);
	}

	return bytes_read;
}

void
cdav_set_user_pw(CURL* curl, const char* user, const char* passwd)
{
	if (user != NULL && passwd != NULL)
	{
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC | CURLAUTH_DIGEST);

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
		ERROR_EXIT("%s\n", PROVIDE_SAVEPATH);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("GET - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

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
		ERROR_EXIT("%s\n", INIT_ERROR);

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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("HEAD - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		OUT_INFO("%s\n", params.buffer);

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
		ERROR_EXIT("%s\n", PROVIDE_FILE);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

	long file_sz = file_size(file_path);

	if (file_sz == -1)
	{
		int err = errno;
		const char* errstr = strerror(err);
		ERROR_EXIT("Error reading file size: %d - %s\n", err, errstr);
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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("PUT - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

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
		ERROR_EXIT("%s\n", PROVIDE_PROPS);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

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
			ERROR_EXIT("%s\n", "Error creating Depth header! - Exiting.");

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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("PROPFIND - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	OUT_INFO("%s\n", params.buffer);

	free(request);

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		free(params.buffer);

	if (headers != NULL)
		curl_slist_free_all(headers);

	if (d != NULL)
		free(d);

	for(size_t i = 0; i < count; i++)
		cdav_free_prop(props[i]);

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
		ERROR_EXIT("%s\n", INVALID_PROPERTIES);

	if (rm_props == NULL && rm_count > 0)
		ERROR_EXIT("%s\n", INVALID_PROPERTIES);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("PROPPATCH - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	OUT_INFO("%s\n", params.buffer);

	free(request);

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		free(params.buffer);

	for(size_t i = 0; i < set_count; i++)
	{
		cdav_free_prop(set_props[i]);
	}

	for(size_t i = 0; i < rm_count; i++)
	{
		cdav_free_prop(rm_props[i]);
	}

	curl_easy_cleanup(curl);
}

void
cdav_mkcol(CDAV_BASIC_PARAMS* basic_params)
{
	basic_params_check(basic_params);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("MKCOL - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		OUT_INFO("%s\n", params.buffer);

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
		ERROR_EXIT("%s\n", INIT_ERROR);

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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("DELETE - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		OUT_INFO("%s\n", params.buffer);

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
		ERROR_EXIT("%s\n", PROVIDE_DESTINATION);

	if (overwrite < 0 || overwrite > 1)
		ERROR_EXIT("%s\n", INVALID_OVERWRITE);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

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
		ERROR_EXIT("%s\n", "Error creating Destination header! - Exiting.");

	if (overwrite == 0)
	{
		char ovwr[] = "Overwrite: F";

		headers = curl_slist_append(headers, ovwr);

		if (headers == NULL)
			ERROR_EXIT("%s\n", "Error creating Destination header! - Exiting.");
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("COPY - %s\n", basic_params->url);
	printf("%s\n", des);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		OUT_INFO("%s\n", params.buffer);

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
		ERROR_EXIT("%s\n", PROVIDE_DESTINATION);

	if (overwrite < 0 || overwrite > 1)
		ERROR_EXIT("%s\n", INVALID_OVERWRITE);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

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
		ERROR_EXIT("Error creating Destination header! - Exiting.");

	if (overwrite == 0)
	{
		char ovwr[] = "Overwrite: F";

		headers = curl_slist_append(headers, ovwr);

		if (headers == NULL)
			ERROR_EXIT("%s\n", "Error creating Destination header! - Exiting.");
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("MOVE - %s\n", basic_params->url);
	printf("%s\n", des);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		OUT_INFO("%s\n", params.buffer);

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
		ERROR_EXIT("%s\n", PROVIDE_LOCKSCOPE);

	if ( (strcmp(scope, "exclusive") != 0) && (strcmp(scope, "shared") != 0) )
		ERROR_EXIT("%s\n", PROVIDE_LOCKSCOPE);
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
		ERROR_EXIT("%s\n", INIT_ERROR);

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
			ERROR_EXIT("%s\n", "Error creating Destination header! - Exiting.");
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

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("LOCK - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		OUT_INFO("%s\n", params.buffer);

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
		ERROR_EXIT("%s\n", PROVIDE_LOCKTOKEN);

	CURL* curl = curl_easy_init();

	if (curl == NULL)
		ERROR_EXIT("%s\n", INIT_ERROR);

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
		ERROR_EXIT("%s\n", "Error creating Lock-Token header! - Exiting.");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = curl;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cdav_receive_into_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &params);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, basic_params->follow_redirect);
	curl_easy_setopt(curl, CURLOPT_PROXY, basic_params->proxy);

	cdav_set_user_pw(curl, basic_params->user, basic_params->passwd);

	OUT_INFO("UNLOCK - %s\n", basic_params->url);

	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const char* err = curl_easy_strerror(result);
		ERROR_EXIT("CURL ERR: %d - %s\n", result, err);
	}

	print_redirect_info(basic_params, curl);

	if (params.buffer != NULL)
		OUT_INFO("%s\n", params.buffer);

	if (params.buffer != NULL)
		free(params.buffer);

	if (headers != NULL)
		curl_slist_free_all(headers);

	curl_easy_cleanup(curl);
}
