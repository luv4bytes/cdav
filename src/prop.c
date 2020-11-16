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

	// TODO: PARSE

	return NULL;
}

CDAV_PROP**
cdav_parse_props(char* prop_string, int* count)
{
	if (prop_string == NULL)
		return NULL;

	CDAV_PROP** list = NULL;

	const char* nested = "\\w*=\\{.*\\}";
	const char* val = "\\w*=\\w*";
	const char* single = "\\w*";

	regex_t reg_nested;
	regex_t reg_val;
	regex_t reg_single;

	int comp_nested = regcomp(&reg_nested, nested, REG_EXTENDED);

	if (comp_nested != 0)
		error_exit("Could not compile nested regex!");

	int comp_val = regcomp(&reg_val, val, REG_EXTENDED);

	if (comp_val != 0)
		error_exit("Could not compile value regex!");

	int comp_single = regcomp(&reg_single, single, REG_EXTENDED);

	if (comp_single != 0)
		error_exit("Could not compile single property regex!");

	regmatch_t nested_matches[MAX_MATCHES];

	int exec = regexec(&reg_nested, prop_string, MAX_MATCHES, nested_matches, 0);

	if (exec != 0 && exec != 1)
		error_exit("Error executing regex!");

	if (exec == 0)
	{
		for(int i = 0; i < MAX_MATCHES; i++)
		{
			size_t from = nested_matches[i].rm_so;
			size_t to = nested_matches[i].rm_eo;

			if (from == -1 && to == -1)
				break;

			size_t sz = (to - from) + 1;

			char buffer[sz];
			memset(buffer, 0, sz);

			str_cpy_from_to(buffer, prop_string, from, to);
			str_set_from_to(prop_string, ' ', from, to);

			CDAV_PROP* p = cdav_prop_from_str(buffer);

			if (p != NULL)
				list = (CDAV_PROP**)realloc(list, sizeof(CDAV_PROP) * *count++);
		}
	}

	regmatch_t val_matches[MAX_MATCHES];

	exec = regexec(&reg_nested, prop_string, MAX_MATCHES, val_matches, 0);

	if (exec != 0 && exec != 1)
		error_exit("Error executing regex!");

	if (exec == 0)
	{
		// TODO: Parse Match
	}

	regmatch_t single_matches[MAX_MATCHES];

	exec = regexec(&reg_nested, prop_string, MAX_MATCHES, single_matches, 0);

	if (exec != 0 && exec != 1)
		error_exit("Error executing regex!");

	if (exec == 0)
	{
		// TODO: Parse Match
	}

	// TODO: Regex

	return NULL;
}
