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
#define CMDTOK_ASSIGN_END ';'
#define CMDTOK_CMDSTART '{'
#define CMDTOK_CMDEND '}'
#define CMDTOK_VALUEIDENT '\"'
#define CMDTOK_COMMENTSTART '#'

#define BLOCK_VAR "VAR"

#define NAME_LEN 1024
#define MAX_VARS 1024

/// Defines the types for command blocks.
typedef enum
{
	NONE,
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

/// Structure stores the used variables in the commandfile.
typedef struct variables_t
{
	char* names[MAX_VARS];
	char* values[MAX_VARS];

	size_t lastIndex;
	
} VARIABLES;

/// Defines types of tokens in a cdav commandfile.
typedef enum
{
	CMD_DEFAULT,
	CMD_ASSIGN,
	CMD_ASSIGN_END,
	CMD_NAME,
	CMD_VALUEIDENT,
	CMD_START,
	CMD_END

} CMDFILE_TOKEN_TYPE;

/// Defines a processed token of a cdav commandfile.
typedef struct cmdfile_token_t
{
	CMDFILE_TOKEN_TYPE type;
	char* value;

} CMDFILE_TOKEN;

/// Global Variables structure.
VARIABLES variables;

/// Adds a new variable with given values.
void
var_add_variable(VARIABLES* vars, const char* name, const char* value);

/// Sets the value for the given argument.
void
cmd_set_arg(CMDBLOCK* block, char* arg, char* value);

/// Checks if the given file is a valid cdav commandfile.
int
isCdavFile(FILE* file);

/// Creates a new token with the given type and value as char* and adds it to the list.
CMDFILE_TOKEN
new_token_str(CMDFILE_TOKEN_TYPE type, const char* value);

/// Creates a new token with the given type and value as char and adds it to the list.
CMDFILE_TOKEN
new_token_char(CMDFILE_TOKEN_TYPE type, char value);

/// Performs lexical analysis of the commandfile.
CMDFILE_TOKEN*
lex_cmdfile(FILE* file, size_t* count);

/// Parses the given tokens to CMDBLOCK*.
CMDBLOCK*
parse_tokens(CMDFILE_TOKEN* tokens, size_t count);

/// Frees the memory used by the CMDFILE_TOKEN.
void
free_token(CMDFILE_TOKEN* token);

/// Executes the instructions in the given commandfile.
void
exec_cmdfile(const char* file);

#endif // CMDFILE_H
