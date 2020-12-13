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

#ifndef CMDFILE_H
#define CMDFILE_H

#include "cdav.h"
#include "err.h"
#include "helper.h"
#include "args.h"

#define EXEC_DIRECTIVE "!cdav!"

#define CMDTOK_ASSIGN '='
#define CMDTOK_CMDSTART '{'
#define CMDTOK_CMDEND '}'
#define CMDTOK_VALUEIDENT '\"'
#define CMDTOK_COMMENTSTART '#'

#define BLOCK_VAR "VAR"

#define NAME_LEN 1024

/// Defines the types for command blocks.
typedef enum
{
	COMMAND,	// Basic command.
	VAR			// Special block for variables.

} CMDBLOCK_TYPE;

/// Defines a command block holding basic arguments and special arguments for commandfile execution.
typedef struct cmdblock_t
{
	ARGS args;
	CMDBLOCK_TYPE type;

	const char* name;
	int parallelity_level;

} CMDBLOCK;

/// Defines types of tokens in a cdav commandfile.
typedef enum
{
	CMD_DEFAULT,
	CMD_KEYWORD,
	CMD_ASSIGN,
	CMD_NAME,
	CMD_VARIABLE,
	CMD_START,
	CMD_END

} CMDFILE_TOKEN_TYPE;

/// Defines a processed token of a cdav commandfile.
typedef struct cmdfile_token_t
{
	CMDFILE_TOKEN_TYPE type;
	char* value;

} CMDFILE_TOKEN;

/// Checks if the given file is a valid cdav commandfile.
int
isCdavFile(FILE* file);

/// Creates a new token with the given type and value and adds it to the list.
CMDFILE_TOKEN
new_token(CMDFILE_TOKEN_TYPE type, const char* value);

/// Parses the commandfile and extracts tokens.
CMDFILE_TOKEN*
parse_cmdfile(FILE* file);

/// Executes the instructions in the given commandfile.
void
exec_cmdfile(const char* file);

#endif // CMDFILE_H
