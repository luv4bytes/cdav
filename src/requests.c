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

char*
cdav_req_propfind(CDAV_PROP** properties)
{
	int res;
	xmlBufferPtr buffer = xmlBufferCreate();

	if (buffer == NULL)
		error_exit("Error creating xmlBufferPtr! - Exiting");

	xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

	if (writer == NULL)
		error_exit("Error creating xmlTextWriterPtr! - Exiting");

	char enc[] = "UTF-8";
	res = xmlTextWriterStartDocument(writer, NULL, enc, NULL);

	if (res < 0)
		error_exit("Error writing document start! - Exiting");

	res = xmlTextWriterEndDocument(writer);

	if (res < 0)
		error_exit("Error writing document end! - Exiting");

	// TODO:

	return NULL;
}
