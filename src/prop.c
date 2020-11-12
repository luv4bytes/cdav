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

#include "../include/prop.h"

CDAV_PROP*
cdav_new_prop()
{
	CDAV_PROP* prop = (CDAV_PROP*) malloc(sizeof(CDAV_PROP));

	prop->name = NULL;
	prop->value = NULL;
	prop->children_size = 0;
	prop->children = NULL;

	return prop;
}

void
cdav_free_prop(CDAV_PROP* prop)
{
	if (prop == NULL)
		return;

	for(int i = 0; i < prop->children_size; i++)
	{
		cdav_free_prop(prop->children[i]);
	}

	if (prop->name != NULL)
		free((char*)prop->name);

	if (prop->value != NULL)
		free((char*)prop->value);

	free(prop);
}

void
cdav_prop_add_child(CDAV_PROP* parent, CDAV_PROP* child)
{
	if (parent == NULL)
		return;

	if (child == NULL)
		return;

	parent->children = (CDAV_PROP**) realloc(parent->children, sizeof(CDAV_PROP) * ++parent->children_size);

	parent->children[parent->children_size - 1] = child;
}

CDAV_PROP*
cdav_prop_from_str(char* str)
{
	if (str == NULL)
		return NULL;

	CDAV_PROP* prop = NULL;

	char* pstr = NULL;
	char* ostr = str;

	if (strstr(str, "=") != NULL)
	{
		while( (pstr = strtok(str, "=")) != NULL)
		{
			str = NULL;

			// TODO: Parse prop
		}
	}

	prop = cdav_new_prop();

	prop->name = ostr;

	return prop;
}

CDAV_PROP**
cdav_parse_props(char* prop_string, int* count)
{
	if (prop_string == NULL)
		return NULL;

	CDAV_PROP** props = NULL;

	char* pstr = NULL;

	while( (pstr = strtok(prop_string, ",")) != NULL)
	{
		prop_string = NULL;

		CDAV_PROP* prop = cdav_prop_from_str(pstr);

		if (prop == NULL)
			break;

		props = (CDAV_PROP**)realloc(props, sizeof(CDAV_PROP) * (++*count));

		props[*count - 1] = prop;
	}

	return props;
}
