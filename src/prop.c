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

/* SCHEME

	Following scheme is used to determine property hierarchies for WebDAV commands.

	======

	PROP = {CHILD = VALUE, CHILD = {SECONDCHILD}}, PROP = VALUE

	======

	Basically translates to following hierarchy in XML:

		<PROP>
			<CHILD>VALUE</CHILD>
			<CHILD>
				<SECONDCHILD/>
			</CHILD>
		</PROP>
		<PROP>VALUE</PROP>

*/

CDAV_PROP*
parse_token(CDAV_PROP* parent, TOKEN* tokens, TOKEN tok, size_t* new_i)
{
	switch (tok.type)
	{
		case DEFAULT:
			return NULL;

		case SYMBOL:
		{
			CDAV_PROP* prop = NULL;

			if (tokens[*new_i + 1].type == ASSIGN)
			{
				*new_i = *new_i + 2;

				prop = cdav_new_prop();

				prop->name = tok.value;

				CDAV_PROP* child = parse_token(prop, tokens, tokens[*new_i], new_i);

				if (child != NULL)
					cdav_prop_add_child(prop, child);

				return prop;
			}

			if (tokens[*new_i - 1].type == ASSIGN && parent != NULL)
				parent->value = tok.value;

			*new_i = *new_i + 1;

			return prop;
		}

		case ASSIGN:
			*new_i = *new_i + 1;
			break;

		case CHILD_START:
			*new_i = *new_i + 1;
			break;

		case CHILD_END:
			*new_i = *new_i + 1;
			break;

		case DELIM:
			*new_i = *new_i + 1;
			break;
	}

	return NULL;
}

CDAV_PROP**
cdav_parse_props(char* prop_string, int* count)
{
	if (prop_string == NULL)
		return NULL;

	TOKEN tokens[MAX_TOKENS];
	memset(tokens, 0, MAX_TOKENS * sizeof(TOKEN*));
	size_t tokencount = 0;

	char propname[MAX_NAMELEN];
	memset(propname, 0, MAX_NAMELEN);

	size_t name_ind = 0;

	size_t len = strlen(prop_string);
	for(size_t i = 0; i < len; i++)
	{
		char c = prop_string[i];

		if (c == ' ' || c == '\n')
			continue;

		size_t namelen = strlen(propname);

		if (c == T_ASSIGN)
		{
			if (namelen != 0)
			{
				char* tmp = (char*)calloc(0, sizeof(char) * namelen);
				strcpy(tmp, propname);
				TOKEN name = {SYMBOL, tmp, tokencount};

				tokens[++tokencount - 1] = name;
			}

			memset(propname, 0, MAX_NAMELEN);

			name_ind = 0;

			char* tmp = (char*)calloc(0, sizeof(char) * 1);
			tmp[0] = c;
			TOKEN assign = {ASSIGN, tmp, tokencount};
			tokens[++tokencount - 1] = assign;

			continue;
		}

		if (c == T_CHILD_START)
		{
			if (namelen != 0)
			{
				char* tmp = (char*)calloc(0, sizeof(char) * namelen);
				strcpy(tmp, propname);
				TOKEN name = {SYMBOL, tmp, tokencount};

				tokens[++tokencount - 1] = name;
			}

			memset(propname, 0, MAX_NAMELEN);

			name_ind = 0;

			char* tmp = (char*)calloc(0, sizeof(char) * 1);
			tmp[0] = c;
			TOKEN start = {CHILD_START, tmp};
			tokens[++tokencount - 1] = start;

			continue;
		}

		if (c == T_CHILD_END)
		{
			if (namelen != 0)
			{
				char* tmp = (char*)calloc(0, sizeof(char) * namelen);
				strcpy(tmp, propname);
				TOKEN name = {SYMBOL, tmp, tokencount};

				tokens[++tokencount - 1] = name;
			}

			memset(propname, 0, MAX_NAMELEN);

			name_ind = 0;

			char* tmp = (char*)calloc(0, sizeof(char) * 1);
			tmp[0] = c;
			TOKEN end = {CHILD_END, tmp};
			tokens[++tokencount - 1] = end;

			continue;
		}

		if (c == T_DELIM)
		{
			if (namelen != 0)
			{
				char* tmp = (char*)calloc(0, sizeof(char) * namelen);
				strcpy(tmp, propname);
				TOKEN name = {SYMBOL, tmp, tokencount};

				tokens[++tokencount - 1] = name;
			}

			memset(propname, 0, MAX_NAMELEN);

			name_ind = 0;

			char* tmp = (char*)calloc(0, sizeof(char) * 1);
			tmp[0] = c;
			TOKEN delim = {DELIM, tmp};
			tokens[++tokencount - 1] = delim;

			continue;
		}

		propname[name_ind++] = c;
	}

	size_t namelen = strlen(propname);

	if (namelen > 0)
	{
		char* tmp = (char*)calloc(0, sizeof(char) * namelen);
		strcpy(tmp, propname);
		TOKEN name = {SYMBOL, tmp};

		tokens[++tokencount - 1] = name;
	}

#ifdef DEBUG
	for(size_t i = 0; i < tokencount; i++)
		printf("%s\n", tokens[i].value);
#endif

	if (tokencount == 0)
		return NULL;

	CDAV_PROP** props = NULL;

	size_t ind = 0;

	while(ind < tokencount)
	{
		CDAV_PROP* p = parse_token(NULL, tokens, tokens[ind], &ind);

		if (p == NULL)
			continue;

		props = (CDAV_PROP**)realloc(props, sizeof(CDAV_PROP*) * ++(*count));

		props[*count - 1] = p;
	}

	return NULL;

	// TODO: Free memory
}
