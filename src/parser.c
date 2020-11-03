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

#include "../include/parser.h"

void
init_args(ARGS* args)
{
	if (args == NULL)
		return;

	args->file = NULL;
	args->operation = NULL;
	args->address = NULL;
	args->user = NULL;
	args->passwd = NULL;
	args->props = NULL;
	args->set_props = NULL;
	args->rm_props = NULL;
	args->destination = NULL;
	args->overwrite = 1;
	args->upload_file = NULL;
	args->save_as = NULL;
	args->depth = NULL;
	args->help = 0;
	args->version = 0;
}
