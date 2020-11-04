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

#ifndef PARSER_H
#define PARSER_H

#include "cdav.h"

/// Defines operations.
typedef enum
{
	GET,
	PUT,
	PROPFIND,
	PROPPATCH,
	MKCOL,
	DELETE,
	COPY,
	MOVE,
	UNKNOWN
} OPS;

/// Evaluates the given operation string.
OPS
eval_op(const char* op);

/// Defines a structure to hold argument values.
typedef struct args_struct_t
{
	char* file;		// -f --file
	char* operation;	// -o --operation
	char* address;		// -a --address
	char* user;		// -u --user
	char* passwd;		// -pw --password
	char* props;		// -p --props
	char* set_props;	// -sp --set-props
	char* rm_props;		// -rp --rm-props
	char* destination;	// -da --destination-address
	int overwrite;		// --no-overwrite
	char* upload_file;	// -uf --upload-file
	char* save_as;		// -s --save-as
	char* depth;		// -d --depth
	int help;		// -h --help
	int version;		// -v --version

} ARGS;

/// Defines a command.
typedef struct cmd_t
{
	ARGS args;
	OPS operation;
} CMD;

/// Initializes ARGS structure fields.
void
init_args(ARGS* args);

#endif // PARSER_H
