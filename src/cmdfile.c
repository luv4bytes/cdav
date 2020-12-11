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

#include "../include/cmdfile.h"

void
exec_cmdfile(const char* file)
{
	if (file == NULL)
		error_exit(PROVIDE_COMMANDFILE);

	FILE* cdavfile;

	size_t fsz = file_size(file);

	if (fsz == -1)
	{
		int err = errno;
		error_exit(strerror(err));
	}

	cdavfile = fopen(file, "r");

	if (cdavfile == NULL)
	{
		int err = errno;
		error_exit(strerror(err));
	}

	char* buffer = (char*)malloc(sizeof(char) * fsz);

	if (fread(buffer, sizeof(char), fsz, cdavfile) <= 0)
	{
		if (ferror(cdavfile) != 0)
		{
			int err = errno;
			error_exit(strerror(err));
		}
	}

#ifdef DEBUG
	printf("%s\n", buffer);
#endif

	fclose(cdavfile);

	// TODO: Exec

	free(buffer);
}
