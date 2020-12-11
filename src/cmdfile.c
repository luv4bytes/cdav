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

int
isCdavFile(FILE* file)
{
	if (file == NULL)
		return -1;

	char* line = NULL;
	size_t cnt = 0;

	ssize_t read = getline(&line, &cnt, file);

	if (read == -1)
	{
		int err = errno;
		char* errstr = strerror(err);

		error_exit(errstr);
	}

	if (strstr(line, EXEC_DIRECTIVE) == NULL)
	{
		free(line);
		return -1;
	}

	return 0;
}

void
exec_cmdfile(const char* file)
{
	if (file == NULL)
		error_exit(PROVIDE_COMMANDFILE);

	if (access(file, R_OK) != 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		error_exit(errstr);
	}

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

	if (isCdavFile(cdavfile) == -1)
		error_exit(INVALID_COMMANDFILE);

	fclose(cdavfile);

	// TODO: Exec
}
