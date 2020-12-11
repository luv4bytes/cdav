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

/// Defines types of tokens in a cdav commandfile.
typedef enum
{
	CMD_DEFAULT,
	CMD_KEYWORD,
	CMD_ASSIGN,
	CMD_NAME

} CMDFILE_TOKEN_TYPE;

/// Defines a processed token of a cdav commandfile.
typedef struct cmdfile_token_t
{
	CMDFILE_TOKEN_TYPE type;

} CMDFILE_TOKEN;

/// Executes the instructions in the given commandfile.
void
exec_cmdfile(const char* file);

#endif // CMDFILE_H
