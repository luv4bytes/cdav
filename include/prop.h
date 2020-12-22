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

#ifndef PROP_H
#define PROP_H

#include "cdav.h"
#include "err.h"
#include "helper.h"

#define T_ASSIGN '='
#define T_CHILD_START '{'
#define T_CHILD_END '}'
#define T_DELIM ','

#define MAX_TOKENS 4096
#define MAX_NAMELEN 512

// Defines a WebDAV property.
typedef struct cdav_prop_st
{
	const char* name;
	const char* value;

	struct cdav_prop_st** children;
	size_t children_size;

} CDAV_PROP;

typedef enum
{
	DEFAULT,
	SYMBOL,
	ASSIGN,
	CHILD_START,
	CHILD_END,
	DELIM

} TYPE;

typedef struct token_st
{
	TYPE type;
	char* value;

} TOKEN;

/// Creates a new property.
CDAV_PROP*
cdav_new_prop();

/// Frees the memory used by the property.
void
cdav_free_prop(CDAV_PROP* prop);

/// Adds the child to the parents children list.
void
cdav_prop_add_child(CDAV_PROP* parent, CDAV_PROP* child);

/// Parses the given token to a CDAV_PROP*.
CDAV_PROP*
parse_token(CDAV_PROP* parent, TOKEN* tokens, TOKEN tok, size_t* new_i);

/// Parses the given prop string to CDAV_PROP**.
CDAV_PROP**
cdav_parse_props(char* props, int* count);

#endif // PROP_H
