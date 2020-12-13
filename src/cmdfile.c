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

CMDFILE_TOKEN
new_token(CMDFILE_TOKEN_TYPE type, const char* value)
{
	CMDFILE_TOKEN tok;

	tok.type = type;
	tok.value = (char*) calloc(0, strlen(value));
	strcpy(tok.value, value);

	return tok;
}

CMDFILE_TOKEN*
parse_cmdfile(FILE* file)
{
	CMDFILE_TOKEN* tokens = NULL;
	size_t tokencount = 0;

	char* line = NULL;
	size_t sz = 0;
	ssize_t read = 0;

	char symbol[NAME_LEN];
	memset(symbol, 0, NAME_LEN);

	while( (read = getline(&line, &sz, file)) != -1)	
	{
		if (line[0] == '\n')
		{
			free(line);
			line = NULL;
			continue;
		}

		for(size_t i = 0; i < read; i++)
		{
			if (line[i] == '\n')
			{
				free(line);
				line = NULL;
				break;
			}

			if (line[i] == CMDTOK_COMMENTSTART)
			{
				free(line);
				line = NULL;
				break;
			}

			if (line[i] == CMDTOK_CMDSTART)
			{
				CMDFILE_TOKEN tok = new_token(CMD_START, &line[i]);
				
				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}

			if (line[i] == CMDTOK_CMDEND)
			{
				CMDFILE_TOKEN tok = new_token(CMD_END, &line[i]);

				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}	

			if (line[i] == CMDTOK_ASSIGN)
			{
				CMDFILE_TOKEN tok = new_token(CMD_ASSIGN, &line[i]);

				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}

			// TODO: Extract symbols

			symbol[i] = line[i];
		}

		if (symbol[0] != 0)
		{
			CMDFILE_TOKEN tok = new_token(CMD_NAME, symbol);

			tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
			tokens[tokencount - 1] = tok;

			memset(symbol, 0, NAME_LEN);
		}

		if (line != NULL)
			free(line);

		// TODO: Parse
	}

	if (read == -1)
	{
		int err = errno;
		char* errstr = strerror(err);

		error_exit(errstr);
	}

	#ifdef DEBUG
		for(size_t i = 0; i < tokencount; i++)
			printf("%s\n", tokens[i].value);
	#endif

	return tokens;
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

	CMDFILE_TOKEN* tokens = parse_cmdfile(cdavfile);	

	// TODO: Exec

	fclose(cdavfile);
}
