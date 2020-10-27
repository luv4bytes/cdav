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

	int res = 0;

	if (prop->value != NULL)
	{
		res = xmlTextWriterWriteElementNS(writer, (const xmlChar*) nsd, (const xmlChar*)prop->name, NULL, (const xmlChar*) prop->value);

		if (res < 0)
			error_exit("Error writing element! - Exiting");

		return;
	}
	else
	{
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)prop->name, NULL);

		if (res < 0)
			error_exit("Error writing element start! - Exiting");
	}

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

	// Named properties need the <prop> element

	for(size_t i = 0; i < count; i++)
	{
		cdav_write_prop(properties[i], writer);
	}

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

	memcpy(req, (char*)buffer->content, req_len);

	xmlFreeTextWriter(writer);

	return req;
}

char*
cdav_req_proppatch(CDAV_PROP** set_props,
		   size_t set_count,
		   CDAV_PROP** rm_props,
		   size_t rm_count)
{
	if (set_props == NULL && rm_props == NULL)
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
	char name_propertyupdate[] = "propertyupdate";
	char nsd_uri[] = "DAV:";

	// <propertyupdate>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_propertyupdate, (const xmlChar*)nsd_uri);

	if (res < 0)
		error_exit("Error writing element start! - Exiting");

	if (set_count > 0)
	{
		char name_set[] = "set";

		// <set>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_set, NULL);

		if (res < 0)
			error_exit("Error writing element start! - Exiting");

		char name_prop[] = "prop";

		// <prop>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_prop, NULL);

		if (res < 0)
			error_exit("Error writing element start! - Exiting");

		for(size_t i = 0; i < set_count; i++)
		{
			cdav_write_prop(set_props[i], writer);
		}

		// </prop>
		res = xmlTextWriterEndElement(writer);

		// </set>
		res = xmlTextWriterEndElement(writer);

		if (res < 0)
			error_exit("Error writing element end! - Exiting");
	}

	if (rm_count > 0)
	{
		char name_rm[] = "remove";

		// <remove>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_rm, NULL);

		if (res < 0)
			error_exit("Error writing element start! - Exiting");

		char name_prop[] = "prop";

		// <prop>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_prop, NULL);

		if (res < 0)
			error_exit("Error writing element start! - Exiting");

		for(size_t i = 0; i < set_count; i++)
		{
			cdav_write_prop(rm_props[i], writer);
		}

		// </prop>
		res = xmlTextWriterEndElement(writer);

		// </remove>
		res = xmlTextWriterEndElement(writer);

		if (res < 0)
			error_exit("Error writing element end! - Exiting");

	}

	// </propertyupdate>
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

	memcpy(req, (char*)buffer->content, req_len);

	xmlFreeTextWriter(writer);

	return req;
}
