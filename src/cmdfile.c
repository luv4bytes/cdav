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
new_token_str(CMDFILE_TOKEN_TYPE type, const char* value)
{
	CMDFILE_TOKEN tok;

	tok.type = type;
	tok.value = (char*) calloc(0, strlen(value));
	strcpy(tok.value, value);

	return tok;
}

CMDFILE_TOKEN
new_token_char(CMDFILE_TOKEN_TYPE type, char value)
{
	CMDFILE_TOKEN tok;

	tok.type = type;
	tok.value = (char*) calloc(0, sizeof(char));
	tok.value[0] = value;

	return tok;
}

void
free_token(CMDFILE_TOKEN* token)
{
	#ifdef DEBUG
		printf("%s\n", token->value);
	#endif

	if (token->value != NULL)
		free(token->value);
}

CMDFILE_TOKEN*
lex_cmdfile(FILE* file, size_t* count)
{
	CMDFILE_TOKEN* tokens = NULL;
	size_t tokencount = 0;

	char* line = NULL;
	size_t sz = 0;
	ssize_t read = 0;

	size_t symbolIndex = 0;

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

			if (line[i] == '\t')
				symbolIndex--;

			if (line[i] == CMDTOK_COMMENTSTART)
			{
				free(line);
				line = NULL;
				break;
			}

			if (line[i] == CMDTOK_VALUEIDENT)
			{
				if (symbol[0] != 0)
				{
					CMDFILE_TOKEN tok = new_token_str(CMD_NAME, symbol);

					tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
					tokens[tokencount - 1] = tok;

					memset(symbol, 0, NAME_LEN);

					symbolIndex = 0;
				}

				CMDFILE_TOKEN tok = new_token_char(CMD_VALUEIDENT, line[i]);
				
				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}

			if (line[i] == CMDTOK_CMDSTART)
			{
				if (symbol[0] != 0)
				{
					CMDFILE_TOKEN tok = new_token_str(CMD_NAME, symbol);

					tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
					tokens[tokencount - 1] = tok;

					memset(symbol, 0, NAME_LEN);

					symbolIndex = 0;
				}

				CMDFILE_TOKEN tok = new_token_char(CMD_START, line[i]);
				
				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}

			if (line[i] == CMDTOK_CMDEND)
			{
				if (symbol[0] != 0)
				{
					CMDFILE_TOKEN tok = new_token_str(CMD_NAME, symbol);

					tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
					tokens[tokencount - 1] = tok;

					memset(symbol, 0, NAME_LEN);

					symbolIndex = 0;
				}

				CMDFILE_TOKEN tok = new_token_char(CMD_END, line[i]);

				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}	

			if (line[i] == CMDTOK_ASSIGN)
			{
				if (symbol[0] != 0)
				{
					CMDFILE_TOKEN tok = new_token_str(CMD_NAME, symbol);

					tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
					tokens[tokencount - 1] = tok;

					memset(symbol, 0, NAME_LEN);

					symbolIndex = 0;
				}

				CMDFILE_TOKEN tok = new_token_char(CMD_ASSIGN, line[i]);

				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}

			if (line[i] == CMDTOK_ASSIGN_END)
			{
				if (symbol[0] != 0)
				{
					CMDFILE_TOKEN tok = new_token_str(CMD_NAME, symbol);

					tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
					tokens[tokencount - 1] = tok;

					memset(symbol, 0, NAME_LEN);

					symbolIndex = 0;
				}

				CMDFILE_TOKEN tok = new_token_char(CMD_ASSIGN_END, line[i]);

				tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
				tokens[tokencount - 1] = tok;

				continue;
			}

			symbol[symbolIndex++] = line[i];
		}

		if (line != NULL)
			free(line);
	}

	#ifdef DEBUG
		if (tokens != NULL)
		{
			for(size_t i = 0; i < tokencount; i++)
				printf("%s\n", tokens[i].value);
		}	
	#endif

	*count = tokencount;

	return tokens;
}

CMDBLOCK*
parse_tokens(CMDFILE_TOKEN* tokens, size_t count)
{
	if (tokens == NULL)
		error_exit("No tokens for parsing.");

	CMDBLOCK* blocks = NULL;
	CMDBLOCK lastBlock;

	for(size_t i = 0; i < count; i++)
	{
		CMDFILE_TOKEN* current = &tokens[i];

		if (current->value == NULL)
			continue;

		switch(current->type)
		{
			case CMD_DEFAULT:
				break;

			case CMD_ASSIGN:
				break;

			case CMD_ASSIGN_END:
				break;

			case CMD_NAME:
				break;

			case CMD_VALUEIDENT:
				break;

			case CMD_START:
			{
				if (i == 0)
					break;

				CMDFILE_TOKEN* before = &tokens[i - 1];

				switch (before->type)
				{
					case CMD_NAME:

						if (strcmp(before->value, BLOCK_VAR) == 0)
						{
							lastBlock.type = VAR;
							lastBlock.name = BLOCK_VAR;
						}							
						else
						{
							lastBlock.type = COMMAND;
							lastBlock.name = before->value;
						}

						// TODO: Parse

					default:
						break;
				}

				break;
			}

			case CMD_END:
				break;

			default:
				break;
		}
	}
	
	// TODO: Special blocks

	return blocks;
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

	size_t count = 0;

	CMDFILE_TOKEN* tokens = lex_cmdfile(cdavfile, &count);	
	CMDBLOCK* blocks = parse_tokens(tokens, count);

	// TODO: Execute blocks
	// TODO: Free tokens

	fclose(cdavfile);
}
