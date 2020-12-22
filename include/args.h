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

#ifndef ARGS_H
#define ARGS_H

#include "cdav.h"
#include "helper.h"

#define ARG_F_SHORT "-f"
#define ARG_F_LONG "--file"

#define ARG_O_SHORT "-o"
#define ARG_O_LONG "--operation"

#define ARG_A_SHORT "-a"
#define ARG_A_LONG "--address"

#define ARG_U_SHORT "-u"
#define ARG_U_LONG "--user"

#define ARG_PW_SHORT "-pw"
#define ARG_PW_LONG "--password"

#define ARG_P_SHORT "-p"
#define ARG_P_LONG "--props"

#define ARG_SP_SHORT "-sp"
#define ARG_SP_LONG "--set-props"

#define ARG_RP_SHORT "-rp"
#define ARG_RP_LONG "--rm-props"

#define ARG_DA_SHORT "-da"
#define ARG_DA_LONG "--destination-address"

#define ARG_NO_OW "--no-overwrite"

#define ARG_UF_SHORT "-uf"
#define ARG_UF_LONG "--upload-file"

#define ARG_S_SHORT "-s"
#define ARG_S_LONG "--save-as"

#define ARG_D_SHORT "-d"
#define ARG_D_LONG "--depth"

#define ARG_LS_SHORT "-ls"
#define ARG_LS_LONG "--lock-scope"

#define ARG_LT_SHORT "-lt"
#define ARG_LT_LONG "--lock-token"

#define ARG_LO_SHORT "-lo"
#define ARG_LO_LONG "--lock-owner"

#define ARG_H_SHORT "-h"
#define ARG_H_LONG "--help"

#define ARG_V_SHORT "-v"
#define ARG_V_LONG "--version"

#define ARG_NO_REDIRECT "--no-redirect"

#define ARG_PROXY "--proxy"

#define ARG_RAW "--raw"

/// Defines operations.
typedef enum
{
	GET,
	HEAD,
	PUT,
	PROPFIND,
	PROPPATCH,
	MKCOL,
	DELETE,
	COPY,
	MOVE,
	LOCK,
	UNLOCK,
	UNKNOWN
} OPS;

/// Evaluates the given operation string.
OPS
args_eval_op(const char* op);

/// Defines a structure to hold argument values.
typedef struct args_struct_st
{
	char* file;			// -f 	--file
	char* operation;	// -o 	--operation
	char* address;		// -a 	--address
	char* user;			// -u 	--user
	char* passwd;		// -pw 	--password
	char* props;		// -p 	--props
	char* set_props;	// -sp 	--set-props
	char* rm_props;		// -rp 	--rm-props
	char* destination;	// -da 	--destination-address
	int overwrite;		// --no-overwrite
	char* upload_file;	// -uf 	--upload-file
	char* save_as;		// -s 	--save-as
	char* depth;		// -d 	--depth
	int help;			// -h 	--help
	int version;		// -v 	--version
	char* lock_scope;	// -ls 	--lock-scope
	char* lock_owner;	// -lo	--lock-owner
	char* lock_token;	// -lt	--lock-token
	int follow_redirect;	// --no-redirect
	char* proxy;		// --proxy
	int raw;			// --raw

} ARGS;

/// Initializes ARGS structure fields.
void
args_init(ARGS* args);

/// Evaluates the given argument.
int
args_eval(const char* arg, const char* short_opt, const char* long_opt);

#endif // ARGS_H
