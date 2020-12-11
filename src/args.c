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

#include "../include/args.h"

void
init_args(ARGS* args)
{
	if (args == NULL)
		return;

	args->file = NULL;
	args->operation = NULL;
	args->address = NULL;
	args->user = NULL;
	args->passwd = NULL;
	args->props = NULL;
	args->set_props = NULL;
	args->rm_props = NULL;
	args->destination = NULL;
	args->overwrite = 1;
	args->upload_file = NULL;
	args->save_as = NULL;
	args->depth = NULL;
	args->lock_scope = NULL;
	args->lock_owner = NULL;
	args->lock_token = NULL;
	args->help = 0;
	args->version = 0;
	args->follow_redirect = 1;
	args->proxy = NULL;
}

OPS
eval_op(const char* operation)
{
	if (operation == NULL)
		return UNKNOWN;

	size_t len = strlen(operation) + 1;
	char buffer[len];
	memset(buffer, 0, len);

	str_to_upper(operation, buffer, len);

	if (strcmp(buffer, "GET") == 0)
		return GET;

	if (strcmp(buffer, "HEAD") == 0)
		return HEAD;

	if (strcmp(buffer, "PUT") == 0)
		return PUT;

	if (strcmp(buffer, "PROPFIND") == 0)
		return PROPFIND;

	if (strcmp(buffer, "PROPPATCH") == 0)
		return PROPPATCH;

	if (strcmp(buffer, "MKCOL") == 0)
		return MKCOL;

	if (strcmp(buffer, "DELETE") == 0)
		return DELETE;

	if (strcmp(buffer, "COPY") == 0)
		return COPY;

	if (strcmp(buffer, "MOVE") == 0)
		return MOVE;

	if (strcmp(buffer, "LOCK") == 0)
		return LOCK;

	if (strcmp(buffer, "UNLOCK") == 0)
		return UNLOCK;

	return UNKNOWN;
}

int
eval_arg(const char* arg, const char* short_opt, const char* long_opt)
{
	if (short_opt != NULL)
	{
		if (strcmp(arg, short_opt) == 0)
			return 1;
	}

	if (long_opt != NULL)
	{
		if (strcmp(arg, long_opt) == 0)
			return 1;
	}

	return 0;
}
