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

#include "../include/parser.h"

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

	if (strcmp(operation, "GET") == 0)
		return GET;

	if (strcmp(operation, "HEAD") == 0)
		return HEAD;

	if (strcmp(operation, "PUT") == 0)
		return PUT;

	if (strcmp(operation, "PROPFIND") == 0)
		return PROPFIND;

	if (strcmp(operation, "PROPPATCH") == 0)
		return PROPPATCH;

	if (strcmp(operation, "MKCOL") == 0)
		return MKCOL;

	if (strcmp(operation, "DELETE") == 0)
		return DELETE;

	if (strcmp(operation, "COPY") == 0)
		return COPY;

	if (strcmp(operation, "MOVE") == 0)
		return MOVE;

	if (strcmp(operation, "LOCK") == 0)
		return LOCK;

	if (strcmp(operation, "UNLOCK") == 0)
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
