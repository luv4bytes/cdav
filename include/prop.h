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

// Defines a WebDAV property.
typedef struct cdav_prop_t
{
	const char* name;
	const char* value;

	struct cdav_prop_t** children;
	size_t children_size;

} CDAV_PROP;

/// Creates a new property.
CDAV_PROP*
cdav_new_prop();

/// Frees the memory used by the property.
void
cdav_free_prop(CDAV_PROP* prop);

/// Adds the child to the parents children list.
void
cdav_prop_add_child(CDAV_PROP* parent, CDAV_PROP* child);

#endif // PROP_H
