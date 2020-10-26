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

#include "../include/requests.h"

void
cdav_write_prop(CDAV_PROP* prop, xmlTextWriterPtr writer)
{
	if (prop == NULL)
		return;

	char nsd[] = "D";
	int res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)prop->name, NULL);

	if (res < 0)
		error_exit("Error writing element start! - Exiting");

	for(size_t i = 0; i < prop->children_size; i++)
	{
		cdav_write_prop(prop->children[i], writer);
	}

	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		error_exit("Error writing element end! - Exiting");
}

char*
cdav_req_propfind(CDAV_PROP** properties, size_t count)
{
	if (properties == NULL)
		return NULL;

	int res;
	xmlBufferPtr buffer = xmlBufferCreate();

	if (buffer == NULL)
		error_exit("Error creating xmlBufferPtr! - Exiting");

	xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

	if (writer == NULL)
		error_exit("Error creating xmlTextWriterPtr! - Exiting");

	res = xmlTextWriterSetIndent(writer, 1);

	if (res < 0)
		error_exit("Error setting indentation! - Exiting");

	// document start
	char enc[] = "UTF-8";
	res = xmlTextWriterStartDocument(writer, NULL, enc, NULL);

	if (res < 0)
		error_exit("Error writing document start! - Exiting");

	char nsd[] = "D";
	char name_propfind[] = "propfind";
	char nsd_uri[] = "DAV:";

	// <propfind>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_propfind, (const xmlChar*)nsd_uri);

	if (res < 0)
		error_exit("Error writing element start! - Exiting");

	// <prop>
	//char name_prop[] = "prop";
	//res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_prop, NULL);

	//if (res < 0)
	//	error_exit("Error writing element start! - Exiting");

	for(size_t i = 0; i < count; i++)
	{
		cdav_write_prop(properties[i], writer);
	}

	// </prop>
	//res = xmlTextWriterEndElement(writer);

	// </propfind>
	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		error_exit("Error writing element end! - Exiting");

	// document end
	res = xmlTextWriterEndDocument(writer);

#ifdef DEBUG
	printf("%s", buffer->content);
#endif

	if (res < 0)
		error_exit("Error writing document end! - Exiting");

	int req_len = strlen((char*)buffer->content);
	char* req = (char*)malloc(req_len);
	memset(req, '\0', req_len);

	strcpy(req, (char*)buffer->content);

	// TODO: Free writer

	return req;
}
