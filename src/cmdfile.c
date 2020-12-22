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
is_cmd_file(FILE* file)
{
	int ret = -1;

	if (file == NULL)
		return ret;

	char* line = NULL;
	size_t cnt = 0;

	ssize_t read = getline(&line, &cnt, file);

	if (read == -1)
	{
		int err = errno;
		char* errstr = strerror(err);

		ERROR_EXIT("%s\n", errstr)
	}

	if (strstr(line, EXEC_DIRECTIVE) == NULL)
		ret = -1;

	free(line);

	return 0;
}

CMDFILE_TOKEN
new_token_str(CMDFILE_TOKEN_TYPE type, const char* value)
{
	CMDFILE_TOKEN tok;

	tok.type = type;
	tok.value = (char*) calloc(strlen(value), sizeof(char));
	strcat(tok.value, value);

	return tok;
}

CMDFILE_TOKEN
new_token_char(CMDFILE_TOKEN_TYPE type, char value)
{
	CMDFILE_TOKEN tok;

	tok.type = type;
	tok.value = (char*) calloc(1, sizeof(char));
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
			continue;

		for(size_t i = 0; i < read; i++)
		{
			if (line[i] == ' ')
				continue;

			if (line[i] == '\n')
				break;

			if (line[i] == '\t')
				continue;

			if (line[i] == CMDTOK_COMMENTSTART)
				break;

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

						tok = new_token_char(CMD_VALUEIDENT, line[i]);
				
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
	}

	if (line != NULL)
		free(line);

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

	vars->names = (char**) realloc(vars->names, sizeof(char*) * vars->count);
	vars->values = (char**) realloc(vars->values, sizeof(char*) * vars->count);
	
	vars->count++;

	vars->names[vars->count - 1] = (char*) calloc(strlen(name), sizeof(char));
	strcat(vars->names[vars->count - 1], name);

	vars->values[vars->count - 1] = (char*) calloc(strlen(name), sizeof(char));
	strcat(vars->values[vars->count - 1], value);
}

char*
var_get_value(char* variable)
{
	if (variable == NULL)
		return NULL;

	size_t len = strlen(variable);

	char buffer[len];
	memset(buffer, 0, len);

	str_cpy_from_to(buffer, variable, 1, len);

	for(size_t i = 0; i < variables.count; i++)
	{
		#ifdef DEBUG
			printf("%s - %s\n", variables.names[i], variables.values[i]);
		#endif

		if (strcmp(variables.names[i], buffer) == 0)
			return variables.values[i];
	}

	return NULL;
}

void
cmd_set_arg(CMDBLOCK* block, char* arg, char* value)
{
	if (block == NULL)
		return;

	if (strcmp(arg, EXEC_ORDER) == 0)
	{
		if (value == NULL)
		{
			block->executionOrder = NULL;
			return;
		}

		block->executionOrder = value;

		return;
	}

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

	if (strcmp_from_to(arg, 0, strlen(arg), ARG_RAW, 2, strlen(ARG_RAW)) == 0)
	{
		block->args.raw = 1;
		return;
	}
}

CMDBLOCK*
parse_tokens(CMDFILE_TOKEN* tokens, size_t count, size_t* createdBlocks)
{
	if (tokens == NULL)
		ERROR_EXIT("No tokens for parsing.");

	CMDBLOCK* blocks = NULL;
	size_t blockCount = 0;
	CMDBLOCK lastBlock;
	
	args_init(&lastBlock.args);
	lastBlock.executionOrder = NULL;

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

				if (next->type == CMD_VALUEIDENT)
				{
					if ((ind + 1) > count || (ind + 2) > count)
						ERROR_EXIT("%s\n", "Error on value identifiers. Please check assignments.");

					CMDFILE_TOKEN* val = &tokens[ind + 1];
					CMDFILE_TOKEN* end = &tokens[ind + 2];
				
					if (end->type == CMD_VALUEIDENT)
					{
						if (lastBlock.type == VAR)
						{
							var_add_variable(&variables, before->value, val->value);
							break;
						}

						cmd_set_arg(&lastBlock, before->value, val->value);
						break;	
					}
				}

				if (next->value[0] == '$')
				{
					char* val = var_get_value(next->value);

					if (val == NULL)
						ERROR_EXIT("Given variable %s is not a valid variable.\n", next->value)

					free(next->value);
					next->value = val;
				}

				if (lastBlock.type == VAR)
				{ 
					var_add_variable(&variables, before->value, next->value);
					break;
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

				*createdBlocks = blockCount;

				args_init(&lastBlock.args);
				lastBlock.executionOrder = NULL;

				break;

			case CMD_NAME:

				if (next->type == CMD_ASSIGN_END && before->type == CMD_ASSIGN_END)
				{
						cmd_set_arg(&lastBlock, current->value, NULL);
						break;	
				}

			default:
				break;
		}
	}
	
	#ifdef DEBUG
		for(int i = 0; i < blockCount; i++)
			printf("%s\n", blocks[i].name);
	#endif 

	return blocks;
}

int
sort_cmdblocks(const void* a, const void* b)
{
	CMDBLOCK* a_block = (CMDBLOCK*)a;
	CMDBLOCK* b_block = (CMDBLOCK*)b;

	if (a_block->executionOrder == NULL)
		return 1;

	if (b_block->executionOrder == NULL)
		return -1;

	int order_a = atoi(a_block->executionOrder);
	int order_b = atoi(b_block->executionOrder);

	return order_a < order_b ? -1 : 1;
}

void*
start_block(void* arg_as_block)
{
	CMDBLOCK* block = (CMDBLOCK*)arg_as_block;

	ARGS args = block->args;
	CDAV_BASIC_PARAMS params;

	params.follow_redirect = args.follow_redirect;
	params.passwd = args.passwd;
	params.proxy = args.proxy;
	params.url = args.address;
	params.user = args.user;
	params.raw = args.raw;

	switch(args_eval_op(args.operation))
	{
		case GET:

			cdav_get(&params, args.save_as);

			break;

		case HEAD:

			cdav_head(&params);

			break;

		case PUT:

			cdav_put(&params, args.upload_file);

			break;

		case PROPFIND:
		{
			CDAV_PROP** prop_list = NULL;

			int count = 0;
			prop_list = cdav_parse_props(args.props, &count);

			cdav_propfind(&params, prop_list, count, args.depth);

			break;
		}

		case PROPPATCH:
		{
			CDAV_PROP** sprops_list = NULL;
			int sprop_count = 0;

			CDAV_PROP** rprops_list = NULL;
			int rprop_count = 0;

			sprops_list = cdav_parse_props(args.set_props, &sprop_count);
			rprops_list = cdav_parse_props(args.rm_props, &rprop_count);

			cdav_proppatch(&params, sprops_list, sprop_count, rprops_list, rprop_count);

			break;
		}

		case MKCOL:

			cdav_mkcol(&params);

			break;

		case DELETE:

			cdav_delete(&params);

			break;

		case COPY:

			cdav_copy(&params, args.destination, args.overwrite);

			break;

		case MOVE:

			cdav_move(&params, args.destination, args.overwrite);

			break;

		case LOCK:

			cdav_lock(&params, args.lock_scope, args.lock_owner, args.depth);

			break;

		case UNLOCK:

			cdav_unlock(&params, args.lock_token);

			break;

		case UNKNOWN:
			ERROR_EXIT("%s\n", UNKNOWN_OPERATION);

		default:
			ERROR_EXIT("%s\n", UNKNOWN_OPERATION);
	}

	return NULL;
}

void
free_variables()
{
	for(size_t i = 0; i < variables.count; i++)
		free(variables.names[i]), free(variables.values[i]);
}

void
free_tokens(CMDFILE_TOKEN* tokens, size_t tokenCount)
{
	for(size_t i = 0; i < tokenCount; i++)
		free_token(&tokens[i]);
}

void
exec_cmdblocks(CMDBLOCK* blocks, size_t count)
{
	if (count < 1)
		return;

	for(size_t i = 0; i < count; i++)
	{
		if (blocks[i].type != VAR && blocks[i].executionOrder == NULL)
			ERROR_EXIT("Block %s needs to have an order defined.", blocks[i].name)
	}

	qsort(blocks, count, sizeof(CMDBLOCK), &sort_cmdblocks);

	#ifdef DEBUG
		for(size_t i = 0; i < count; i++)
			printf("Name: %s - Order: %s\n", blocks[i].name, blocks[i].executionOrder);
	#endif

	pthread_t threads[count];
	memset(threads, 0, count);
	size_t ind = 0;

	int lastOrder = 0;
	int currentOrder = 0;

	for(size_t i = 0; i < count; i++)
	{
		if (blocks[i].type != COMMAND)
			continue;

		currentOrder = atoi(blocks[i].executionOrder);

		if (currentOrder != lastOrder)
		{
			for(size_t j = 0; j < ind; j++)
				pthread_join(threads[j], NULL);

			memset(threads, 0, count);
			ind = 0;
		}

		pthread_t t;
		pthread_create(&t, NULL, &start_block, (void*)(&blocks[i]));
		threads[ind++] = t;

		lastOrder = currentOrder;
	}

	for(size_t i = 0; i < ind; i++)
		pthread_join(threads[i], NULL);
}

void
exec_cmdfile(const char* file)
{
	if (file == NULL)
		ERROR_EXIT("%s\n", PROVIDE_COMMANDFILE)

	if (access(file, R_OK) != 0)
	{
		int err = errno;
		const char* errstr = strerror(err);

		ERROR_EXIT("%s\n", errstr)
	}

	FILE* cdavfile;

	size_t fsz = file_size(file);

	if (fsz == -1)
	{
		int err = errno;
		ERROR_EXIT("%s\n", strerror(err));
	}

	cdavfile = fopen(file, "r");

	if (cdavfile == NULL)
	{
		int err = errno;
		ERROR_EXIT("%s\n", strerror(err))
	}

	if (is_cmd_file(cdavfile) == -1)
		ERROR_EXIT("%s\n", INVALID_COMMANDFILE)

	size_t tokenCount = 0;
	size_t blockCount = 0;

	CMDFILE_TOKEN* tokens = lex_cmdfile(cdavfile, &tokenCount);	
	CMDBLOCK* blocks = parse_tokens(tokens, tokenCount, &blockCount);

	exec_cmdblocks(blocks, blockCount);	

	free_variables();
	free_tokens(tokens, tokenCount);

	fclose(cdavfile);
}
