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

	/* TODO:
	Valueident
	Escaped chars
	*/

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
				int breakout = 0;

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

				for(int j = i + 1; j < read; j++)
				{
					if (line[j] == CMDTOK_VALUEIDENT)
					{
						CMDFILE_TOKEN tok = new_token_str(CMD_NAME, symbol);

						tokens = (CMDFILE_TOKEN*) realloc(tokens, sizeof(CMDFILE_TOKEN) * ++tokencount);
						tokens[tokencount - 1] = tok;

						memset(symbol, 0, NAME_LEN);

						symbolIndex = 0; 

						breakout = 1;
						break;
					}

					symbol[symbolIndex++] = line[j];
				}

				if (breakout == 1)
					break;

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

void
var_add_variable(VARIABLES* vars, const char* name, const char* value)
{
	if (vars == NULL)
		return;

	vars->names[vars->lastIndex] = (char*) calloc(0, sizeof(char));
	strcpy(vars->names[vars->lastIndex], name);

	vars->values[vars->lastIndex] = (char*) calloc(0, sizeof(char));
	strcpy(vars->values[vars->lastIndex], value);

	vars->lastIndex++;
}

void
cmd_set_arg(CMDBLOCK* block, char* arg, char* value)
{
	if (block == NULL)
		return;

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_F_LONG, 2, strlen(ARG_F_LONG)) == 0)
	{
		block->args.file = value;
		return;
	}
		
	if (strcmp_from_to(arg, 0, strlen(arg), ARG_O_LONG, 2, strlen(ARG_O_LONG)) == 0)
	{
		block->args.operation = value;
		return;
	}
		
	if (strcmp_from_to(arg, 0, strlen(arg), ARG_A_LONG, 2, strlen(ARG_A_LONG)) == 0)
	{
		block->args.address = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_U_LONG, 2, strlen(ARG_U_LONG)) == 0)
	{
		block->args.user = value;
		return;
	}
		
	if (strcmp_from_to(arg, 0, strlen(arg), ARG_PW_LONG, 2, strlen(ARG_PW_LONG)) == 0)
	{
		block->args.passwd = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_P_LONG, 2, strlen(ARG_P_LONG)) == 0)
	{
		block->args.props = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_SP_LONG, 2, strlen(ARG_SP_LONG)) == 0)
	{
		block->args.set_props = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_RP_LONG, 2, strlen(ARG_RP_LONG)) == 0)
	{
		block->args.rm_props = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_DA_LONG, 2, strlen(ARG_DA_LONG)) == 0)
	{
		block->args.destination = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_NO_OW, 2, strlen(ARG_NO_OW)) == 0)
	{
		block->args.overwrite = 0;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_UF_LONG, 2, strlen(ARG_UF_LONG)) == 0)
	{
		block->args.upload_file = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_S_LONG, 2, strlen(ARG_S_LONG)) == 0)
	{
		block->args.save_as = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_D_LONG, 2, strlen(ARG_D_LONG)) == 0)
	{
		block->args.depth = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_LS_LONG, 2, strlen(ARG_LS_LONG)) == 0)
	{
		block->args.lock_scope = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_LT_LONG, 2, strlen(ARG_LT_LONG)) == 0)
	{
		block->args.lock_token = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_LO_LONG, 2, strlen(ARG_LO_LONG)) == 0)
	{
		block->args.lock_owner = value;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_H_LONG, 2, strlen(ARG_H_LONG)) == 0)
	{
		block->args.help = 1;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_V_LONG, 2, strlen(ARG_V_LONG)) == 0)
	{
		block->args.version = 1;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_NO_REDIRECT, 2, strlen(ARG_NO_REDIRECT)) == 0)
	{
		block->args.follow_redirect = 0;
		return;
	}

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_PROXY, 2, strlen(ARG_PROXY)) == 0)
	{
		block->args.proxy = value;
		return;
	}

	// TODO: Malloc because of freed tokens?
}

CMDBLOCK*
parse_tokens(CMDFILE_TOKEN* tokens, size_t count)
{
	if (tokens == NULL)
		error_exit("No tokens for parsing.");

	CMDBLOCK* blocks = NULL;
	size_t blockCount = 0;
	CMDBLOCK lastBlock;
	
	init_args(&lastBlock.args);

	for(size_t i = 0; i < count; i++)
	{
		CMDFILE_TOKEN* current = &tokens[i];

		int ind = i > 0 ? i - 1 : i;
		CMDFILE_TOKEN* before = &tokens[ind];

		ind = (i == (i - 1)) ? i : i + 1;
		CMDFILE_TOKEN* next = &tokens[ind];

		if (current->value == NULL)
			continue;

		switch(current->type)
		{
			case CMD_ASSIGN:

				if (lastBlock.type == VAR)
				{
					var_add_variable(&variables, before->value, next->value);
					break;
				}

				if (next->type == CMD_VALUEIDENT)
				{
					CMDFILE_TOKEN* val = &tokens[ind + 1];
					CMDFILE_TOKEN* end = &tokens[ind + 2];

					// TODO: Values are not right yet

					if (end->type == CMD_VALUEIDENT)
					{
						cmd_set_arg(&lastBlock, before->value, val->value);
						break;	
					}
				}

				cmd_set_arg(&lastBlock, before->value, next->value);
				break;

			case CMD_START:
			{
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

					default:
						break;
				}

				break;
			}

			case CMD_END:
				
				blocks = (CMDBLOCK*) realloc(blocks, sizeof(CMDBLOCK) * ++blockCount);
				blocks[blockCount - 1] = lastBlock;

				init_args(&lastBlock.args);

				break;

			default:
				break;
		}
	}
	
	/* TODO: 
	Variables '$'
	Variable array not fixed size
	*/

	#ifdef DEBUG
		for(int i = 0; i < blockCount; i++)
			printf("%s\n", blocks[i].name);
	#endif 

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
