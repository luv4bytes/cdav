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

#define ERROR_ELEMENT "Error writing element! - Exiting."
#define ERROR_ELEMENT_START "Error writing element start! - Exiting."
#define ERROR_ELEMENT_END "Error writing element end! - Exiting."
#define ERROR_DOC_START "Error writing document start! - Exiting."
#define ERROR_DOC_END "Error writing document end! - Exiting."

#define ERROR_XML_BUFFER "Error creating xmlBufferPtr! - Exiting."
#define ERROR_XML_WRITER "Error creating xmlWriterPtr! - Exiting."

void
cdav_write_prop(CDAV_PROP* prop, xmlTextWriterPtr writer, char* ns)
{
	if (prop == NULL)
		return;

	int res = 0;

	if (prop->value != NULL)
	{
		res = xmlTextWriterWriteElementNS(writer, (const xmlChar*)ns, (const xmlChar*)prop->name, NULL, (const xmlChar*) prop->value);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT)

		return;
	}
	else
	{
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)ns, (const xmlChar*)prop->name, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)
	}

	for(size_t i = 0; i < prop->children_size; i++)
	{
		cdav_write_prop(prop->children[i], writer, ns);
	}

	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)
}

char*
cdav_req_propfind(CDAV_PROP** properties, size_t count)
{
	if (properties == NULL)
		return NULL;

	int res;
	xmlBufferPtr buffer = xmlBufferCreate();

	if (buffer == NULL)
		ERROR_EXIT("%s\n", ERROR_XML_BUFFER)

	xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

	if (writer == NULL)
		ERROR_EXIT("%s\n", ERROR_XML_WRITER)

	res = xmlTextWriterSetIndent(writer, 1);

	if (res < 0)
		ERROR_EXIT("Error setting indentation! - Exiting");

	// document start
	char enc[] = "UTF-8";
	res = xmlTextWriterStartDocument(writer, NULL, enc, NULL);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_DOC_START)

	char nsd[] = "D";
	char name_propfind[] = "propfind";
	char nsd_uri[] = "DAV:";

	// <propfind>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_propfind, (const xmlChar*)nsd_uri);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

	if (count > 1 || (count == 1 && strcmp(properties[0]->name, "allprop") != 0))
	{
		char name_prop[] = "prop";

		// <prop>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_prop, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)
	}

	for(size_t i = 0; i < count; i++)
	{
		cdav_write_prop(properties[i], writer, nsd);
	}

	if (count > 1 || (count == 1 && strcmp(properties[0]->name, "allprop") != 0))
	{
		// </prop>
		res = xmlTextWriterEndElement(writer);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_END)
	}

	// </propfind>
	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	// document end
	res = xmlTextWriterEndDocument(writer);

#ifdef DEBUG
	printf("%s", buffer->content);
#endif

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

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
		ERROR_EXIT("%s\n", ERROR_XML_BUFFER)

	xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

	if (writer == NULL)
		ERROR_EXIT("%s\n", ERROR_XML_WRITER)

	res = xmlTextWriterSetIndent(writer, 1);

	if (res < 0)
		ERROR_EXIT("Error setting indentation! - Exiting")

	// document start
	char enc[] = "UTF-8";
	res = xmlTextWriterStartDocument(writer, NULL, enc, NULL);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_DOC_START)

	char nsd[] = "D";
	char name_propertyupdate[] = "propertyupdate";
	char nsd_uri[] = "DAV:";

	// <propertyupdate>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_propertyupdate, (const xmlChar*)nsd_uri);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

	if (set_count > 0)
	{
		char name_set[] = "set";

		// <set>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_set, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

		char name_prop[] = "prop";

		// <prop>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_prop, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

		for(size_t i = 0; i < set_count; i++)
		{
			cdav_write_prop(set_props[i], writer, NULL);
		}

		// </prop>
		res = xmlTextWriterEndElement(writer);

		// </set>
		res = xmlTextWriterEndElement(writer);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_END)
	}

	if (rm_count > 0)
	{
		char name_rm[] = "remove";

		// <remove>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_rm, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

		char name_prop[] = "prop";

		// <prop>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_prop, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

		for(size_t i = 0; i < set_count; i++)
		{
			cdav_write_prop(rm_props[i], writer, NULL);
		}

		// </prop>
		res = xmlTextWriterEndElement(writer);

		// </remove>
		res = xmlTextWriterEndElement(writer);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	}

	// </propertyupdate>
	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	// document end
	res = xmlTextWriterEndDocument(writer);

#ifdef DEBUG
	printf("%s", buffer->content);
#endif

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_DOC_END)

	int req_len = strlen((char*)buffer->content);
	char* req = (char*)malloc(req_len);
	memset(req, '\0', req_len);

	memcpy(req, (char*)buffer->content, req_len);

	xmlFreeTextWriter(writer);

	return req;
}

char*
cdav_req_lock(const char* scope,
	      const char* owner)
{
	if (scope == NULL)
		return NULL;

	int res;
	xmlBufferPtr buffer = xmlBufferCreate();

	if (buffer == NULL)
		ERROR_EXIT("%s\n", ERROR_XML_BUFFER)

	xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

	if (writer == NULL)
		ERROR_EXIT("%s\n", ERROR_XML_WRITER)

	res = xmlTextWriterSetIndent(writer, 1);

	if (res < 0)
		ERROR_EXIT("%s\n", "Error setting indentation! - Exiting");

	// document start
	char enc[] = "UTF-8";
	res = xmlTextWriterStartDocument(writer, NULL, enc, NULL);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_DOC_START)

	char nsd[] = "D";
	char name_lockinfo[] = "lockinfo";
	char nsd_uri[] = "DAV:";

	// <lockinfo>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_lockinfo, (const xmlChar*)nsd_uri);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

	char name_lockscope[] = "lockscope";

	// <lockscope>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_lockscope, NULL);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

	// <scope/>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)scope, NULL);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	// </lockscope>
	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	char name_locktype[] = "locktype";
	char locktype[] = "write";

	// <locktype>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_locktype, NULL);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

	// <type/>
	res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)locktype, NULL);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	// </locktype>
	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	if (owner != NULL)
	{
		char name_owner[] = "owner";

		// <owner>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)name_owner, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

		// <ownervalue/>
		res = xmlTextWriterStartElementNS(writer, (const xmlChar*)nsd, (const xmlChar*)owner, NULL);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_START)

		res = xmlTextWriterEndElement(writer);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

		// </owner>
		res = xmlTextWriterEndElement(writer);

		if (res < 0)
			ERROR_EXIT("%s\n", ERROR_ELEMENT_END)
	}

	// </lockinfo>
	res = xmlTextWriterEndElement(writer);

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_ELEMENT_END)

	// document end
	res = xmlTextWriterEndDocument(writer);

#ifdef DEBUG
	printf("%s", buffer->content);
#endif

	if (res < 0)
		ERROR_EXIT("%s\n", ERROR_DOC_END)

	int req_len = strlen((char*)buffer->content);
	char* req = (char*)malloc(req_len);
	memset(req, '\0', req_len);

	memcpy(req, (char*)buffer->content, req_len);

	xmlFreeTextWriter(writer);

	return req;
}
