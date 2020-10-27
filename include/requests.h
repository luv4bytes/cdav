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

#ifndef REQUESTS_H
#define REQUESTS_H

#include "cdav.h"
#include "prop.h"
#include "err.h"

#include <libxml/xmlwriter.h>

/// Writes 
void
cdav_write_prop(CDAV_PROP* prop, xmlTextWriterPtr writer);

/// Creates a WebDAV PROPFIND request.
char*
cdav_req_propfind(CDAV_PROP** properties, size_t count);

/// Creates a WebDAV PROPPATCH request.
char*
cdav_req_proppatch(CDAV_PROP** set_props, size_t set_count, CDAV_PROP** rm_props, size_t rm_count);

#endif // REQUESTS_H
